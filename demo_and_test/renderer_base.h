#pragma once

#include "common.h"

#ifdef RENDERER
#undef RENDERER
#endif

#define RENDERER

using window_pointer = glfw::window::pointer;

struct renderer_core
{
    virtual void draw() = 0;
};

struct renderer_event
{
    virtual void process_event(window_pointer window) = 0;
};

struct renderer
    : renderer_core
    , renderer_event
{
};

template<typename Singleton>
class renderer_base
{
public:
    renderer_base() = default;
    virtual ~renderer_base()
    {
        ms_instance.reset();
    }

    renderer_base& operator=(renderer_base const&) = delete;
    renderer_base& operator=(renderer_base&&) = delete;
    renderer_base(renderer_base const&) = delete;
    renderer_base(renderer_base&&) = delete;

    static std::shared_ptr<Singleton> instance()
    {
        assert(ms_instance);
        if (!ms_instance)
        {
            throw std::exception{ 
                "must invoke 'init()' before all this object's methods,"
                "and you use this class must as std::shared_ptr!" };
        }
        return ms_instance;
    }

protected:
    static std::shared_ptr<Singleton> ms_instance;
public:
    static void framebuffer_size_callback(
        glfw::window::pointer window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }
    static void mouse_callback(
        glfw::window::pointer window, double xpos, double ypos)
    {
        if (ms_instance)
        {
            ms_instance->do_mouse_callback(xpos, ypos);
        }
    }
    static void scroll_callback(
        glfw::window::pointer window, double xoffset, double yoffset)
    {
        if (ms_instance)
        {
            ms_instance->do_scroll_callback(xoffset, yoffset);
        }
    }

public:
    //必须极早调用，以避免ms_instance为空。
    void init()
    {
        static_cast<Singleton*>(this)->do_init();
    }

    //继承类需要实现以下方法
protected:
    void do_init()
    {
        //继承类实例化 ms_instance
        //ms_instance = shared_from_this();
        
    }

    void do_mouse_callback(double xpos, double ypos)
    {

    }

    void do_scroll_callback(double xoffset, double yoffset)
    {

    }
};

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#if defined(_MSC_VER)
template<typename T>
__declspec(selectany) std::shared_ptr<T> renderer_base<T>::ms_instance;
#endif
#endif
