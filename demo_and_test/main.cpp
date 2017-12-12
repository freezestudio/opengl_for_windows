//
//main.cpp

#include "particle_renderer.h"
//#include "ibl_renderer.h"

int main()
{
	auto gl = freeze::make<freeze::freegl>(3, 3);
	gl.hint(freeze::window_hint_type::samples, 4);

	auto window = freeze::make<freeze::window>(SCR_WIDTH, SCR_HEIGHT);
	window.make_context_current();
	window.set_frame_buffer_size_callback(RENDERER::framebuffer_size_callback);
	window.set_cursor_pos_callback(RENDERER::mouse_callback);
	window.set_scroll_callback(RENDERER::scroll_callback);
	window.set_mouse_button_callback(RENDERER::button_callback);
	window.load_loader();
	window.swap_interval(1);


	auto render = std::make_shared<RENDERER>();
	render->init();

	auto wnd = window.get_window_handle();
	window.set_user_data(render.get());
	void* data = window.get_user_data();

	int scrWidth, scrHeight;
	freeze::get_frame_buffer_size(window.get(), &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);

	freeze::depth::enable();

	while (!window.should_close())
	{
		render->process_event(window.get());

		freeze::clear_color_depth(0.1f, 0.1f, 0.1f);
		render->draw();

		window.swap_buffers();
		window.poll_events();
	}


	return 0;
}
