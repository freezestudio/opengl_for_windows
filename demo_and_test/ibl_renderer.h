#pragma once
#include "renderer_base.h"

#ifdef RENDERER
#undef RENDERER
#endif

#define RENDERER ibl_renderer

#define SCR_WIDTH 1280
#define SCR_HEIGHT 700

class ibl_renderer
    : public renderer_base<ibl_renderer>
    , public std::enable_shared_from_this<ibl_renderer>
    , public renderer
{
public:
    ibl_renderer();
    virtual ~ibl_renderer();
public:
    void do_init();
    virtual void draw() override;
    void process_event(window_pointer window);
    void picking();
    int hit_test(int x, int y);
    void on_hit(int hit);
    void do_mouse_callback(freeze::window::pointer window, double xpos, double ypos);
    void do_scroll_callback(freeze::window::pointer window, double xoffset, double yoffset);
    void do_button_callback(freeze::window::pointer window, int button, int action, int mods);
private:
    void set_vertices();
    void set_shader();
    void set_texture();
    void set_model();

	void set_particle();
private:
    float last_x;
    float last_y;
    bool first_mouse;
    float delta_time;
    float last_frame;
private:
    freeze::camera scene_camera;

    freeze::program pbr_shader;
    freeze::program etc_shader;
    freeze::program irr_shader;
    freeze::program pft_shader;
    freeze::program brdf_shader;
    freeze::program bg_shader;
    freeze::program pick_shader;

	freeze::program particle_shader;
	freeze::texture2d particle_tex;
	freeze::vertex_array_buffer particle_vao;
	
    freeze::vertex_array_buffer cube_vao;
    freeze::vertex_array_buffer quad_vao;

    freeze::frame_buffer pbr_fbo;
    freeze::frame_buffer pick_fbo;

    freeze::texture2d hdr_tex;
    freeze::texture_cube cubemap_tex;
    freeze::texture_cube irr_tex;
    freeze::texture_cube pft_tex;
    freeze::texture2d brdf_tex;

    freeze::model_t<freeze::model_vertex, true> ufo_model;
};
