# OpenGL for Windows

[![Build status](https://travis-ci.org/glfw/glfw.svg?branch=master)](https://travis-ci.org/glfw/glfw)
[![Build status](https://ci.appveyor.com/api/projects/status/0kf0ct9831i5l6sp/branch/master?svg=true)](https://ci.appveyor.com/project/elmindreda/glfw)
[![Coverity Scan](https://scan.coverity.com/projects/4884/badge.svg)](https://scan.coverity.com/projects/glfw-glfw)

freeGL 3D������⣬[glfw](http://www.glfw.org/)���c++ǳ���װ,��[OpenGL](https://www.opengl.org/)�������õ����޷�װ��

## Introduction

freeGL���ǻ���OpenGL3.3�����ϰ汾��c++17��Windows��Androidԭ��������װ��

## Download

��git�ͻ��˹��߻�ȡ�� `git clone https://github.com/freezestudio/opengl_for_windows.git`

## Quick Start

1. ����glfw��̬���ӿ�;
2. ������Ҫ��Դ���а���`glad.c`;
3. ���ú��ļ�����·���󣬴�����ֻ��`#include <common.h>`
3. ���벢����ʾ����

```c++
#include <common.h>

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int)
{
    //��ʼ��
    auto freegl = freeze::make<glfw::glfw>(3, 3);
    
    //����
    auto window = freeze::make<glfw::window>(600, 400);
    window.make_context_current();
    window.set_key_callback([](glfw::window::pointer w, int key, int scancode, int action, int mods) {
        glfw::set_window_should_close(w);
    });
    window.set_frame_buffer_size_callback([](glfw::window::pointer w, int width, int height) {
        glViewport(0, 0, width, height);
    });
    
    //����glad
    freegl.load_loader();
    //���ѵ�˫�����л�
    freegl.swap_interval(1);
    
    //��ɫ������
    auto shader = freeze::make_program();
    shader.compile(
        "#version 330 core                     \n"
        "layout(location=0)in vec3 pos;        \n"
        "void main(){                          \n"
        "    gl_Position = vec4(pos,1.0f);     \n"
        "}                                     \n"s,
        "#version 330 core                     \n"
        "out vec4 color;                       \n"
        "void main(){                          \n"
        "    color = vec4(0.4f,0.3f,0.2f,1.0f);\n"
        "}                                     \n"s);
    
    //��������
    float vertices[] = {	
        /*��*/-0.5f,-0.5f,1.0f,
        /*��*/0.5f,-0.5f,1.0f,
        /*��*/0.0f, 0.5f,1.0f, };

    auto vao = freeze::make_vertex_array_buffer();
    vao.bind();
    auto vbo = freeze::make_vertex_buffer();
    vbo.bind();
    vbo.copy_data(vertices, sizeof(vertices));
    auto vbo_vertex = freeze::make_vertex();
    vbo_vertex.set(0, 3, GL_FLOAT, 3 * sizeof(float), 0);
    vbo.unbind();
    vao.unbind();
    
    //��Ⱦ
    while (!window.should_close())
    {
    	auto color = freeze::make_color(0.1f, 0.1f, 0.1f, 1.0f);
        color.clear();

        shader.use();
        vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        vao.unbind();

        window.swap_buffers();
        freegl.poll_events();
    }

    return 0;
}
```

��Ĺ����ǵ����ģ�ÿ����һ�����ܶ������ʾ��������������鿴ÿһ����ʾ������ǩ��ÿһ���ύ�е�ʾ���ļ�����Ӧ����ɫ�����롣

## documentation & Tutorial

* ���Ľ̳̲ο���<https://learnopengl-cn.github.io>
* Ӣ�Ľ̳̲ο���<https://learnopengl.com/>
* API�ĵ��ο���<https://www.khronos.org/registry/OpenGL-Refpages/gl4/>
* API�ĵ��ο���<https://www.khronos.org/registry/OpenGL-Refpages/es3/>
* WIKI  �ο���https://www.khronos.org/opengl/wiki/