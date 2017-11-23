#include "pbr_renderer.h"



pbr_renderer::pbr_renderer()
    : scene_camera{0.0f,0.0f,3.0f}
	, last_x(SCR_WIDTH / 2.0f)
	, last_y(SCR_HEIGHT / 2.0f)
	, first_mouse(true)
	, delta_time(0.0f)
	, last_frame(0.0f)
{
}


pbr_renderer::~pbr_renderer()
{
}

// lights
// ------
glm::vec3 lightPositions[] = {
    glm::vec3(-10.0f,  10.0f, 10.0f),
    glm::vec3(10.0f,  10.0f, 10.0f),
    glm::vec3(-10.0f, -10.0f, 10.0f),
    glm::vec3(10.0f, -10.0f, 10.0f),
};
glm::vec3 lightColors[] = {
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f)
};

void pbr_renderer::do_init()
{
    ms_instance = shared_from_this();
    freeze::depth::enable();

    pbr_shader.compile_file("resource/shaders/pbr.vert"s, "resource/shaders/pbr.frag");
    pbr_shader.use();
    pbr_shader.set_int("albedoMap"s, 0);
    pbr_shader.set_int("normalMap"s, 1);
    pbr_shader.set_int("metallicMap"s, 2);
    pbr_shader.set_int("roughnessMap"s, 3);

	auto pbr_base_dir = "resource/objects/bpr/feidie/"s;
	auto albedo_name = "FEIDIE_3DART_blinn3SG_BaseColor .PNG"s;
	auto normal_name = "FEIDIE_3DART_blinn3SG_Normal .PNG"s;
	auto metallic_name = "FEIDIE_3DART_blinn3SG_Metallic .png"s;
	auto roughness_name = "FEIDIE_3DART_blinn3SG_Roughness .png"s;

    albedo_tex.bind();
    {
        auto albedo_data = freeze::load_image_from_file(pbr_base_dir+albedo_name);
        albedo_tex.set_image(albedo_data);
        albedo_tex.mipmap();
        albedo_tex.set_wrap_s(GL_REPEAT);
        albedo_tex.set_wrap_t(GL_REPEAT);
        albedo_tex.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
        albedo_tex.set_mag_filter(GL_LINEAR);
    }    
    albedo_tex.unbind();

    normal_tex.bind();
    {
        auto normal_data = freeze::load_image_from_file(pbr_base_dir+normal_name);
        normal_tex.set_image(normal_data);
        normal_tex.mipmap();
        normal_tex.set_wrap_s(GL_REPEAT);
        normal_tex.set_wrap_t(GL_REPEAT);
        normal_tex.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
        normal_tex.set_mag_filter(GL_LINEAR);
    }    
    normal_tex.unbind();

    metallic_tex.bind();
    {
        auto metallic_data = freeze::load_image_from_file(pbr_base_dir+metallic_name);
        metallic_tex.set_image(metallic_data);
        metallic_tex.mipmap();
        metallic_tex.set_wrap_s(GL_REPEAT);
        metallic_tex.set_wrap_t(GL_REPEAT);
        metallic_tex.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
        metallic_tex.set_mag_filter(GL_LINEAR);
    }    
    metallic_tex.unbind();

    roughness_tex.bind();
    {
        auto roughness_data = freeze::load_image_from_file(pbr_base_dir+roughness_name);
        roughness_tex.set_image(roughness_data);
        roughness_tex.mipmap();
        roughness_tex.set_wrap_s(GL_REPEAT);
        roughness_tex.set_wrap_t(GL_REPEAT);
        roughness_tex.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
        roughness_tex.set_mag_filter(GL_LINEAR);
    }    
    roughness_tex.unbind();


    auto projection = glm::perspective(glm::radians(scene_camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    pbr_shader.set_mat4("projection"s, projection);

	ufo_model.load(pbr_base_dir + "feidie.obj"s);
	ufo_model.setup();
}

void pbr_renderer::draw()
{
	auto current_frame = glfw::detail::get_time();

	delta_time = current_frame - last_frame;
	last_frame = current_frame;

    pbr_shader.use();

    auto view = scene_camera.get_view_matrix();
    pbr_shader.set_mat4("view"s, view);
    pbr_shader.set_vec3("camPos"s, scene_camera.Position);
    albedo_tex.active(0);
    albedo_tex.bind();
    normal_tex.active(1);
    normal_tex.bind();
    metallic_tex.active(2);
    metallic_tex.bind();
    roughness_tex.active(3);
    roughness_tex.bind();

	for (auto i = 0u; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
	{
		auto newPos = lightPositions[i] + glm::vec3{ sin(glfwGetTime()*5.0f)*5.0f,0.0f,0.0f };
		pbr_shader.set_vec3("lightPositions["s + std::to_string(i) + "]"s, newPos);
		pbr_shader.set_vec3("lightColors["s + std::to_string(i) + "]"s, lightColors[i]);
	}

    glm::mat4 model{ 1.0f };
	model = glm::translate(model, glm::vec3{ 0.0f,-1.0f,0.0f });
	model = glm::scale(model, glm::vec3{ 0.10f });
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3{ 0.0f,1.0f,0.0f });
	pbr_shader.set_mat4("model"s, model);
	ufo_model.draw(pbr_shader);
}

void pbr_renderer::process_event(window_pointer window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5 * delta_time;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		scene_camera.process_keyboard(freeze::camera_movement::FORWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		scene_camera.process_keyboard(freeze::camera_movement::BACKWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		scene_camera.process_keyboard(freeze::camera_movement::LEFT, delta_time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		scene_camera.process_keyboard(freeze::camera_movement::RIGHT, delta_time);
}

void pbr_renderer::do_mouse_callback(double xpos, double ypos)
{
	if (first_mouse)
	{
		last_x = static_cast<float>(xpos);
		last_y = static_cast<float>(ypos);
		first_mouse = false;
	}

	auto xoffset = static_cast<float>(xpos) - last_x;
	auto yoffset = last_y - static_cast<float>(ypos);

	last_x = static_cast<float>(xpos);
	last_y = static_cast<float>(ypos);

	scene_camera.process_mouse_movement(xoffset, yoffset);
}

void pbr_renderer::do_scroll_callback(double xoffset, double yoffset)
{
	scene_camera.process_mouse_scroll(static_cast<float>(yoffset));
}
