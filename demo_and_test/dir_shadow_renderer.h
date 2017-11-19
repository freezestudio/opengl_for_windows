#pragma once

#include "renderer_base.h"

#ifdef RENDERER
#undef RENDERER
#endif

#define RENDERER dir_shadow_renderer

#define SCR_WIDTH 800
#define SCR_HEIGHT 600
#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

class dir_shadow_renderer
    : public renderer_base<dir_shadow_renderer>
    , public std::enable_shared_from_this<dir_shadow_renderer>
    , public renderer
{
public:
    dir_shadow_renderer();
    virtual ~dir_shadow_renderer();
public:
    void draw();
    void process_event(window_pointer window);
public:
    void do_init();
    void do_mouse_callback(double xpos, double ypos);
    void do_scroll_callback(double xoffset, double yoffset);
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
    void draw_scene(freeze::program& shader);
    void draw_cube();
private:
    freeze::camera              scene_camera;
private:
    freeze::frame_buffer        depth_fbo;
    freeze::vertex_array_buffer plane_vao;
    freeze::vertex_array_buffer cube_vao;
    freeze::program             depth_shader;
    freeze::program             shadow_shader;
    freeze::texture2d           depth_texture;
    freeze::texture2d           plane_texture;
    freeze::delay_model         test_model;
};
