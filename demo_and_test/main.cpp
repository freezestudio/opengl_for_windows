#include <free_gl.h>

void simp_test_glm()
{
    auto vec = glm::vec4{ 1.f,0.f,0.f,1.f };

    //建议的顺序是：缩放-->旋转-->位移
    auto trans = glm::translate(glm::mat4{ 1.f }, glm::vec3{ 1.f,1.f,0.f });
    auto rotate = glm::rotate(glm::mat4{ 1.f }, glm::radians(45.0f), glm::vec3{ 0.f,0.f,1.f });
    auto scale = glm::scale(glm::mat4{ 1.f }, glm::vec3{ 0.5f,0.5f,0.5f });

    //从右向左
    vec = trans * rotate * scale * vec;
}

void key_callback(glfw::window::pointer win, int key, int scancode, int action, int mods)
{
    auto actioncode = static_cast<glfw::key_mouse_action>(action);
    if (actioncode != glfw::key_mouse_action::press)
    {
        return;
    }

    auto keycode = static_cast<glfw::key_code>(key);

    if (keycode == glfw::key_code::escape)
    {
        glfw::set_window_should_close(win);
    }
}

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int)
{
    //simp_test_glm();

    auto free_gl = make<glfw::glfw>();
    free_gl.set_opengl_version(3, 3);
    free_gl.set_opengl_core_profile();

    
    auto free_window = make<glfw::window>(800, 600, "hello"s);
    free_window.make_context_current();
    free_window.set_key_callback(key_callback);
    free_window.set_frame_buffer_size_callback([](glfw::window::pointer w, int width, int height) {
        gl::view_port(0, 0, width, height);
    });

    free_gl.load_loader();
    free_gl.swap_interval(1);

    auto shader_program = gl::make_shader_program("vertex.glsl"s, "fragment.glsl"s);

    // 设置顶点数据 (缓存) 并配置顶点属性
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    };

    glm::vec3 positions[]{
        {0.0f,  0.0f,  0.0f},
        {2.0f,  5.0f, -15.0f },
        {-1.5f, -2.2f, -2.5f },
        {-3.8f, -2.0f, -12.3f },
        {2.4f, -0.4f, -3.5f },
        {-1.7f,  3.0f, -7.5f },
        {1.3f, -2.0f, -2.5f },
        {1.5f,  2.0f, -2.5f },
        {1.5f,  0.2f, -1.5f },
        {-1.3f,  1.0f, -1.5f },
    };

    auto single_vao = make<gl::single_vao>();
    auto single_vbo = make<gl::single_vbo>();

    single_vao.bind(0);
    {
        single_vbo.bind(0);
        {
            single_vbo.from(vertices, sizeof(vertices));
            single_vbo.set(0, 3, gl::data_type::_float, false, 5 * sizeof(float), 0 * sizeof(float));
            single_vbo.set(1, 2, gl::data_type::_float, false, 5 * sizeof(float), (3 + 0) * sizeof(float));

        }
        single_vbo.unbind();
    }
    single_vao.unbind();

    auto single_texture2d_1 = gl::make_single_texture("container.jpg"s);
    auto single_texture2d_2 = gl::make_single_texture("awesomeface.png"s, gl::texture_format::rgb, gl::texture_format::rgba);

    shader_program.use();
    shader_program.set_uniform("ourTexture0"s, 0);
    shader_program.set_uniform("ourTexture1"s, 1);

    gl::enable_depth_test();

    while (!free_window.should_close())
    {
        gl::clear_color(0.2f, 0.3f, 0.4f);
        gl::clear(gl::bit_field::color_buffer_bit);
        gl::clear(gl::bit_field::depth_buffer_bit);

        single_texture2d_1.active(0);
        single_texture2d_1.bind(0);

        single_texture2d_2.active(0);
        single_texture2d_2.bind(0);

        shader_program.use();

        auto view = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ 0.0f,0.0f,-3.0f });
        auto proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        shader_program.set_uniform_matrix("proj"s, 1, glm::value_ptr(proj));
        shader_program.set_uniform_matrix("view"s, 1, glm::value_ptr(view));

            single_vao.bind(0);
        for (auto i = 0; i < 10; ++i)
        {
            auto model = glm::translate(glm::mat4{ 1.0f }, positions[i]);
            auto angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3{ 1.0f,0.3f,0.5f });
            shader_program.set_uniform_matrix("model"s, 1, glm::value_ptr(model));


            gl::draw_arrays(gl::draw_mode::triangles, 0, 36);
        }

        free_window.swap_buffers();
        free_gl.poll_events();
    }
    return 0;
}