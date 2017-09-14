#pragma once

#include <free_gl.h>

// 设置顶点数据 (缓存) 并配置顶点属性
constexpr float vertices[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

constexpr auto vertices_size_bytes = sizeof(vertices);
constexpr auto vertices_size = vertices_size_bytes / (5 * sizeof(float));

//球心坐标为（x，y，z），球的半径为radius，M，N分别表示球体的横纵向被分成多少份  
void sphere(glm::vec3 core, GLfloat radius, GLfloat M, GLfloat N, glm::vec3* vertices)
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
				vertices[index++] = glm::vec3(core.x + x[k], core.y + y[k], core.z + z[k]);
			}
		}
	}
}

//全局变量

gl::camera camera{ { 0.0f,0.0f,3.0f } };
float delta_time{ 0.0f };
float latest_time{ 0.0f };
bool first_enter{ true };

float latest_pos_x{ 800.0f / 2.0f };
float latest_pos_y{ 600.0f / 2.0f };

//事件回调

void frame_buffer_size_callback(glfw::window::pointer w, int width, int height)
{
	gl::view_port(0, 0, width, height);
}

void key_callback(glfw::window::pointer win, int key, int scancode, int action, int mods)
{
	auto keycode = static_cast<glfw::key_code>(key);
	switch (keycode)
	{
	default:
		break;
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
	}
}

void cursor_enter_callback(glfw::window::pointer win, int entered)
{
	glfw::set_cursor_mode(win, entered ? glfw::window::cursor_mode::hidden :
		glfw::window::cursor_mode::normal);
	first_enter = entered;
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
	float yoffset = latest_pos_y - ypos; //y坐标由下到上

	latest_pos_x = xpos;
	latest_pos_y = ypos;

	camera.mouse_move(xoffset, yoffset);
}

void scroll_callback(glfw::window::pointer win, double xoffset, double yoffset)
{
	camera.scroll(yoffset);
}

int free()
{
	auto freegl = make<glfw::glfw>(3, 3);

	auto freewin = make<glfw::window>(800, 600);
	freewin.make_context_current();
	freewin.set_frame_buffer_size_callback(frame_buffer_size_callback);
	freewin.set_key_callback(key_callback);
	freewin.set_cursor_enter_callback(cursor_enter_callback);
	freewin.set_cursor_pos_callback(cursor_pos_callback);
	freewin.set_scroll_callback(scroll_callback);


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
			multi_vbo.set(0, 3, gl::data_type::_float, false, 8 * sizeof(float), 0);
			multi_vbo.set(1, 3, gl::data_type::_float, false, 8 * sizeof(float), 3 * sizeof(float));
			multi_vbo.set(2, 2, gl::data_type::_float, false, 8 * sizeof(float), 6 * sizeof(float));
		}
		multi_vbo.unbind();
	}
	multi_vao.unbind();

	auto diffuse_texture = gl::make_single_texture("container2.png"s);
	auto specular_texture = gl::make_single_texture("container2_specular.png"s);
	auto emission_texture = gl::make_single_texture("matrix.jpg"s);

	auto light_program = gl::make_shader_program("light.vs"s, "light.fs"s);
	auto cube_program = gl::make_shader_program("box.vs"s, "box.fs"s);

	cube_program.use();
	cube_program.set_vec3("view_position"s, camera.get_position());
	cube_program.set_int("material.diffuse"s, 0);
	cube_program.set_int("material.specular"s, 1);
	cube_program.set_int("material.emission"s, 2);
	cube_program.set_float("material.shininess"s, 32.0f);

	cube_program.set_vec3("light.position"s, glm::vec3{ 1.2f, 1.0f, 2.0f });
	cube_program.set_vec3("light.ambient"s, glm::vec3{ 0.2f});
	cube_program.set_vec3("light.diffuse"s, glm::vec3{ 0.5f});
	cube_program.set_vec3("light.specular"s, glm::vec3{ 1.0f});

	gl::enable_depth_test();

	while (!freewin.should_close())
	{
		float current_time = freegl.get_time();
		delta_time = current_time - latest_time;
		latest_time = current_time;

		gl::clear_color(0.2f, 0.3f, 0.4f);
		gl::clear(gl::bit_field::color_buffer_bit);
		gl::clear(gl::bit_field::depth_buffer_bit);

		//draw light
		{
			multi_vao.bind(multi_vao.first);
			light_program.use();

			auto light_proj = glm::perspective(glm::radians(camera.get_zoom()), 800.0f / 600.0f, 0.1f, 100.0f);
			auto light_view = camera.get_view();
			auto light_model = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ 1.2f, 1.0f, 2.0f });
			light_model = glm::scale(light_model, glm::vec3{ 0.2f });

			light_program.set_mat4("proj"s, light_proj);
			light_program.set_mat4("view"s, light_view);
			light_program.set_mat4("model"s, light_model);

			gl::draw_arrays(gl::draw_mode::triangle_fan, 0, sphere_size);
		}

		//draw cube
		{
			multi_vao.bind(multi_vao.second);

			diffuse_texture.active(0);
			diffuse_texture.bind(0);

			specular_texture.active(0);
			specular_texture.bind(0);

			emission_texture.active(0);
			emission_texture.bind(0);

			cube_program.use();

			auto cube_proj = glm::perspective(glm::radians(camera.get_zoom()), 800.0f / 600.0f, 0.1f, 100.0f);
			auto cube_view = camera.get_view();
			auto cube_model = glm::rotate(glm::mat4{ 1.0f }, glm::radians(45.0f), glm::vec3{ 1.0f,0.0f,1.0f });

			cube_program.set_mat4("proj"s, cube_proj);
			cube_program.set_mat4("view"s, cube_view);
			cube_program.set_mat4("model"s, cube_model);

			gl::draw_arrays(gl::draw_mode::triangles, 0, vertices_size);
		}

		freegl.poll_events();
		freewin.swap_buffers();
	}
	return 0;
}