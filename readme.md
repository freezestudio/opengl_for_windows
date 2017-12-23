# OpenGL for Windows

[![Build status](https://travis-ci.org/glfw/glfw.svg?branch=master)](https://travis-ci.org/glfw/glfw)
[![Build status](https://ci.appveyor.com/api/projects/status/0kf0ct9831i5l6sp/branch/master?svg=true)](https://ci.appveyor.com/project/elmindreda/glfw)
[![Coverity Scan](https://scan.coverity.com/projects/4884/badge.svg)](https://scan.coverity.com/projects/glfw-glfw)

freeGL 3D基础类库，[glfw](http://www.glfw.org/)库的c++浅层封装,简化[OpenGL](https://www.opengl.org/)函数调用的有限封装。

## Introduction

freeGL，是基于OpenGL3.3及以上版本、c++17的Windows及Android原生简明封装。

## Download

用git客户端工具获取： `git clone https://github.com/freezestudio/opengl_for_windows.git`

## Quick Start

1. 编译glfw动态链接库;
2. 程序需要在源码中包含`glad.c`;
3. 设置好文件引用路径后，代码中只需`#include <common.h>`
3. 编译并运行示例。

```c++
#include <common.h>

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int)
{
    //初始化
    auto gl = freeze::make<freeze::freegl>(4, 3);
    
    //窗口
    auto window = freeze::make<freeze::window>(600, 400);
    window.make_context_current();
    window.set_key_callback([](freeze::window::pointer w, int key, int scancode, int action, int mods) {
        freeze::set_window_should_close(w);
    });
    window.set_frame_buffer_size_callback([](freeze::window::pointer w, int width, int height) {
        glViewport(0, 0, width, height);
    });
    
    //加载glad
    window.load_loader();
    //更佳的双缓冲切换
    window.swap_interval(1);
    
    //着色器程序
    auto shader = freeze::make_program();
    shader.compile(
        "#version 430 core                     \n"
        "layout(location=0)in vec3 pos;        \n"
        "void main(){                          \n"
        "    gl_Position = vec4(pos,1.0f);     \n"
        "}                                     \n"s,
        "#version 430 core                     \n"
        "out vec4 color;                       \n"
        "void main(){                          \n"
        "    color = vec4(0.4f,0.3f,0.2f,1.0f);\n"
        "}                                     \n"s);
    
    //顶点数据
    float vertices[] = {	
        /*左*/-0.5f,-0.5f,1.0f,
        /*右*/0.5f,-0.5f,1.0f,
        /*顶*/0.0f, 0.5f,1.0f, };

    auto vao = freeze::make_vertex_array_buffer();
    vao.bind();
    auto vbo = freeze::make_vertex_buffer();
    vbo.bind();
    vbo.copy_data(vertices, sizeof(vertices));
    freeze::vertex::set(0, 3, 0, 0);
    vbo.unbind();
    vao.unbind();
    
    //渲染
    while (!window.should_close())
    {
    	auto color = freeze::make_color(0.1f, 0.1f, 0.1f, 1.0f);
        color.clear();

        shader.use();
        vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        vao.unbind();

        window.swap_buffers();
        window.poll_events();
    }

    return 0;
}
```

库的功能是递增的，每递增一个功能都会更改示例的输出，如果想查看每一步的示例，请签出每一步提交中的示例文件及对应的着色器代码。

## documentation & Tutorial

* 中文教程参考：<https://learnopengl-cn.github.io>
* 英文教程参考：<https://learnopengl.com/>
* API文档参考：<https://www.khronos.org/registry/OpenGL-Refpages/gl4/>
* API文档参考：<https://www.khronos.org/registry/OpenGL-Refpages/es3/>
* WIKI  参考：<https://www.khronos.org/opengl/wiki/>