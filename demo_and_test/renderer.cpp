#include <common.h>
#include "renderer.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

renderer::renderer()
    : scene_camera{0.0f,0.0f,3.0f}
{

}

void renderer::init()
{
    set_triangle();
    set_texture();
    set_shader();
}

void renderer::set_triangle()
{
    GLfloat triangles[] = {
        -1.0f,-1.0f, 0.0f,0.0f,
         1.0f,-1.0f, 1.0f,0.0f,
         0.0f, 1.0f, 0.5f, 0.5f,
    };

    triangle_vao.bind();
    auto vbo = freeze::make_vertex_buffer();
    vbo.bind();
    vbo.copy_data(triangles, sizeof(triangles));
    auto triangle_vertex = freeze::make_vertex();
    triangle_vertex.set(0, 2, GL_FLOAT, 4 * sizeof(GLfloat), 0);
    triangle_vertex.set(1, 2, GL_FLOAT, 4 * sizeof(GLfloat), 2 * sizeof(GLfloat));
    vbo.unbind();
    triangle_vao.unbind();
}

void renderer::set_shader()
{
    triangle_shader.compile_file("resource/shaders/triangle.vert"s, "resource/shaders/triangle.frag"s);
    triangle_shader.use();
    triangle_shader.set_int("TriangleTexture"s, 0);
}

void renderer::set_texture()
{
    triangle_texture.bind();
    triangle_texture.set_image(freeze::load_image_from_file("resource/textures/wood.png"s));
    triangle_texture.mipmap();
    triangle_texture.set_min_filter(GL_LINEAR);
    triangle_texture.set_mag_filter(GL_LINEAR);
    triangle_texture.set_wrap_s(GL_REPEAT);
    triangle_texture.set_wrap_t(GL_REPEAT);
    triangle_texture.unbind();
}


void renderer::draw()
{
    triangle_shader.use();
    triangle_texture.active();
    triangle_texture.bind();
    triangle_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    triangle_vao.unbind();
}

void renderer::framebuffer_size_callback(glfw::window::pointer window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void renderer::mouse_callback(glfw::window::pointer window, double xpos, double ypos)
{

}

void renderer::scroll_callback(glfw::window::pointer window, double xoffset, double yoffset)
{

}



