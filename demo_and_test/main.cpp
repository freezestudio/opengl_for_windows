#include <free_gl.h>

//glm���÷�
void simp_test_glm()
{
    auto vec = glm::vec4{ 1.f,0.f,0.f,1.f };

    //�����˳���ǣ�����-->��ת-->λ��
    auto trans = glm::translate(glm::mat4{ 1.f }, glm::vec3{ 1.f,1.f,0.f });
    auto rotate = glm::rotate(glm::mat4{ 1.f }, glm::radians(45.0f), glm::vec3{ 0.f,0.f,1.f });
    auto scale = glm::scale(glm::mat4{ 1.f }, glm::vec3{ 0.5f,0.5f,0.5f });

    //��������
    vec = trans * rotate * scale * vec;
}

//ȫ�ֱ���

gl::camera camera{ {0.0f,0.0f,3.0f} };
float delta_time{ 0.0f };
float latest_time{ 0.0f };
bool first_enter{ true };

float latest_pos_x{ 800.0f / 2.0f };
float latest_pos_y{ 600.0f / 2.0f };

//�¼��ص�

void key_callback(glfw::window::pointer win, int key, int scancode, int action, int mods)
{    
    auto keycode = static_cast<glfw::key_code>(key);
    switch (keycode)
    {
    case glfw::key_code::unknown:
        break;
    case glfw::key_code::space:
        break;
    case glfw::key_code::A:
        camera.key_press(gl::camera::movement::left, delta_time);
        break;
    case glfw::key_code::D:
        camera.key_press(gl::camera::movement::right, delta_time);
        break;
    case glfw::key_code::S:
        camera.key_press(gl::camera::movement::forwrad, delta_time);
        break;
    case glfw::key_code::W:
        camera.key_press(gl::camera::movement::backward, delta_time);
        break;
    case glfw::key_code::escape:
        glfw::set_window_should_close(win);
        break;
    default:
        break;
    }
}

void cursor_pos_callback(glfw::window::pointer win, double xpos, double ypos)
{
    if (first_enter)
    {
        latest_pos_x = xpos;
        latest_pos_y = ypos;
        first_enter = false;
    }

    float xoffset = xpos - latest_pos_x;
    float yoffset = latest_pos_y - ypos; //y�������µ���

    latest_pos_x = xpos;
    latest_pos_y = ypos;

    camera.mouse_move(xoffset, yoffset);
}

void scroll_callback(glfw::window::pointer win, double xoffset, double yoffset)
{
    camera.scroll(yoffset);
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
    free_window.set_cursor_enter_callback([](glfw::window::pointer win, int entered) {
        glfw::set_cursor_mode(win,entered ? glfw::window::cursor_mode::hidden :
            glfw::window::cursor_mode::normal);
        first_enter = entered;
    });
    free_window.set_cursor_pos_callback(cursor_pos_callback);
    free_window.set_scroll_callback(scroll_callback);
    free_window.set_frame_buffer_size_callback([](glfw::window::pointer w, int width, int height) {
        gl::view_port(0, 0, width, height);
    });

    free_gl.load_loader();
    free_gl.swap_interval(1);

    auto shader_program = gl::make_shader_program("vertex.glsl"s, "fragment.glsl"s);

    // ���ö������� (����) �����ö�������
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
        float current_time = free_gl.get_time();
        delta_time = current_time - latest_time;
        latest_time = current_time;

        gl::clear_color(0.2f, 0.3f, 0.4f);
        gl::clear(gl::bit_field::color_buffer_bit);
        gl::clear(gl::bit_field::depth_buffer_bit);

        single_texture2d_1.active(0);
        single_texture2d_1.bind(0);

        single_texture2d_2.active(0);
        single_texture2d_2.bind(0);

        shader_program.use();

        auto view = camera.get_view();
        auto proj = glm::perspective(glm::radians(camera.get_zoom()), 800.0f / 600.0f, 0.1f, 100.0f);
        shader_program.set_uniform_matrix("proj"s, proj);
        shader_program.set_uniform_matrix("view"s, view);

            single_vao.bind(0);
        for (auto i = 0; i < 10; ++i)
        {
            auto model = glm::translate(glm::mat4{ 1.0f }, positions[i]);
            auto angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3{ 1.0f,0.3f,0.5f });
            shader_program.set_uniform_matrix("model"s, model);

            gl::draw_arrays(gl::draw_mode::triangles, 0, 36);
        }

        free_window.swap_buffers();
        free_gl.poll_events();
    }
    return 0;
}