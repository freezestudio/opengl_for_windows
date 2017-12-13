#include "transform_feedback_test.h"



transform_feedback_test::transform_feedback_test()
{
}


transform_feedback_test::~transform_feedback_test()
{
}

void transform_feedback_test::do_init()
{
    ms_instance = shared_from_this();
    
    auto tfb = freeze::make_transform_feedback();

    auto tfb_shader = freeze::make_program();
    tfb_shader.compile_file("resources/shaders/tfb_test.vs"s,
        ""s, 
        "resources/shaders/tfb_test.gs"s);

    GLchar const* feedback_varyings[] = { "out_value" };
    tfb_shader.set_varyings(feedback_varyings, 1);

    tfb_shader.link();
    tfb_shader.use();

    auto vao = freeze::make_vertex_array_buffer();
    vao.bind();

    auto input_vbo = freeze::make_vertex_buffer();
    input_vbo.bind();
    GLfloat data[] = { 1,2,3,4,5 };
    input_vbo.copy_data(data, sizeof(data));
    freeze::vertex::set_enable(0, 1, 0, 0);

    auto output_vbo = freeze::make_vertex_buffer();
    output_vbo.bind();
    output_vbo.copy_data(nullptr, sizeof(data)*3, GL_STATIC_READ);

    tfb.bind();
    tfb.bind_base(output_vbo,0);

    glEnable(GL_RASTERIZER_DISCARD);
    tfb.begin(GL_TRIANGLES);
    glDrawArrays(GL_POINTS, 0, 5);
    tfb.end();
    glDisable(GL_RASTERIZER_DISCARD);
    glFlush();

    GLfloat feed_back[5*3];
    tfb.get_subdata(feed_back, 0, sizeof(feed_back));

    for (auto fb : feed_back)
    {
        std::cout << fb << " ";
    }
    std::cout << std::endl;
}

void transform_feedback_test::draw()
{
}

void transform_feedback_test::update()
{
}

void transform_feedback_test::draw_particle()
{
}

void transform_feedback_test::process_event(window_pointer window)
{
}
