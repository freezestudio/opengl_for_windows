#include "cubemap_renderer.h"

static float cube[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
};

cubemap_renderer::cubemap_renderer()
    : scene_camera{ 0.0f,0.0f,3.0f }
    , width{0.0f}
    , height{0.0f}
    , last_x(SCR_WIDTH / 2.0f)
    , last_y(SCR_HEIGHT / 2.0f)
    , first_mouse(true)
    , delta_time(0.0f)
    , last_frame(0.0f)
{
}


cubemap_renderer::~cubemap_renderer()
{
}

void cubemap_renderer::do_init()
{
    ms_instance = shared_from_this();

    cubemap_shader.compile_file_and_link(
        "resources/shaders/skybox.vert"s,
        "resources/shaders/skybox.frag"s
    );
    cubemap_shader.use();
    cubemap_shader.set_int("skybox"s, 0);

    auto cube_data = freeze::load_cubmap_from_dir("resources/textures/skybox"s);
    cubemap_tex.bind();
    cubemap_tex.set_image(cube_data);
    cubemap_tex.set_wrap_r(GL_CLAMP_TO_EDGE);
    cubemap_tex.set_wrap_s(GL_CLAMP_TO_EDGE);
    cubemap_tex.set_wrap_t(GL_CLAMP_TO_EDGE);
    cubemap_tex.set_min_filter(GL_LINEAR);
    cubemap_tex.set_mag_filter(GL_LINEAR);
    cubemap_tex.unbind();

    cubemap_vao.bind();
    auto vbo = freeze::make_vbo();
    vbo.bind();
    vbo.copy_data(cube, sizeof(cube));
    freeze::vertex::set_enable(0, 3, 0, 0);
    vbo.unbind();
    trans_vbo.bind();
    trans_vbo.copy_data(nullptr, sizeof(glm::mat4) * 2, GL_DYNAMIC_DRAW);
    trans_vbo.unbind();
    cubemap_vao.unbind();

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    width = vp[2];
    height = vp[3];
}

void cubemap_renderer::draw()
{
    auto projection = glm::perspective(glm::radians(scene_camera.Zoom),
        width / height, 0.1f, 100.0f);
    auto view = scene_camera.get_view_matrix();
    trans_vbo.bind();
    trans_vbo.copy_subdata(&projection, sizeof(glm::mat4), 0);
    trans_vbo.copy_subdata(&view, sizeof(glm::mat4), sizeof(glm::mat4));

    cubemap_vao.bind();
    freeze::vertex::set_enable(1, 4, 32, 0);
    freeze::vertex::set_enable(2, 4, 32, 4);
    freeze::vertex::set_enable(3, 4, 32, 8);
    freeze::vertex::set_enable(4, 4, 32, 12);
    freeze::vertex::set_enable(5, 4, 32, 16);
    freeze::vertex::set_enable(6, 4, 32, 20);
    freeze::vertex::set_enable(7, 4, 32, 24);
    freeze::vertex::set_enable(8, 4, 32, 28);
    freeze::vertex::divisor(1, 1);
    freeze::vertex::divisor(2, 1);
    freeze::vertex::divisor(3, 1);
    freeze::vertex::divisor(4, 1);
    freeze::vertex::divisor(5, 1);
    freeze::vertex::divisor(6, 1);
    freeze::vertex::divisor(7, 1);
    freeze::vertex::divisor(8, 1);

    cubemap_shader.use();
    cubemap_tex.active();
    cubemap_tex.bind();
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 1);

    cubemap_vao.unbind();
}

void cubemap_renderer::process_event(window_pointer window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 0.25f * delta_time;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        scene_camera.process_keyboard(freeze::camera_movement::FORWARD, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        scene_camera.process_keyboard(freeze::camera_movement::BACKWARD, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        scene_camera.process_keyboard(freeze::camera_movement::LEFT, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        scene_camera.process_keyboard(freeze::camera_movement::RIGHT, cameraSpeed);
}

void cubemap_renderer::do_mouse_callback(freeze::window::pointer window, double xpos, double ypos)
{
    if (first_mouse)
    {
        last_x = xpos;
        last_y = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - last_x;
    float yoffset = last_y - ypos;

    last_x = xpos;
    last_y = ypos;

    scene_camera.process_mouse_movement(xoffset, yoffset);
}
