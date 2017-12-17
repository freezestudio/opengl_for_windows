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
    
    vao.bind();
    for (auto i = 0; i < 2; ++i)
    {
        tfo[i].bind();
        vbo[i].bind();
        vbo[i].copy_data(nullptr, sizeof(float) * 5, GL_DYNAMIC_DRAW);
        tfo[i].bind_base(vbo[i], 0);
    }

    draw_shader.compile_file_and_link(
        "resources/shaders/tfb_draw_test.vs"s,
        "resources/shaders/tfb_test.fs"s);

    tf_shader.compile_file(
        "resources/shaders/tfb_test.vs"s,
        ""s, 
        "resources/shaders/tfb_test.gs"s);

    GLchar const* feedback_varyings[] = { "out_value" };
    tf_shader.set_varyings(feedback_varyings, 1);

    tf_shader.link();
    tf_shader.use();

    //缓存用的是前一个
    float data[] = { 1,2,3,4,5 };
    vbo[prev].bind();
    vbo[prev].copy_subdata(data, sizeof(data), 0);
    freeze::vertex::set_enable(0, 1, 0, 0);
    
    tfo[next].bind();
    //tfo绑定的是后一个
    //这里，仅回馈而不用于渲染
    //数据将回馈到tfo[next]所绑定的缓冲区中
    //tfo[next]所绑定的缓冲区是vbo[next]
    glEnable(GL_RASTERIZER_DISCARD);
    tfo[next].begin(GL_POINTS);
    glDrawArrays(GL_POINTS, 0, 5);
    tfo[next].end();
    glDisable(GL_RASTERIZER_DISCARD);
    //glFlush();
    
    
    glPointSize(4.0f);
}

void transform_feedback_test::draw()
{
    draw_particle();
    update();
}

void transform_feedback_test::draw_particle()
{
    GLfloat feedback[5];
    tfo[next].get_subdata(feedback, 0, sizeof(feedback));
    for (auto fb : feedback)
    {
        std::cout << fb << " ";
    }
    std::cout << std::endl;

    draw_shader.use();
    
    tfo[next].bind();
    vbo[next].bind();
    freeze::vertex::set_enable(0, 1, 0, 0);
    tfo[next].draw(GL_POINTS);
}

void transform_feedback_test::update()
{
    std::swap(prev, next);

    tf_shader.use();

    tfo[next].bind();
    vbo[prev].bind();

    //tfo绑定的是后一个
    //这里，仅回馈而不用于渲染
    //数据将回馈到tfo[next]所绑定的缓冲区中
    //tfo[next]所绑定的缓冲区是vbo[next]
    glEnable(GL_RASTERIZER_DISCARD);
    tfo[prev].begin(GL_POINTS);
    tfo[prev].draw(GL_POINTS);
    tfo[prev].end();
    glDisable(GL_RASTERIZER_DISCARD);

}

void transform_feedback_test::process_event(window_pointer window)
{
}
