# OpenGL for Windows

free_gl 3D基础类库，[glfw](http://www.glfw.org/)库的c++浅层封装,简化[OpenGL](https://www.opengl.org/)函数调用的有限封装。

## Download

直接复制<https://github.com/freezestudio/opengl_for_windows.git>，用git客户端工具拉取。

## Quick Start

1. 编译glfw动态链接库;
2. 程序需要在源码中包含`glad.c`;
3. 设置好文件引用路径后，程序中只需`#include <free_gl.h>`
3. 编译并运行示例。

```c++
#include <free_gl.h>

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int)
{
    //初始化
    auto free_gl = make<glfw::glfw>();
    free_gl.set_opengl_version(3, 3);
    free_gl.set_opengl_core_profile();
    
    //窗口
    auto free_window = make<glfw::window>(800, 600, "hello"s);
    free_window.make_context_current();
    free_window.set_key_callback([](glfw::window::pointer w, int key, int scancode, int action, int mods) {
    	glfw::set_window_should_close(w);
    });
    free_window.set_frame_buffer_size_callback([](glfw::window::pointer w, int width, int height) {
    	gl::view_port(0, 0, width, height);
    });
    
    //加载glad
    free_gl.load_loader();
    free_gl.swap_interval(1);
    
    //着色器程序
    auto shader_program = gl::make_shader_program("vertex.glsl"s, "fragment.glsl"s);
    
    //顶点数据
    float verties[] = {	/*左*/-0.5f,-0.5f,1.0f,/*右*/0.5f,-0.5f,1.0f,/*顶*/0.0f, 0.5f,1.0f,};
    auto single_vao = make<gl::single_vao>();
    auto single_vbo = make<gl::single_vbo>();
    
    //传递到GPU
    single_vao.bind(0);
    {
    	single_vbo.bind(0);
    	{
    		single_vbo.from(verties, sizeof(verties));
    		single_vbo.set(0, 3, gl::data_type::_float, true, 0, 0);
    	}
    	single_vbo.unbind();
    }
    single_vao.unbind();
    
    //渲染
    while (!free_window.should_close())
    {
    	gl::clear_color(0.2f, 0.3f, 0.4f);
    	gl::clear(gl::bit_field::color_buffer_bit);
    
    	shader_program.use();
    	single_vao.bind(0);
    
    	gl::draw_arrays(gl::draw_mode::triangles, 0, 3);
    
    	free_window.swap_buffers();
    	free_gl.poll_events();
    }
    return 0;
}
```

## documentation & Tutorial

中文教程参考：<https://learnopengl-cn.github.io>,英文教程参考：<https://learnopengl.com/>
API文档参考：<https://www.khronos.org/registry/OpenGL-Refpages/gl4/>