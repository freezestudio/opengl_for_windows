#pragma once

#include "renderer_base.h"

#ifdef RENDERER
#undef RENDERER
#endif

#define RENDERER cubemap_renderer

#ifdef SCR_WIDTH
#undef SCR_WIDTH
#endif

#ifdef SCR_HEIGHT
#undef SCR_HEIGHT
#endif

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

class cubemap_renderer
    : public renderer_base<cubemap_renderer>
    , public std::enable_shared_from_this<cubemap_renderer>
    , public renderer
{
public:
    cubemap_renderer();
    virtual ~cubemap_renderer();
public:
    void do_init();
    virtual void draw() override;
    void process_event(window_pointer window);
    void do_mouse_callback(freeze::window::pointer window, double xpos, double ypos);
private:
    float width, height;
    float last_x;
    float last_y;
    bool first_mouse;
    float delta_time;
    float last_frame;
    freeze::camera              scene_camera;
private:
    freeze::program             cubemap_shader;
    freeze::texture_cube        cubemap_tex;
    freeze::vao                 cubemap_vao;
    freeze::vbo                 trans_vbo;
};