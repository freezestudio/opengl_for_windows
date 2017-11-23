#pragma once

#include "renderer_base.h"

#ifdef RENDERER
#undef RENDERER
#endif

#define RENDERER pbr_renderer

#ifdef SCR_WIDTH
#undef SCR_WIDTH
#endif

#ifdef SCR_HEIGHT
#undef SCR_HEIGHT
#endif

#define SCR_WIDTH 1280
#define SCR_HEIGHT 800

class pbr_renderer
    : public renderer_base<pbr_renderer>
    , public std::enable_shared_from_this<pbr_renderer>
    , public renderer
{
public:
    pbr_renderer();
    virtual ~pbr_renderer();
public:
    void do_init();
    void draw();
    void process_event(window_pointer window);
	void do_mouse_callback(double xpos, double ypos);
	void do_scroll_callback(double xoffset, double yoffset);
private:
	float last_x;
	float last_y;
	bool  first_mouse;
	float delta_time;
	float last_frame;
private:
    freeze::camera scene_camera;
    freeze::program pbr_shader;
    freeze::texture2d albedo_tex, normal_tex, metallic_tex, roughness_tex;
	freeze::delay_model ufo_model;
};

