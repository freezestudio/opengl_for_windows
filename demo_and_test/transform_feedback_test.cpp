#include "transform_feedback_test.h"

static float quad_vertices[] = {
    // positions  // texture Coords
    -1.0f, -1.0f, 0.0f, 0.0f, //leftbottom
     1.0f, -1.0f, 1.0f, 0.0f, //rightbottom
    -1.0f,  1.0f, 0.0f, 1.0f, //lefttop
     1.0f,  1.0f, 1.0f, 1.0f, //righttop
};

//#define NormalOf(L,X) ((float)((float)(2.0f*X)/((float)L)-1.0f))
//#define ONE_OF_WIDTH  NormalOf(COUNTOF_WIDTH,1)
//#define ONE_OF_HEIGHT NormalOf(COUNTOF_HEIGHT,1)

static inline float normal_of(float length, float number)
{
    return 2.0f * number / length - 1.0f;
}

static inline float one_of_length(float length)
{
    return 1.0f/length;
}

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
        "resources/shaders/tfb_test.fs"s
    );
    image_shader.use();
    image_shader.set_int("image"s, 0);

    auto tex_datas = freeze::load_images_from_dir("resources/textures"s);
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
    vbo.copy_data(quad_vertices, sizeof(quad_vertices));
    freeze::vertex::set_enable(0, 4, 0, 0);
    vbo.unbind();
    vao.unbind();

    glm::vec3 offset[COUNTOF_MUL] = {  };
    auto index = 0;
    for (auto y = 0; y < COUNTOF_HEIGHT; ++y)
    {
        for (auto x = 0; x < COUNTOF_WIDTH; ++x)
        {
            offset[index++] = glm::vec3{
                normal_of(SCR_WIDTH,x*PER_WIDTH) + one_of_length(COUNTOF_WIDTH),
                normal_of(SCR_HEIGHT,y*PER_WIDTH) + one_of_length(COUNTOF_HEIGHT),
                0.0f
            };
        }
    }

    for (auto i = 0; i < COUNTOF_MUL; i++)
    {
        auto model = glm::translate(glm::mat4{1.0f},offset[i]);
        model = glm::scale(model, 
            glm::vec3{ 
                1.0f / (float)COUNTOF_WIDTH,
                1.0f / (float)COUNTOF_HEIGHT,
                1.0f 
            });
        mat_model[i] = model;
    }
}

void transform_feedback_test::draw()
{
    //1s  = 1000ms
    //1ms = 1000us
    //1us = 1000ns
    auto current_time = high_clock::now();
    auto span = current_time - last_time;
    delta_time = (float)span.count() * 0.000000001f;
    last_time = current_time;

    image_shader.use();
    vao.bind();

    for (auto i = 0; i < COUNTOF_MUL; ++i)
    {
        image_shader.set_mat4("model"s, mat_model[i]);
        vec_image_tex[i%vec_image_tex.size()].active();
        vec_image_tex[i%vec_image_tex.size()].bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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
