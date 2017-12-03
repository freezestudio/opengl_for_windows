#include "pbr_renderer.h"



pbr_renderer::pbr_renderer()
    : scene_camera{ 0.0f,0.0f,3.0f }
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

    auto current_frame = glfw::detail::get_time();

    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    freeze::depth::enable();

    pbr_shader.compile_file("resource/shaders/pbr.vert"s, "resource/shaders/pbr.frag");
    
    auto projection = glm::perspective(glm::radians(scene_camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    pbr_shader.use();
    pbr_shader.set_mat4("projection"s, projection);

    ufo_model.load("resource/objects/ufo/ufo.obj"s);
    ufo_model.setup(pbr_shader);
}

void pbr_renderer::draw()
{
    auto view = scene_camera.get_view_matrix();
    auto model = glm::mat4{ 1.0f };
    model = glm::scale(glm::mat4{ 1.0f }, glm::vec3{ 0.015f });
    model = glm::translate(model, glm::vec3{ 0.0f,-0.25f,0.0f });

    pbr_shader.use();
    pbr_shader.set_mat4("view"s, view);
    pbr_shader.set_mat4("model"s, model);
    pbr_shader.set_vec3("camPos"s, scene_camera.Position);
    
    for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
    {
        glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
        //newPos = lightPositions[i];
        pbr_shader.set_vec3("lightPositions[" + std::to_string(i) + "]", newPos);
        pbr_shader.set_vec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
    }

    ufo_model.draw(pbr_shader);
}

void pbr_renderer::process_event(window_pointer window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * delta_time;
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
        last_x = xpos;
        last_y = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - last_x;
    float yoffset = last_y - ypos; // reversed since y-coordinates go from bottom to top

    last_x = xpos;
    last_y = ypos;

    scene_camera.process_mouse_movement(xoffset, yoffset);
}

void pbr_renderer::do_scroll_callback(double xoffset, double yoffset)
{
    scene_camera.process_mouse_scroll(yoffset);
}
