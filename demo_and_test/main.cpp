#include <free_gl.h>

//#include "camera_test.h"

// 设置顶点数据 (缓存) 并配置顶点属性
constexpr float vertices[] = {
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

constexpr auto vertices_size_bytes = sizeof(vertices);
constexpr auto vertices_size = vertices_size_bytes / (5 * sizeof(float));

//球心坐标为（x，y，z），球的半径为radius，M，N分别表示球体的横纵向被分成多少份  
void sphere(glm::vec3 core, GLfloat radius, GLfloat M, GLfloat N,glm::vec3* vertices)
{
    auto PI = glm::pi<float>();

    float step_z = PI / M;
    float step_xy = 2 * PI / N;
    float x[4], y[4], z[4];

    float angle_z = 0.0;
    float angle_xy = 0.0;

    auto index = 0;
    
    for (int i = 0; i < M; i++)
    {
        angle_z = i * step_z;

        for (int j = 0; j < N; j++)
        {
            angle_xy = j * step_xy;

            x[0] = radius * sin(angle_z) * cos(angle_xy);
            y[0] = radius * sin(angle_z) * sin(angle_xy);
            z[0] = radius * cos(angle_z);

            x[1] = radius * sin(angle_z + step_z) * cos(angle_xy);
            y[1] = radius * sin(angle_z + step_z) * sin(angle_xy);
            z[1] = radius * cos(angle_z + step_z);

            x[2] = radius * sin(angle_z + step_z) * cos(angle_xy + step_xy);
            y[2] = radius * sin(angle_z + step_z) * sin(angle_xy + step_xy);
            z[2] = radius * cos(angle_z + step_z);

            x[3] = radius * sin(angle_z) * cos(angle_xy + step_xy);
            y[3] = radius * sin(angle_z) * sin(angle_xy + step_xy);
            z[3] = radius * cos(angle_z);

            for (int k = 0; k < 4; k++)
            {
                vertices[index++]=glm::vec3(core.x + x[k], core.y + y[k], core.z + z[k]);
            }
        }
    }

}

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int)
{
    auto freegl = make<glfw::glfw>(3,3);

    auto freewin = make<glfw::window>(800, 600);
    freewin.make_context_current();
    freewin.set_frame_buffer_size_callback(
        [](glfw::window::pointer w, int width, int height) {
        gl::view_port(0, 0, width, height);
    });

    freegl.load_loader();
    freegl.swap_interval(1);

    glm::vec3 sphere_vertices[60 * 60 * 4];
    sphere(glm::vec3(0), 1.0f, 60, 60, sphere_vertices);
    auto sphere_size_byte = sizeof(sphere_vertices);
    auto sphere_size = (sizeof(sphere_vertices) / sizeof(sphere_vertices[0]));

    auto multi_vao = make<gl::multi_vao<2>>();
    auto multi_vbo = make<gl::multi_vbo<2>>();

    //light
    multi_vao.bind(multi_vao.first);
    {
        multi_vbo.bind(multi_vbo.first);
        {
            multi_vbo.from(sphere_vertices, sphere_size_byte);
            multi_vbo.set(0, 3, gl::data_type::_float, false, 3 * sizeof(float), 0);
        }
        multi_vbo.unbind();
    }
    multi_vao.unbind();

    //cube
    multi_vao.bind(multi_vao.second);
    {
        multi_vbo.bind(multi_vbo.second);
        {
            multi_vbo.from(vertices, vertices_size_bytes);
            multi_vbo.set(0, 3, gl::data_type::_float, false, 5 * sizeof(float), 0);
        }
        multi_vbo.unbind();
    }
    multi_vao.unbind();

    auto norm_mat4 = glm::mat4{ 1.0f };
    auto proj = glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    auto view = glm::lookAt(glm::vec3{ 0.0f,0.0f,3.0f }, glm::vec3{ 0.0f }, glm::vec3{ 0.0f,1.0f,0.0f });
    auto model = glm::translate(norm_mat4, glm::vec3{ 14.0f,13.0f,-25.0f });
    
    auto light_program = gl::make_shader_program("light.vs"s, "light.fs"s);
    light_program.use();
    light_program.set_mat4("proj"s, proj);
    light_program.set_mat4("view"s, view);
    light_program.set_mat4("model"s, model);

    norm_mat4 = glm::mat4{ 1.0f };
    auto cube_program = gl::make_shader_program("box.vs"s,"box.fs"s);
    cube_program.use();
    cube_program.set_vec3("light_color"s, glm::vec3{1.0f});
    cube_program.set_vec3("cube_color"s, glm::vec3{ 1.0f, 0.5f, 0.31f });
    cube_program.set_mat4("proj"s, proj);
    cube_program.set_mat4("view"s, view);
    cube_program.set_mat4("model"s, glm::rotate(norm_mat4, glm::radians(45.0f), glm::vec3{1.0f,1.0f,0.0f}));
    
    while (!freewin.should_close())
    {
        gl::clear_color(0.2f, 0.3f, 0.4f);
        gl::clear(gl::bit_field::color_buffer_bit);

        //draw light
        {
            multi_vao.bind(multi_vao.first);
            light_program.use();

            gl::draw_arrays(gl::draw_mode::triangle_fan, 0, sphere_size);
        }
        
        //draw cube
        {
            multi_vao.bind(multi_vao.second);
            cube_program.use();

            gl::draw_arrays(gl::draw_mode::triangles, 0, vertices_size);
        }

        freegl.poll_events();
        freewin.swap_buffers();
    }
    return 0;
}