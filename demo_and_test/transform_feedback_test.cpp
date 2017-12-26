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
    , last_frame(0.0f)
{
}


transform_feedback_test::~transform_feedback_test()
{
}

void transform_feedback_test::do_init()
{
    ms_instance = shared_from_this();

    auto vert = "resources/shaders/instance_render.vert"s;
    auto frag = "resources/shaders/instance_render.frag"s;

    planet_shader.compile_file_and_link(vert, frag);
    planet_shader.use();
    planet_shader.set_int("diffuse"s, 6);

    auto planet_data = freeze::load_image_from_file(
        "resources/objects/planet/planet_Quom1200.png"s);
    planet_tex.bind();
    planet_tex.set_image(planet_data);
    planet_tex.mipmap();
    planet_tex.set_wrap_s(GL_REPEAT);
    planet_tex.set_wrap_t(GL_REPEAT);
    planet_tex.set_min_filter(GL_LINEAR_MIPMAP_NEAREST);
    planet_tex.set_mag_filter(GL_LINEAR);
    planet_tex.unbind();

    planet_model.load("resources/objects/planet/planet.obj"s);
    planet_model.setup(planet_shader);

    auto model = glm::mat4{ 1.0f };
    model = glm::scale(model, glm::vec3{ 0.2f });
    auto vbo = freeze::make_vbo();
    vbo.bind();
    vbo.copy_data(&model, sizeof(glm::mat4));

    for (auto& ilist : planet_model._vec_mesh_data)
    {
        ilist.mesh.vao.bind();
        freeze::vertex::set_enable(3, 4, 16, 0);
        freeze::vertex::set_enable(4, 4, 16, 4);
        freeze::vertex::set_enable(5, 4, 16, 8);
        freeze::vertex::set_enable(6, 4, 16, 12);
        freeze::vertex::divisor(3, 1);
        freeze::vertex::divisor(4, 1);
        freeze::vertex::divisor(5, 1);
        freeze::vertex::divisor(6, 1);
        ilist.mesh.vao.unbind();
    }

}

void transform_feedback_test::draw()
{
    auto current_frame = freeze::window::get_time();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    auto projection = glm::perspective(
        glm::radians(scene_camera.Zoom),
        (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f);
    auto view = scene_camera.get_view_matrix();
    auto model = glm::mat4{ 1.0f };

    planet_shader.use();
    planet_shader.set_mat4("projection"s, projection);
    planet_shader.set_mat4("view"s, view);
    planet_shader.set_mat4("model"s, model);
    planet_tex.active(6);
    planet_tex.bind();

    planet_model.draw_instanced(planet_shader,1);

    //for (auto& ilist : planet_model._vec_mesh_data)
    //{
    //    ilist.mesh.vao.bind();
    //    glDrawElementsInstanced(GL_TRIANGLES,
    //        ilist.data.Indices.size(), 
    //        GL_UNSIGNED_INT, 0,1);
    //    ilist.mesh.vao.unbind();
    //}
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
