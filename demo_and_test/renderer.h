#pragma once

class renderer
{
public:
    renderer();
public:
    void init();
    void draw();
public:
    static void framebuffer_size_callback(glfw::window::pointer window, int width, int height);
    static void mouse_callback(glfw::window::pointer window, double xpos, double ypos);
    static void scroll_callback(glfw::window::pointer window, double xoffset, double yoffset);

private:
    void set_triangle();
    void set_shader();
    void set_texture();
private:
    freeze::camera              scene_camera;
private:
    freeze::vertex_array_buffer triangle_vao;
    freeze::program             triangle_shader;
    freeze::texture2d           triangle_texture;
};