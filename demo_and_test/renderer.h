#pragma once

class renderer : public std::enable_shared_from_this<renderer>
{
public:
    renderer();
public:
    void init();
    void draw();
    void process_event(glfw::window::pointer window);
public:
    static std::shared_ptr<renderer> instance();
    static std::shared_ptr<renderer> ms_instance;
public:
    static void framebuffer_size_callback(glfw::window::pointer window, int width, int height);
    static void mouse_callback(glfw::window::pointer window, double xpos, double ypos);
    static void scroll_callback(glfw::window::pointer window, double xoffset, double yoffset);
public:
    float last_x;
    float last_y;
    bool first_mouse;
    float delta_time;
    float last_frame;
private:
    void set_vertices();
    void set_shaders();
    void set_textures();
private:
    void set_plane();
    void set_cube();
    void set_quad();
    void draw_plane();
    void draw_cube();
    void draw_quad();
private:
    freeze::camera              scene_camera;
private:
    freeze::vertex_array_buffer plane_vao;
    freeze::vertex_array_buffer cube_vao;
    freeze::vertex_array_buffer quad_vao;
    freeze::program             shadow_shader;
    freeze::program             quad_shader;
    freeze::frame_buffer        shadow_fbo;
    freeze::texture2d           shadow_texture;
    freeze::texture2d           quad_texture;
};