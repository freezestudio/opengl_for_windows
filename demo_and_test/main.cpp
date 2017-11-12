//
//main.cpp

#include "point_shadow_renderer.h"
#include "dir_shadow_renderer.h"

int main()
{
    auto freegl = freeze::make<glfw::glfw>(4, 3);

    auto window = freeze::make<glfw::window>(SCR_WIDTH, SCR_HEIGHT);
    window.make_context_current();
    window.set_frame_buffer_size_callback(RENDERER::framebuffer_size_callback);
    window.set_cursor_pos_callback(RENDERER::mouse_callback);
    window.set_scroll_callback(RENDERER::scroll_callback);

    freegl.load_loader();
    freegl.swap_interval(1);
        
    auto render = std::make_shared<RENDERER>();
    render->init();

    while (!window.should_close())
    {
        auto current_frame = freegl.get_time();
        RENDERER::instance()->delta_time = current_frame - RENDERER::instance()->last_frame;
        RENDERER::instance()->last_frame = current_frame;
        render->process_event(window.get());

        auto color = freeze::make_color(0.1f, 0.1f, 0.1f, 1.0f);
        color.clear();
        freeze::depth::clear();

        render->draw();
        
        window.swap_buffers();
        freegl.poll_events();
    }

	return 0;
}
