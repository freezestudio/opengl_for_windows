//
//main.cpp

#include "ibl_renderer.h"

//auto load_gl()
//{
//	
//}

int main()
{
	auto gl = freeze::make<freeze::freegl>(4, 3);
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

	// then before rendering, 
	// configure the viewport to the original framebuffer's screen dimensions
	int scrWidth, scrHeight;
	freeze::get_frame_buffer_size(window.get(), &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);

	while (!window.should_close())
	{
		render->process_event(window.get());

		auto color = freeze::make_color(0.1f, 0.1f, 0.1f, 1.0f);
		color.clear();
		freeze::depth::clear();

		render->draw();

		window.swap_buffers();
		window.poll_events();
	}


	return 0;
}
