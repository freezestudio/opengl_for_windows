#pragma once

#include "renderer_base.h"

#ifdef RENDERER
#undef RENDERER
#endif

#define RENDERER point_shadow_renderer

class point_shadow_renderer 
    : public renderer_base<point_shadow_renderer>
    , public std::enable_shared_from_this<point_shadow_renderer>
    , public renderer
{
public:
    point_shadow_renderer();
    virtual ~point_shadow_renderer();
public:
    virtual void draw() override;
    virtual void process_event(glfw::window::pointer window) override;
public:
    void do_init();
    void do_mouse_callback(double xpos, double ypos);
    void do_scroll_callback(double xoffset, double yoffset);
};

