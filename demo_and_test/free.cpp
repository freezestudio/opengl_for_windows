#include "free.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

renderer::renderer()
    : camera{ 0.0f,0.0f,3.0f }
{

}

void renderer::init()
{
    //深度测试
    freeze::depth::enable();

    //阴影贴图
    depthmap_tex.bind();
    depthmap_tex.set_image(GL_DEPTH_COMPONENT, 1024, 1024, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    depthmap_tex.set_min_filter(GL_NEAREST);
    depthmap_tex.set_mag_filter(GL_NEAREST);
    depthmap_tex.set_wrap_s(GL_REPEAT);
    depthmap_tex.set_wrap_t(GL_REPEAT);
    depthmap_tex.unbind();

    //深度缓冲
    depthmap_fbo.bind();
    depthmap_fbo.attachement_depth(depthmap_tex.ref());
    depthmap_fbo.draw_buffer(GL_NONE);
    depthmap_fbo.read_buffer(GL_NONE);
    depthmap_fbo.unbind();

    bool fbo_test = depthmap_fbo.is_complete();

    //地板贴图
    wood_tex.bind();
    wood_tex.set_image(freeze::set_image_file("resource/textures/wood.png"s));
    wood_tex.mipmap();
    wood_tex.set_wrap_s(GL_REPEAT);
    wood_tex.set_wrap_t(GL_REPEAT);
    wood_tex.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
    wood_tex.set_mag_filter(GL_LINEAR);
    wood_tex.unbind();

    //阴影着色器
    depth_shader.compile_file("resource/shaders/shadow_mapping_depth.vert"s, "resource/shaders/shadow_mapping_depth.frag"s);

    debug_depth_quad_shader.compile_file("resource/shaders/debug_quad.vert"s, "resource/shaders/debug_quad.frag"s);
    debug_depth_quad_shader.use();
    debug_depth_quad_shader.set_int("depthMap", 0);

    //顶点
    set_vertices();
}

void renderer::resize(int width, int height)
{
    glViewport(0, 0, width, height);
}

void renderer::draw()
{
    auto color = freeze::make_color();
    color.set(0.1f, 0.1f, 0.1f, 1.0f);
    color.clear();
    freeze::depth::clear();
    
    float near_plane = 1.0f;
    float far_plane = 7.5f;
    auto light_pos = glm::vec3{ -2.0f,4.0f,-1.0f };

    auto light_proj = glm::ortho(-10.0f, 10.0f, near_plane, far_plane);
    auto light_view = glm::lookAt(light_pos, glm::vec3{ 0.0f }, glm::vec3{ 0.0f,1.0f,0.0f });
    auto light_space_view = light_proj * light_view;

    depth_shader.use();
    depth_shader.set_mat4("lightspaceview"s, light_space_view);

    resize(1024, 1024);
    depthmap_fbo.bind();
    freeze::depth::clear();
    wood_tex.active();
    wood_tex.bind();
    draw_scene();
    depthmap_fbo.unbind();

    resize(800, 600);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    debug_depth_quad_shader.use();
    debug_depth_quad_shader.set_float("nearPlane"s, near_plane);
    debug_depth_quad_shader.set_float("farPlane"s, far_plane);
    depthmap_tex.active();
    depthmap_tex.bind();

    draw_quad();
}

void renderer::key_callback(glfw::window::pointer window, int key, int scancode, int action, int mode)
{

}

void renderer::scroll_callback(glfw::window::pointer window, double xoffset, double yoffset)
{

}

void renderer::mouse_callback(glfw::window::pointer window, double xpos, double ypos)
{

}

void renderer::framebuffer_size_callback(glfw::window::pointer window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void renderer::set_vertices()
{
    GLfloat plane_vertices[] = {
        // Positions           // Normals        // Texture Coords
         25.0f, -0.5f,  25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
        -25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f,  25.0f,
        -25.0f, -0.5f,  25.0f, 0.0f, 1.0f, 0.0f, 0.0f,  0.0f,

         25.0f, -0.5f,  25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
         25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,
        -25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f,  25.0f
    };

    plane_vao.bind();

    auto plane_vbo = freeze::make_vertex_buffer();
    plane_vbo.bind();
    plane_vbo.copy_data(plane_vertices, sizeof(plane_vertices));
    auto plane_vertex = freeze::make_vertex();
    plane_vertex.set(0, 3, GL_FLOAT, 8 * sizeof(GLfloat), 0);
    plane_vertex.set(1, 3, GL_FLOAT, 8 * sizeof(GLfloat), 3 * sizeof(GLfloat));
    plane_vertex.set(2, 2, GL_FLOAT, 8 * sizeof(GLfloat), 6 * sizeof(GLfloat));
    plane_vbo.unbind();

    plane_vao.unbind();

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

    auto cube_vbo = freeze::make_vertex_buffer();
    cube_vbo.bind();
    cube_vbo.copy_data(cube_vertices, sizeof(cube_vertices));
    auto cube_vertex = freeze::make_vertex();
    cube_vertex.set(0, 3, GL_FLOAT, 8 * sizeof(GLfloat), 0);
    cube_vertex.set(1, 3, GL_FLOAT, 8 * sizeof(GLfloat), 3 * sizeof(GLfloat));
    cube_vertex.set(2, 2, GL_FLOAT, 8 * sizeof(GLfloat), 6 * sizeof(GLfloat));
    cube_vbo.unbind();

    cube_vao.unbind();

    float quad_vertices[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };

    quad_vao.bind();

    auto quad_vbo = freeze::make_vertex_buffer();
    quad_vbo.bind();
    quad_vbo.copy_data(quad_vertices, sizeof(quad_vertices));
    auto quad_vertex = freeze::make_vertex();
    quad_vertex.set(0, 3, GL_FLOAT, 5 * sizeof(GLfloat), 0);
    quad_vertex.set(1, 2, GL_FLOAT, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat));
    quad_vbo.unbind();

    quad_vao.unbind();

}


void renderer::draw_scene()
{
    //floor
    depth_shader.use();
    depth_shader.set_mat4("model"s, glm::mat4{ 1.0f });
    plane_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);

    //cube
    auto model = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ 0.0f,1.5f,0.0f });
    model = glm::scale(model, glm::vec3{ 0.5f });
    depth_shader.set_mat4("model"s, model);
    draw_cube();
}

void renderer::draw_cube()
{
    cube_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    cube_vao.unbind();
}

void renderer::draw_quad()
{    
    quad_vao.bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    quad_vao.unbind();
}
