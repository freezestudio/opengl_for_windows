#include <common.h>
#include "renderer.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

////////////////////////////////////////////////////////////
std::shared_ptr<renderer> renderer::ms_instance = nullptr;
////////////////////////////////////////////////////////////

renderer::renderer()
    : scene_camera{0.0f,0.0f,3.0f}
    , last_x(SCR_WIDTH/2.0f)
    , last_y(SCR_HEIGHT/2.0f)
    , first_mouse(true)
    , delta_time(0.0f)
    , last_frame(0.0f)
{    
}

std::shared_ptr<renderer> renderer::instance()
{ 
    assert(ms_instance);
    return ms_instance;
}

void renderer::init()
{
    ms_instance = shared_from_this();

    freeze::depth::enable();

    set_vertices();
    set_textures();
    set_shaders();
}

void renderer::draw()
{
    auto light_position = glm::vec3{ -2.0f,4.0f,-1.0f };
    float near_plane = 1.0f;
    float far_plane = 7.5f;

    auto light_proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    auto light_view = glm::lookAt(light_position, glm::vec3{ 0.0f }, glm::vec3{ 0.0f,1.0f,0.0f });
    depth_shader.use();
    depth_shader.set_mat4("lightSpaceMatrix"s, light_proj * light_view);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    depth_fbo.bind();
    {
        freeze::depth::clear();
        plane_texture.bind();
        plane_texture.active();

        draw_scene(depth_shader);
    }
    depth_fbo.unbind();

    ////////////////////////////////////////////////////

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ////////////////////////////////////////////////////

    auto proj = glm::perspective(glm::radians(scene_camera.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
    auto view = scene_camera.get_view_matrix();

    shadow_shader.use();
    shadow_shader.set_mat4("projection"s, proj);
    shadow_shader.set_mat4("view"s, view);
    shadow_shader.set_mat4("lightSpaceMatrix"s, light_proj * light_view);
    shadow_shader.set_vec3("lightPos"s, light_position);
    shadow_shader.set_vec3("viewPos"s, scene_camera.Position);
    shadow_shader.set_float("shininess"s, 64.0f);

    plane_texture.active();
    plane_texture.bind();
    depth_texture.active(1);
    depth_texture.bind();

    draw_scene(shadow_shader);

}

void renderer::process_event(glfw::window::pointer window)
{
    auto deltaTime = instance()->delta_time;

    auto key = glfw::get_key(window, GLFW_KEY_ESCAPE);
    if (key == GLFW_PRESS)
    {
        glfw::set_window_should_close(window);
    }

    key = glfw::get_key(window, GLFW_KEY_W);
    if (key == GLFW_PRESS)
    {
        scene_camera.process_keyboard(freeze::camera_movement::FORWARD, deltaTime);
    }

    key = glfw::get_key(window, GLFW_KEY_S);
    if (key == GLFW_PRESS)
    {
        scene_camera.process_keyboard(freeze::camera_movement::BACKWARD, deltaTime);
    }

    key = glfw::get_key(window, GLFW_KEY_A);
    if (key == GLFW_PRESS)
    {
        scene_camera.process_keyboard(freeze::camera_movement::LEFT, deltaTime);
    }

    key = glfw::get_key(window, GLFW_KEY_D);
    if (key == GLFW_PRESS)
    {
        scene_camera.process_keyboard(freeze::camera_movement::RIGHT, deltaTime);
    }
}

void renderer::framebuffer_size_callback(glfw::window::pointer window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void renderer::mouse_callback(glfw::window::pointer window, double xpos, double ypos)
{
    if (instance()->first_mouse)
    {
        instance()->last_x = xpos;
        instance()->last_y = ypos;
        instance()->first_mouse = false;
    }

    auto xoffset = xpos - instance()->last_x;
    auto yoffset = instance()->last_y - ypos;

    instance()->last_x = xpos;
    instance()->last_y = ypos;

    instance()->scene_camera.process_mouse_movement(xoffset, yoffset);
}

void renderer::scroll_callback(glfw::window::pointer window, double xoffset, double yoffset)
{
    instance()->scene_camera.process_mouse_scroll(yoffset);
}

void renderer::set_vertices()
{
    set_plane();
    set_cube();
}

void renderer::set_shaders()
{
    shadow_shader.compile_file("resource/shaders/shadow.vert"s, "resource/shaders/shadow.frag"s);
    shadow_shader.use();
    shadow_shader.set_int("diffuseTexture"s, 0);
    shadow_shader.set_int("shadowMap"s, 1);

    depth_shader.compile_file("resource/shaders/depth.vert"s, "resource/shaders/depth.frag"s);
    
}

void renderer::set_textures()
{
    //阴影深度纹理
    depth_texture.bind();
    depth_texture.set_image(GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    depth_texture.set_min_filter(GL_NEAREST);
    depth_texture.set_mag_filter(GL_NEAREST);
    depth_texture.set_wrap_s(GL_CLAMP_TO_BORDER);
    depth_texture.set_wrap_t(GL_CLAMP_TO_BORDER);
    float colors[] = { 1.0f,1.0f,1.0f,1.0f };
    depth_texture.set_border_color(colors);
    depth_texture.unbind();

    //阴影深度缓冲
    depth_fbo.bind();
    depth_fbo.attachement_depth(depth_texture.ref());
    depth_fbo.draw_buffer(GL_NONE);
    depth_fbo.read_buffer(GL_NONE);
    depth_fbo.unbind();

    //地板纹理
    plane_texture.bind();
    plane_texture.set_image(freeze::load_image_from_file("resource/textures/wood.png"s));
    plane_texture.mipmap();
    plane_texture.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
    plane_texture.set_mag_filter(GL_LINEAR);
    plane_texture.set_wrap_s(plane_texture.get_format() == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    plane_texture.set_wrap_t(plane_texture.get_format() == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    plane_texture.unbind();
}

void renderer::set_plane()
{
    GLfloat plane_vertices[] = {
        // positions            // normals         // texcoords
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f,
    };

    plane_vao.bind();
    auto vbo = freeze::make_vertex_buffer();
    vbo.bind();
    vbo.copy_data(plane_vertices, sizeof(plane_vertices));
    auto plane_vertex = freeze::make_vertex();
    plane_vertex.set(0, 3, GL_FLOAT, 8 * sizeof(GLfloat), 0);
    plane_vertex.set(1, 3, GL_FLOAT, 8 * sizeof(GLfloat), 3 * sizeof(GLfloat));
    plane_vertex.set(2, 2, GL_FLOAT, 8 * sizeof(GLfloat), 6 * sizeof(GLfloat));
    vbo.unbind();
    plane_vao.unbind();
}

void renderer::set_cube()
{
    float cube_vertices[] = {
        // back face
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
        // front face
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        // right face
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
        // bottom face
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
         1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
         1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
    };

    cube_vao.bind();
    auto vao = freeze::make_vertex_buffer();
    vao.bind();
    vao.copy_data(cube_vertices, sizeof(cube_vertices));
    auto cube_vertex = freeze::make_vertex();
    cube_vertex.set(0, 3, GL_FLOAT, 8 * sizeof(float), 0);
    cube_vertex.set(1, 3, GL_FLOAT, 8 * sizeof(float), 3 * sizeof(float));
    cube_vertex.set(2, 2, GL_FLOAT, 8 * sizeof(float), 6 * sizeof(float));
    vao.unbind();
    cube_vao.unbind();
}

void renderer::draw_scene(freeze::program& shader)
{
    //floor
    auto model = glm::mat4{ 1.0f };
    shader.set_mat4("model"s, model);

    plane_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    plane_vao.unbind();

    //cubes
    model = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ 0.0f,1.5f,0.0f });
    model = glm::scale(model, glm::vec3{ 0.5f });
    shader.set_mat4("model"s, model);
    draw_cube();

    model = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ 2.0f,0.0f,1.0f });
    model = glm::scale(model, glm::vec3{ 0.5f });
    shader.set_mat4("model"s, model);
    draw_cube();

    model = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ -1.0f,0.0f,2.0f });
    model = glm::scale(model, glm::vec3{ 0.25f });
    shader.set_mat4("model"s, model);
    draw_cube();

}

void renderer::draw_cube()
{
    cube_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    cube_vao.unbind();
}



