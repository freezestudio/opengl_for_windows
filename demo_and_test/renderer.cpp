#include <common.h>
#include "renderer.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

////////////////////////////////////////////////////////////
std::shared_ptr<renderer> renderer::ms_instance = nullptr;
////////////////////////////////////////////////////////////

renderer::renderer()
    : scene_camera{0.0f,0.0f,3.0f}
    , last_x(300.f)
    , last_y(200.f)
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
    draw_plane();
    //cubes - 2
    draw_cube();
    draw_quad();
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
    set_quad();
}

void renderer::set_shaders()
{
    shadow_shader.compile_file("resource/shaders/shadow.vert"s, "resource/shaders/shadow.frag"s);
    
    quad_shader.compile_file("resource/shaders/quad.vert"s, "resource/shaders/quad.frag"s);
    quad_shader.use();
    quad_shader.set_int("WoodTexture"s, 0);
}

void renderer::set_textures()
{
    shadow_texture.bind();
    shadow_texture.set_image(GL_DEPTH_COMPONENT, 1024, 1024, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    shadow_texture.set_min_filter(GL_NEAREST);
    shadow_texture.set_mag_filter(GL_NEAREST);
    shadow_texture.set_wrap_s(GL_REPEAT);
    shadow_texture.set_wrap_t(GL_REPEAT);
    shadow_texture.unbind();

    shadow_fbo.bind();
    shadow_fbo.attachement_depth(shadow_texture.ref());
    shadow_fbo.draw_buffer(GL_NONE);
    shadow_fbo.read_buffer(GL_NONE);
    shadow_fbo.unbind();

    quad_texture.bind();
    quad_texture.set_image(freeze::load_image_from_file("resource/textures/wood.png"s));
    quad_texture.mipmap();
    quad_texture.set_min_filter(GL_LINEAR);
    quad_texture.set_mag_filter(GL_LINEAR);
    quad_texture.set_wrap_s(GL_REPEAT);
    quad_texture.set_wrap_t(GL_REPEAT);
    quad_texture.unbind();
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
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 10.0f,
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

void renderer::set_quad()
{
    float quad_vertices[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    quad_vao.bind();
    auto vbo = freeze::make_vertex_buffer();
    vbo.bind();
    vbo.copy_data(quad_vertices, sizeof(quad_vertices));
    auto quad_vertex = freeze::make_vertex();
    quad_vertex.set(0, 3, GL_FLOAT, 5 * sizeof(float), 0);
    quad_vertex.set(1, 2, GL_FLOAT, 5 * sizeof(float), 3 * sizeof(float));
    vbo.unbind();
    quad_vao.unbind();
}

void renderer::draw_plane()
{
    auto light_position = glm::vec3{ -2.0f,4.0f,-1.0f };
    float near_plane = 1.0f;
    float far_plane = 7.5f;

    auto proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    auto view = glm::lookAt(light_position, glm::vec3{ 0.0f }, glm::vec3{ 0.0f,1.0f,0.0f });
    shadow_shader.use();
    shadow_shader.set_mat4("view", proj * view);

    glViewport(0, 0, 1024, 1024);
    shadow_fbo.bind();
    freeze::depth::clear();
    quad_texture.active();
    quad_texture.bind();

    //floor
    auto model = glm::mat4{ 1.0f };
    shadow_shader.set_mat4("model"s, model);
    plane_vao.bind();

    glDrawArrays(GL_TRIANGLES, 0, 6);

    //cubes - 2
    //model = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ 0.0f,1.5f,0.0f });
    //model = glm::scale(model, glm::vec3{ 0.5f });
    //shadow_shader.set_mat4("model"s, model);
    //draw_cube();

    //model = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ 2.0f,0.0f,1.0f });
    //model = glm::scale(model, glm::vec3{ 0.5f });
    //shadow_shader.set_mat4("model"s, model);
    //draw_cube();

    //model = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ -1.0f,0.0f,2.0f });
    //model = glm::scale(model, glm::vec3{ 0.25f });
    //shadow_shader.set_mat4("model"s, model);
    //draw_cube();

    plane_vao.unbind();
    shadow_fbo.unbind();

}

void renderer::draw_cube()
{
    shadow_fbo.bind();
    shadow_shader.use();
    cube_vao.bind();
    quad_texture.active();
    quad_texture.bind();

    auto model = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ 0.0f,1.0f,0.0f });
    model = glm::scale(model, glm::vec3{ 0.5f });
    shadow_shader.set_mat4("model"s, model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ 2.0f,0.0f,1.0f });
    model = glm::scale(model, glm::vec3{ 0.5f });
    shadow_shader.set_mat4("model"s, model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ -1.0f,0.0f,2.0f });
    model = glm::scale(model, glm::vec3{ 0.25f });
    shadow_shader.set_mat4("model"s, model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    cube_vao.unbind();
    shadow_fbo.unbind();

    //cubes - 2
    //cube_vao.bind();
    //glDrawArrays(GL_TRIANGLES, 0, 36);
    //cube_vao.unbind();
}

void renderer::draw_quad()
{
    glViewport(0, 0, 600, 400);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    quad_shader.use();
    quad_shader.set_float("near_plane"s, 1.0f);
    quad_shader.set_float("far_plane"s, 7.5f);
    shadow_texture.active();
    shadow_texture.bind();
    quad_vao.bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    quad_vao.unbind();
}



