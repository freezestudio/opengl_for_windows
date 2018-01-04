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

    image_shader.compile_file_and_link(
        "resources/shaders/tfb_test.vs"s,
        "resources/shaders/tfb_test.fs"s,
        "resources/shaders/tfb_test.gs"s
    );
    image_shader.use();
    image_shader.set_int("image"s, 0);

    auto tex_datas = freeze::load_images_from_dir("C:\\Users\\olfan\\Pictures\\1200600"s);
    assert(tex_datas.size() > 0);

    stbi_set_flip_vertically_on_load(1);
    for (auto i = 0u; i < tex_datas.size(); ++i)
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
    stbi_set_flip_vertically_on_load(0);

    vao.bind();

    auto vbo = freeze::make_vbo();
    vbo.bind();
    float pos[] = { 0.0f,0.0f };
    vbo.copy_data(pos, sizeof(pos));
    freeze::vertex::set_enable(0, 2, 0, 0);
    vbo.unbind();

    //float offset[200] = { 0.0f };
    //auto index = 0;
    //for (auto y = -10; y < 10; y += 2)
    //{
    //    for (auto x = -10; x < 10; x += 2)
    //    {
    //        offset[index++] = (float)x * 0.2f + 0.2f;
    //        offset[index++] = (float)y * 0.2f + 0.2f;
    //    }
    //}

    //auto vbo2 = freeze::make_vbo();
    //vbo2.bind();
    //vbo2.copy_data(offset, sizeof(offset));
    //freeze::vertex::set_enable(1, 2, 0, 0);
    //freeze::vertex::divisor(1, 1);
    //vbo2.unbind();

    vao.unbind();

    glm::vec3 offset[100] = {  };
    auto index = 0;
    for (auto y = -10; y < 10; y += 2)
    {
        for (auto x = -10; x < 10; x += 2)
        {
            offset[index].x = (float)x * 0.2f + 0.2f;
            offset[index].y = (float)y * 0.2f + 0.2f;
            index++;
        }
    }

    for (auto i = 0; i < 100; i++)
    {
        mat_model[i] = glm::translate(glm::mat4{1.0f}, glm::vec3{ offset[i].x,offset[i].y,0.0f });
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

    image_shader.use();
    vao.bind();

    for (auto i = 0; i < 100; ++i)
    {
        image_shader.set_mat4("model"s, mat_model[i]);
        vec_image_tex[i%vec_image_tex.size()].active();
        vec_image_tex[i%vec_image_tex.size()].bind();
        glDrawArrays(GL_POINTS, 0, 1);
    }

    vao.unbind();
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
