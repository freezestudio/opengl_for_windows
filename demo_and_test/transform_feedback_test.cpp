#include "transform_feedback_test.h"


static float quad_vertices[] = {
    // positions        //               // texture Coords
    -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
};

transform_feedback_test::transform_feedback_test()
    : scene_camera{ 0.0f,0.0f,3.0f }
    , last_x(SCR_WIDTH / 2.0f)
    , last_y(SCR_HEIGHT / 2.0f)
    , first_mouse(true)
    , delta_time(0.0f)
{
}


transform_feedback_test::~transform_feedback_test()
{
}

void transform_feedback_test::do_init()
{
    ms_instance = shared_from_this();

    auto tex_datas = freeze::load_images_from_dir("resources/textures/skybox0"s);
    assert(tex_datas.size() > 0);
    for (auto i = 0; i < tex_datas.size(); ++i)
    {
        auto tex = freeze::make_texture2d();
        tex.bind();
        tex.set_image(tex_datas[i]);
        tex.mipmap();
        tex.set_wrap_s(GL_REPEAT);
        tex.set_wrap_t(GL_REPEAT);
        tex.set_min_filter(GL_LINEAR);
        tex.set_mag_filter(GL_LINEAR);
        tex.unbind();
        vec_image_tex.emplace_back(tex);
    }
}

void transform_feedback_test::draw()
{
    //1s  = 1000ms
    //1ms = 1000us
    //1us = 1000ns
    auto current_time = high_clock::now();
    auto span = last_time - current_time;
    delta_time = (float)span.count() * 0.000000001f;
    last_time = current_time;

}

void transform_feedback_test::process_event(window_pointer window)
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
