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

using Shader = freeze::program;
using Camera = freeze::camera;
using Texture = freeze::texture2d;
using Cubmap = freeze::texture_cube;
using Vao = freeze::vertex_array_buffer;
using Vbo = freeze::vertex_buffer;
using Fbo = freeze::frame_buffer;
using Rbo = freeze::render_buffer;
using Model = freeze::pbrmodel;

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
	void set_shader();
	void set_vertex();
	void set_model();
private:
	float last_x;
	float last_y;
	bool  first_mouse;
	float delta_time;
	float last_frame;
private:
	Camera scene_camera;

	Shader pbr_shader;
	Shader equirectangular_shader;
	Shader irradiance_shader;
	Shader prefilter_shader;
	Shader bkgnd_shader;

	Model ufo_model;
};

