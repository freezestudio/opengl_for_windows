#pragma once

#include "renderer_base.h"

#ifdef RENDERER
#undef RENDERER
#endif

#define RENDERER particle_renderer

#ifdef SCR_WIDTH
#undef SCR_WIDTH
#endif

#ifdef SCR_HEIGHT
#undef SCR_HEIGHT
#endif

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

class particle_renderer
	: public renderer_base<particle_renderer>
	, public std::enable_shared_from_this<particle_renderer>
	, public renderer
{
public:
	particle_renderer();
	~particle_renderer();
public:
	void do_init();
	virtual void draw() override;
	void update();
	void draw_particle();
	void process_event(window_pointer window);
private:
	float last_x;
	float last_y;
	bool first_mouse;
	float delta_time;
	float last_frame;
	float ftimes;
private:
	freeze::camera scene_camera;

	freeze::program billboard_shader;
	freeze::program partical_shader;

	freeze::texture2d billboard_tex;
	freeze::texture1d random_tex;

	freeze::transform_feedback  tfb[2];
	freeze::vertex_array_buffer vao[2];
	freeze::vertex_buffer       vbo[2];

	bool first_render = true;
	unsigned int current = 0;
	unsigned int next = 1;
};

