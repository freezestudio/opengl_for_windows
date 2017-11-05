//
//
#include "common.h"
#include "renderer.h"

int main()
{
    auto freegl = freeze::make<glfw::glfw>(4, 3);

    auto window = freeze::make<glfw::window>(600, 400);
    window.make_context_current();
    window.set_frame_buffer_size_callback(renderer::framebuffer_size_callback);

    freegl.load_loader();
    freegl.swap_interval(1);

    auto render = std::make_shared<renderer>();
    render->init();

    while (!window.should_close())
    {
        auto color = freeze::make_color(0.1f, 0.1f, 0.1f, 1.0f);
        color.clear();
        render->draw();
        window.swap_buffers();
        freegl.poll_events();
    }

	return 0;
}