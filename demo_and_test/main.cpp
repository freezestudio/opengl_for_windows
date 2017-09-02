#include <free_gl.h>

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int)
{
    auto free_gl = make<glfw::glfw>();
    free_gl.set_opengl_version(3, 3);
    free_gl.set_opengl_core_profile();

    auto free_window = make<glfw::window>(800, 600, "hello"s);
    free_window.make_context_current();
    free_window.set_key_callback([](glfw::window::pointer w, int key, int scancode, int action, int mods) {
        glfw::set_window_should_close(w);
    });
    free_window.set_frame_buffer_size_callback([](glfw::window::pointer w, int width, int height) {
        gl::view_port(0, 0, width, height);
    });

    free_gl.load_loader();
    free_gl.swap_interval(1);

    auto shader_program = gl::make_shader_program("vertex.glsl"s, "fragment.glsl"s);

    float vertices[] = {
        0.5f, 0.5f, 0.0f,   // ���Ͻ�
        0.5f, -0.5f, 0.0f,  // ���½�
        -0.5f, -0.5f, 0.0f, // ���½�
        -0.5f, 0.5f, 0.0f   // ���Ͻ�
    };

    unsigned int indices[] = { // ע��������0��ʼ! 
        0, 1, 3, // ��һ��������
        1, 2, 3  // �ڶ���������
    };

    float texcoods[] = {
        0.0f,0.0f,
        1.0f,0.0f,
        0.5f,1.0f,
    };
    auto single_vao = make<gl::single_vao>();
    auto single_vbo = make<gl::single_vbo>();
    auto single_ebo = make<gl::single_ebo>();

    single_vao.bind(0);
    {
        single_vbo.bind(0);
        {
            single_vbo.from(vertices, sizeof(vertices));
            single_vbo.set(0, 3, gl::data_type::_float, false, 0, 0);

        }
        single_vbo.unbind();

        single_ebo.bind(0);
        single_ebo.from(indices, sizeof(indices));
    }
    single_vao.unbind();

    while (!free_window.should_close())
    {
        gl::clear_color(0.2f, 0.3f, 0.4f);
        gl::clear(gl::bit_field::color_buffer_bit);

        shader_program.use();
        single_vao.bind(0);

        gl::draw_arrays(gl::draw_mode::triangles, 0, 3);
        gl::draw_elements(gl::draw_mode::triangles, 6, gl::data_type::_unsigned_int, 0);

        free_window.swap_buffers();
        free_gl.poll_events();
    }
    return 0;
}