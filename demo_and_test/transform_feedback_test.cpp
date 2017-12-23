#include "transform_feedback_test.h"



transform_feedback_test::transform_feedback_test()
    : scene_camera{0.0f,0.0f,3.0f}
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
    rock_shader.compile_file_and_link(vert,frag);
    rock_shader.set_int("diffuse"s, 6);
    
    planet_model.load("resources/objects/planet/planet.obj"s);
    planet_model.setup(planet_shader);

    rock_model.load("resources/objects/rock/rock.obj"s);
    rock_model.setup(rock_shader);

    auto planet_data = freeze::load_image_from_file("resources/objects/planet/planet_Quom1200.png"s);
    planet_tex.bind();
    planet_tex.set_image(planet_data);
    planet_tex.mipmap();
    planet_tex.set_wrap_s(GL_REPEAT);
    planet_tex.set_wrap_t(GL_REPEAT);
    planet_tex.set_min_filter(GL_LINEAR_MIPMAP_NEAREST);
    planet_tex.set_mag_filter(GL_LINEAR);
    planet_tex.unbind();

    auto rock_data = freeze::load_image_from_file("resources/objects/rock/Rock-Texture-Surface.jpg"s);
    rock_tex.bind();
    rock_tex.set_image(rock_data);
    rock_tex.mipmap();
    rock_tex.set_wrap_s(GL_REPEAT);
    rock_tex.set_wrap_t(GL_REPEAT);
    rock_tex.set_min_filter(GL_LINEAR_MIPMAP_NEAREST);
    rock_tex.set_mag_filter(GL_LINEAR);
    rock_tex.unbind();
}

void transform_feedback_test::draw()
{
    auto projection = glm::perspective(glm::radians(scene_camera.Zoom),
        (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f);
    auto view = scene_camera.get_view_matrix();


}

void transform_feedback_test::process_event(window_pointer window)
{
}
