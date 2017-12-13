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

    //std::ifstream ifs{ "resources/shaders/tfb_test.vs"s };
    //if (!ifs.good())return;

    //std::stringstream ss;
    //ss << ifs.rdbuf();
    //auto source = ss.str();
    //ifs.close();
    //auto char_source = source.c_str();

    //auto shader = glCreateShader(GL_VERTEX_SHADER);
    //glShaderSource(shader, 1, &char_source, nullptr);
    //glCompileShader(shader);

    //auto program = glCreateProgram();
    //glAttachShader(program,shader);

    ////1. 必须在link之前
    //GLchar const* feedback_varyings[] = { "out_value" };
    //glTransformFeedbackVaryings(program, 1, feedback_varyings, GL_INTERLEAVED_ATTRIBS);

    //glLinkProgram(program);
    //GLint success = 0;
    //glGetProgramiv(program, GL_LINK_STATUS, &success);
    //if (!success)
    //{
    //    GLint length;
    //    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    //    GLchar info[512];
    //    glGetProgramInfoLog(program, 512, nullptr, info);
    //    std::cout << info << std::endl;
    //}

    //glUseProgram(program);

    ////2. 必须绑定VAO
    //GLuint vao;
    //glGenVertexArrays(1, &vao);
    //glBindVertexArray(vao);

    //GLfloat data[] = { 1.0f,2.0f,3.0f,4.0f,5.0f };

    ////3. 必须有2个通用缓冲区
    ////输入
    //GLuint vbo;
    //glGenBuffers(1, &vbo);
    //glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, 0);

    ////输出
    //GLuint tbo;
    //glGenBuffers(1, &tbo);
    //glBindBuffer(GL_ARRAY_BUFFER, tbo);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(data), nullptr, GL_STATIC_READ);
    //
    ////4. 必须禁用栅格化
    //glEnable(GL_RASTERIZER_DISCARD);

    ////5. 必须将输出连接到tfb
    //glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);

    ////6. 输出到tfb
    //glBeginTransformFeedback(GL_POINTS);
    //glDrawArrays(GL_POINTS, 0, 5);
    //glEndTransformFeedback();

    //glDisable(GL_RASTERIZER_DISCARD);

    //glFlush();

    //GLfloat feedback[5];
    //glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);
    //for (auto fb : feedback)
    //{
    //    std::cout << fb << " ";
    //}
    //std::cout << std::endl;

    //glDeleteBuffers(1, &tbo);
    //glDeleteBuffers(1, &vbo);
    //glDeleteVertexArrays(1, &vao);

    //glDeleteShader(shader);
    //glDeleteProgram(program);

    auto tfb = freeze::make_transform_feedback();

    auto tfb_shader = freeze::make_program();
    tfb_shader.compile_file("resources/shaders/tfb_test.vs"s,""s, "resources/shaders/tfb_test.gs"s);

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
