//
// Created by freezestudio on 2017/10/11.
//

#ifndef FREEGL_COMMON_H
#define FREEGL_COMMON_H


//
// stl
//

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <type_traits>
#include <algorithm>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // translate, rotate, scale, perpective
#include <glm/gtc/constants.hpp>        // PI
#include <glm/gtc/type_ptr.hpp>

//
//for load 3d model
//
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
#if defined(_MSC_VER)

#pragma once

#include <iostream>
#include <utility>
#include <future>
#include <chrono>
#include <functional>
#include <algorithm>
#include <set>
#include <array>
#include <codecvt>
#include <filesystem>
#include <cassert>

#include <glad/glad.h>
#include <glfw/glfw3.h>

using namespace std::literals;
namespace fs = std::experimental::filesystem;

#ifdef _DEBUG
#define LOGI(...) ((void)printf(__VA_ARGS__))
#define LOGW(...) ((void)printf(__VA_ARGS__))
#define LOGE(...) ((void)printf(__VA_ARGS__))
#else
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#endif

namespace freeze
{
    template<typename T, typename = void>
    struct make_impl;

    template<bool condition>
    struct when;

    template<typename T, typename>
    struct make_impl : make_impl<T, when<true>>
    {
    };

    template<typename T, bool condition>
    struct make_impl<T, when<condition>>
    {
        template<typename... Args>
        static constexpr auto make_helper(int, Args&&... args)
        {
            return T{ std::forward<decltype(args)>(args)... };
        }

        template<typename... Args>
        static constexpr auto make_helper(long, Args&&... args)
        {
            static_assert((sizeof...(args), false));
        }

        template<typename... Args>
        static constexpr decltype(auto) apply(Args&&... args)
        {
            return make_helper(int{}, std::forward<decltype(args)>(args)...);
        }
    };

    //template<typename T>
    //constexpr auto make_t = [](auto&&... x) 
    //{
    //    return make_impl<T>::apply(std::forward<decltype(args)>(args)...);
    //};

    template<typename T>
    struct make_t
    {
        template<typename... Args>
        constexpr decltype(auto) operator()(Args&&... args) const
        {
            return make_impl<T>::apply(std::forward<decltype(args)>(args)...);
        }
    };

    template<typename T>
    constexpr make_t<T> make{};
}

/////////////////////////////////////////////////////////////////////////

//glfw::enum
namespace glfw {

    enum class window_hint_type
    {
        // ----------------------------------------------------------- 缺省值 ----------------------------- 支持的值 --------
        resizable = GLFW_RESIZABLE,	                                //GLFW_TRUE                    GLFW_TRUE or GLFW_FALSE
        visible = GLFW_VISIBLE,	                                    //GLFW_TRUE                    GLFW_TRUE or GLFW_FALSE
        decorated = GLFW_DECORATED,	                                //GLFW_TRUE                    GLFW_TRUE or GLFW_FALSE
        focused = GLFW_FOCUSED,	                                    //GLFW_TRUE                    GLFW_TRUE or GLFW_FALSE
        auto_iconify = GLFW_AUTO_ICONIFY,	                        //GLFW_TRUE                    GLFW_TRUE or GLFW_FALSE
        floating = GLFW_FLOATING,	                                //GLFW_FALSE                   GLFW_TRUE or GLFW_FALSE
        maximized = GLFW_MAXIMIZED,	                                //GLFW_FALSE                   GLFW_TRUE or GLFW_FALSE
        red_bits = GLFW_RED_BITS,                                   //8                            0 to INT_MAX or GLFW_DONT_CARE
        green_bits = GLFW_GREEN_BITS,	                            //8                            0 to INT_MAX or GLFW_DONT_CARE
        blue_bits = GLFW_BLUE_BITS,	                                //8                            0 to INT_MAX or GLFW_DONT_CARE
        alpha_bits = GLFW_ALPHA_BITS,	                            //8                            0 to INT_MAX or GLFW_DONT_CARE
        depth_bits = GLFW_DEPTH_BITS,	                            //24                           0 to INT_MAX or GLFW_DONT_CARE
        stencil_bits = GLFW_STENCIL_BITS,	                        //8                            0 to INT_MAX or GLFW_DONT_CARE
        accum_red_bits = GLFW_ACCUM_RED_BITS,	                    //0                            0 to INT_MAX or GLFW_DONT_CARE
        accum_green_bits = GLFW_ACCUM_GREEN_BITS,	                //0	                           0 to INT_MAX or GLFW_DONT_CARE
        accum_blue_bits = GLFW_ACCUM_BLUE_BITS,	                    //0                            0 to INT_MAX or GLFW_DONT_CARE
        accum_alpha_bits = GLFW_ACCUM_ALPHA_BITS,                   //0                            0 to INT_MAX or GLFW_DONT_CARE
        aux_buffers = GLFW_AUX_BUFFERS,	                            //0                            0 to INT_MAX or GLFW_DONT_CARE
        samples = GLFW_SAMPLES,	                                    //0                            0 to INT_MAX or GLFW_DONT_CARE
        refresh_rate = GLFW_REFRESH_RATE,	                        //GLFW_DONT_CARE               0 to INT_MAX or GLFW_DONT_CARE
        stereo = GLFW_STEREO,	                                    //GLFW_FALSE                   GLFW_TRUE or GLFW_FALSE
        srgb_capable = GLFW_SRGB_CAPABLE,	                        //GLFW_FALSE                   GLFW_TRUE or GLFW_FALSE
        doubleubffer = GLFW_DOUBLEBUFFER,	                        //GLFW_TRUE                    GLFW_TRUE or GLFW_FALSE
        client_api = GLFW_CLIENT_API,	                            //GLFW_OPENGL_API              GLFW_OPENGL_API, GLFW_OPENGL_ES_API or GLFW_NO_API
        context_creation_api = GLFW_CONTEXT_CREATION_API,	        //GLFW_NATIVE_CONTEXT_API      GLFW_NATIVE_CONTEXT_API or GLFW_EGL_CONTEXT_API
        context_version_major = GLFW_CONTEXT_VERSION_MAJOR,	        //1	                           Any valid major version number of the chosen client API
        context_version_minor = GLFW_CONTEXT_VERSION_MINOR,	        //0	                           Any valid minor version number of the chosen client API
        context_robustness = GLFW_CONTEXT_ROBUSTNESS,	            //GLFW_NO_ROBUSTNESS           GLFW_NO_ROBUSTNESS, GLFW_NO_RESET_NOTIFICATION or GLFW_LOSE_CONTEXT_ON_RESET
        context_release_behavior = GLFW_CONTEXT_RELEASE_BEHAVIOR,	//GLFW_ANY_RELEASE_BEHAVIOR    GLFW_ANY_RELEASE_BEHAVIOR, GLFW_RELEASE_BEHAVIOR_FLUSH or GLFW_RELEASE_BEHAVIOR_NONE
        opengl_forward_compat = GLFW_OPENGL_FORWARD_COMPAT,	        //GLFW_FALSE                   GLFW_TRUE or GLFW_FALSE
        opengl_debug_context = GLFW_OPENGL_DEBUG_CONTEXT,	        //GLFW_FALSE                   GLFW_TRUE or GLFW_FALSE
        opengl_profile = GLFW_OPENGL_PROFILE,	                    //GLFW_OPENGL_ANY_PROFILE      GLFW_OPENGL_ANY_PROFILE, GLFW_OPENGL_COMPAT_PROFILE or GLFW_OPENGL_CORE_PROFILE
    };

    enum class key_code
    {
        unknown = GLFW_KEY_UNKNOWN,//-1
        space = GLFW_KEY_SPACE,//32
        apostrophe = GLFW_KEY_APOSTROPHE,//39 /* ' */
        comma = GLFW_KEY_COMMA,//44 /* , */
        minus = GLFW_KEY_MINUS,//45 /* - */
        period = GLFW_KEY_PERIOD,//46 /* . */
        slash = GLFW_KEY_SLASH,//47 /* / */
        _0 = GLFW_KEY_0,//48
        _1 = GLFW_KEY_1,//49
        _2 = GLFW_KEY_2,//50
        _3 = GLFW_KEY_3,//51
        _4 = GLFW_KEY_4,//52
        _5 = GLFW_KEY_5,//53
        _6 = GLFW_KEY_6,//54
        _7 = GLFW_KEY_7,//55
        _8 = GLFW_KEY_8,//56
        _9 = GLFW_KEY_9,//57
        semicolon = GLFW_KEY_SEMICOLON,//59 /* ; */
        equal = GLFW_KEY_EQUAL,//61 /* = */
        A = GLFW_KEY_A,//65
        B = GLFW_KEY_B,//66
        C = GLFW_KEY_C,//67
        D = GLFW_KEY_D,//68
        E = GLFW_KEY_E,//69
        F = GLFW_KEY_F,//70
        G = GLFW_KEY_G,//71
        H = GLFW_KEY_H,//72
        I = GLFW_KEY_I,//73
        J = GLFW_KEY_J,//74
        K = GLFW_KEY_K,//75
        L = GLFW_KEY_L,//76
        M = GLFW_KEY_M,//77
        N = GLFW_KEY_N,//78
        O = GLFW_KEY_O,//79
        P = GLFW_KEY_P,//80
        Q = GLFW_KEY_Q,//81
        R = GLFW_KEY_R,//82
        S = GLFW_KEY_S,//83
        T = GLFW_KEY_T,//84
        U = GLFW_KEY_U,//85
        V = GLFW_KEY_V,//86
        W = GLFW_KEY_W,//87
        X = GLFW_KEY_X,//88
        Y = GLFW_KEY_Y,//89
        Z = GLFW_KEY_Z,//90
        left_bracket = GLFW_KEY_LEFT_BRACKET,//91 /* [ */
        backslash = GLFW_KEY_BACKSLASH,//92 /* \ */
        right_bracket = GLFW_KEY_RIGHT_BRACKET,//93 /* ] */
        grave_accent = GLFW_KEY_GRAVE_ACCENT,//96 /* ` */
        world_1 = GLFW_KEY_WORLD_1,//161 /* non-US #1 */
        world_2 = GLFW_KEY_WORLD_2,//162 /* non-US #2 */
        escape = GLFW_KEY_ESCAPE,//256
        enter = GLFW_KEY_ENTER,//257
        tab = GLFW_KEY_TAB,//258
        backspace = GLFW_KEY_BACKSPACE,//259
        insert = GLFW_KEY_INSERT,//260
        _delete = GLFW_KEY_DELETE,//261
        right = GLFW_KEY_RIGHT,//262
        left = GLFW_KEY_LEFT,//263
        down = GLFW_KEY_DOWN,//264
        up = GLFW_KEY_UP,//265
        page_up = GLFW_KEY_PAGE_UP,//266
        page_down = GLFW_KEY_PAGE_DOWN,//267
        home = GLFW_KEY_HOME,//268
        end = GLFW_KEY_END,//269
        caps_lock = GLFW_KEY_CAPS_LOCK,//280
        scroll_lock = GLFW_KEY_SCROLL_LOCK,//281
        num_lock = GLFW_KEY_NUM_LOCK,//282
        print_screen = GLFW_KEY_PRINT_SCREEN,//283
        pause = GLFW_KEY_PAUSE,//284
        F1 = GLFW_KEY_F1,//290
        F2 = GLFW_KEY_F2,//291
        F3 = GLFW_KEY_F3,//292
        F4 = GLFW_KEY_F4,//293
        F5 = GLFW_KEY_F5,//294
        F6 = GLFW_KEY_F6,//295
        F7 = GLFW_KEY_F7,//296
        F8 = GLFW_KEY_F8,//297
        F9 = GLFW_KEY_F9,//298
        F10 = GLFW_KEY_F10,//299
        F11 = GLFW_KEY_F11,//300
        F12 = GLFW_KEY_F12,//301
        F13 = GLFW_KEY_F13,//302
        F14 = GLFW_KEY_F14,//303
        F15 = GLFW_KEY_F15,//304
        F16 = GLFW_KEY_F16,//305
        F17 = GLFW_KEY_F17,//306
        F18 = GLFW_KEY_F18,//307
        F19 = GLFW_KEY_F19,//308
        F20 = GLFW_KEY_F20,//309
        F21 = GLFW_KEY_F21,//310
        F22 = GLFW_KEY_F22,//311
        F23 = GLFW_KEY_F23,//312
        F24 = GLFW_KEY_F24,//313
        F25 = GLFW_KEY_F25,//314
        kp_0 = GLFW_KEY_KP_0,//320
        kp_1 = GLFW_KEY_KP_1,//321
        kp_2 = GLFW_KEY_KP_2,//322
        kp_3 = GLFW_KEY_KP_3,//323
        kp_4 = GLFW_KEY_KP_4,//324
        kp_5 = GLFW_KEY_KP_5,//325
        kp_6 = GLFW_KEY_KP_6,//326
        kp_7 = GLFW_KEY_KP_7,//327
        kp_8 = GLFW_KEY_KP_8,//328
        kp_9 = GLFW_KEY_KP_9,//329
        kp_decimal = GLFW_KEY_KP_DECIMAL,//330
        kp_divide = GLFW_KEY_KP_DIVIDE,//331
        kp_multiply = GLFW_KEY_KP_MULTIPLY,//332
        kp_subtract = GLFW_KEY_KP_SUBTRACT,//333
        kp_add = GLFW_KEY_KP_ADD,//334
        kp_enter = GLFW_KEY_KP_ENTER,//335
        kp_equal = GLFW_KEY_KP_EQUAL,//336
        left_shift = GLFW_KEY_LEFT_SHIFT,//340
        left_control = GLFW_KEY_LEFT_CONTROL,//341
        left_alt = GLFW_KEY_LEFT_ALT,//342
        left_super = GLFW_KEY_LEFT_SUPER,//343
        right_shift = GLFW_KEY_RIGHT_SHIFT,//344
        right_control = GLFW_KEY_RIGHT_CONTROL,//345
        right_alt = GLFW_KEY_RIGHT_ALT,//346
        right_super = GLFW_KEY_RIGHT_SUPER,//347
        menu = GLFW_KEY_MENU,//348
        last = GLFW_KEY_LAST,//GLFW_KEY_MENU
    };

    enum class mouse_code
    {
        button_1 = GLFW_MOUSE_BUTTON_1,//0
        button_2 = GLFW_MOUSE_BUTTON_2,//1
        button_3 = GLFW_MOUSE_BUTTON_3,//2
        button_4 = GLFW_MOUSE_BUTTON_4,//3
        button_5 = GLFW_MOUSE_BUTTON_5,//4
        button_6 = GLFW_MOUSE_BUTTON_6,//5
        button_7 = GLFW_MOUSE_BUTTON_7,//6
        button_8 = GLFW_MOUSE_BUTTON_8,//7
        button_last = GLFW_MOUSE_BUTTON_LAST,//GLFW_MOUSE_BUTTON_8
        button_left = GLFW_MOUSE_BUTTON_LEFT,//GLFW_MOUSE_BUTTON_1
        button_middle = GLFW_MOUSE_BUTTON_MIDDLE,//  GLFW_MOUSE_BUTTON_3
        button_right = GLFW_MOUSE_BUTTON_RIGHT,// GLFW_MOUSE_BUTTON_2

    };

    enum class key_mouse_action
    {
        release, press, repeat
    };

    enum class input_mode
    {
        cursor = GLFW_CURSOR,
        strick_keys = GLFW_STICKY_KEYS,
        stricky_mouse_buttons = GLFW_STICKY_MOUSE_BUTTONS,
    };

	enum class monitor_type
	{
		none,primary,
	};
}

//glfw::detail
namespace glfw
{
    namespace detail
    {

        inline int make_version(int major, int minor)
        {
            return MAKELONG(major, minor);
        }

        inline int major_version(int version)
        {
            return LOWORD(version);
        }

        inline int minor_version(int version)
        {
            return HIWORD(version);
        }

        inline bool init()
        {
            return glfwInit() == GLFW_TRUE;
        }

#pragma region 在调用 init() 之前可用

        inline void terminate()
        {
            glfwTerminate();
        }

        inline void get_version(int& major, int& minor, int& rev)
        {
            glfwGetVersion(&major, &minor, &rev);
        }

        inline std::string get_version_string()
        {
            return glfwGetVersionString();
        }

#pragma endregion

        //GLFWwindow* create_window(int width, int height, std::string const& title, GLFWmonitor* _monitor,GLFWwindow* share)
        //{
        //	return glfwCreateWindow(width, height, title.c_str(), _monitor, share);
        //}

        inline void window_hint(window_hint_type hint, int value)
        {
            glfwWindowHint(static_cast<int>(hint), value);
        }

        inline bool load_gl_loader()
        {
            return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == GLFW_TRUE;
        }

        //inline bool gl_loader()
        //{
        //    return gladLoadGL();
        //}

        inline void poll_events()
        {
            glfwPollEvents();
        }

        inline void wait_events()
        {
            glfwWaitEvents();
        }

        inline void swap_interval(int interval = 0) //fast 0,normal 1
        {
            glfwSwapInterval(interval);
        }

        inline double get_time()
        {
            return glfwGetTime();
        }
    }
}

//glfw::class
namespace glfw
{

    class glfw
    {
    public:
        glfw()
            : inited_(detail::init())
        {

        }

        glfw(int major, int minor)
            : inited_(detail::init())
        {
            set_opengl_version(major, minor);
            set_opengl_core_profile();
        }

        ~glfw()
        {
            detail::terminate();
        }

        //initilize
    public:
        //void set_opengl_version(int version)
        //{
        //    set_opengl_version(major_version(version), minor_version(version));
        //}

        void set_opengl_version(int major, int minor)
        {
            detail::window_hint(window_hint_type::context_version_major, major);
            detail::window_hint(window_hint_type::context_version_minor, minor);
        }

        void set_opengl_core_profile()
        {
            set_opengl_profile(GLFW_OPENGL_CORE_PROFILE);
        }

        void set_opengl_profile(int profile)
        {
            detail::window_hint(window_hint_type::opengl_profile, profile);
        }

        //feature
    public:
        void swap_interval(int interval = 0)
        {
            detail::swap_interval(interval);
        }

        bool load_loader()
        {
            return detail::load_gl_loader();
        }

        void poll_events()
        {
            detail::poll_events();
        }

        float get_time() const
        {
            return (float)detail::get_time();
        }

        //callback
    public:
        //void handler(int error,const char* description)
        template<typename Handler>
        void set_error_callback(Handler&& handler)
        {
            glfwSetErrorCallback(handler);
        }
    private:
        bool inited_;
    };


    class monitor
    {
    public:
        using pointer = GLFWmonitor * ;
        using const_pointer = GLFWmonitor const*;

        monitor()
            : monitorp_(glfwGetPrimaryMonitor())
        {

        }

    public:
        pointer get()
        {
            return monitorp_;
        }

        const_pointer get() const
        {
            return monitorp_;
        }

    private:
        GLFWmonitor * monitorp_;
    };

    class cursor
    {

    };


    class window
    {
    public:
        using pointer = GLFWwindow * ;
        using const_pointer = GLFWwindow const*;

        enum class cursor_mode
        {
            normal = GLFW_CURSOR_NORMAL,
            hidden = GLFW_CURSOR_HIDDEN,
            disabled = GLFW_CURSOR_DISABLED,
        };

        window()
            : windowp_(nullptr)
        {

        }

        window(int width, int height,
            std::string const& title = "demo"s,
            monitor* _monitor = nullptr,
            window* _window = nullptr)
        {
            create(width, height, title, _monitor, _window);
        }

        ~window()
        {
            if (windowp_)
            {
                destroy();
                windowp_ = nullptr;
            }
        }
    public:

        bool create(int width, int height,
            std::string const& title = "demo"s,
            monitor* _monitor = nullptr,
            window* _window = nullptr)
        {
            windowp_ = glfwCreateWindow(width, height,
                title.c_str(),
                _monitor ? _monitor->get() : nullptr,
                _window ? _window->get() : nullptr);
            return windowp_ != nullptr;
        }

        void destroy()
        {
            glfwDestroyWindow(windowp_);
        }

        void make_context_current()
        {
            glfwMakeContextCurrent(windowp_);
        }

        void swap_buffers()
        {
            glfwSwapBuffers(windowp_);
        }

        bool should_close() const
        {
            return glfwWindowShouldClose(windowp_) == GLFW_TRUE;
        }

        void set_should_close(bool _close = true)
        {
            glfwSetWindowShouldClose(windowp_, true ? GL_TRUE : GL_FALSE);
        }

        void get_frame_buffer_size(int& width, int& height)
        {
            glfwGetFramebufferSize(windowp_, &width, &height);
        }

        template<typename Handler>
        void process_input_event(key_code key, Handler&& handler)
        {
            auto action = get_key(key);
            handler(action);
        }

        key_mouse_action get_key(key_code key) const
        {
            int action = glfwGetKey(windowp_, static_cast<int>(key));
            return static_cast<key_mouse_action>(action);
        }

        void set_cursor_mode(cursor_mode mode)
        {
            glfwSetInputMode(windowp_, static_cast<int>(input_mode::cursor), static_cast<int>(mode));
        }

        pointer get()
        {
            return windowp_;
        }

        const_pointer get() const
        {
            return windowp_;
        }

        //callback
    public:
        //void handler(GLFWwindow* window,int key,int scancode,int action,int mods)
        template<typename KeyHandler>
        void set_key_callback(KeyHandler&& handler)
        {
            glfwSetKeyCallback(windowp_, std::forward<KeyHandler>(handler));
        }

        //void handler(GLFWwindow* window,int width,int height)
        template<typename FramebufferSizeHandler>
        void set_frame_buffer_size_callback(FramebufferSizeHandler&& handler)
        {
            glfwSetFramebufferSizeCallback(windowp_, std::forward<FramebufferSizeHandler>(handler));
        }

        //void handler(GLFWwindow* window,int entered)
        template<typename CursorEnterHandler>
        void set_cursor_enter_callback(CursorEnterHandler&& handler)
        {
            glfwSetCursorEnterCallback(windowp_, std::forward<CursorEnterHandler>(handler));
        }

        //void handler(GLFWwindow* window,double xpos,double ypos)
        template<typename CursorPosHandler>
        void set_cursor_pos_callback(CursorPosHandler&& handler)
        {
            glfwSetCursorPosCallback(windowp_, std::forward<CursorPosHandler>(handler));
        }

        //void handler(GLFWwindow* window,double xoffset,double yoffset)
        template<typename ScrollHandler>
        void set_scroll_callback(ScrollHandler&& handler)
        {
            glfwSetScrollCallback(windowp_, std::forward<ScrollHandler>(handler));
        }

        //void handler(GLFWwindow* window,int button,int action,int mods)
        template<typename MouseButtonHandler>
        void set_mouse_button_callback(MouseButtonHandler&& handler)
        {
            glfwSetMouseButtonCallback(windowp_, std::forward<MouseButtonHandler>(handler));
        }

    private:
        GLFWwindow * windowp_;
    };

}

//glfw::function
namespace glfw
{

    inline void set_window_should_close(window::pointer win, bool _close = true)
    {
        glfwSetWindowShouldClose(win, _close ? GL_TRUE : GL_FALSE);
    }

    inline int get_key(window::pointer win, int key)
    {
        return glfwGetKey(win, key);
    }

    inline void set_cursor_mode(window::pointer win, window::cursor_mode mode)
    {
        glfwSetInputMode(win, static_cast<int>(input_mode::cursor), static_cast<int>(mode));
    }

    inline void set_cursor_normal(window::pointer win)
    {
        glfwSetInputMode(win, static_cast<int>(input_mode::cursor), static_cast<int>(window::cursor_mode::normal));
    }

    inline void set_cursor_disable(window::pointer win)
    {
        glfwSetInputMode(win, static_cast<int>(input_mode::cursor), static_cast<int>(window::cursor_mode::disabled));
    }

    inline void set_cursor_hidden(window::pointer win)
    {
        glfwSetInputMode(win, static_cast<int>(input_mode::cursor), static_cast<int>(window::cursor_mode::hidden));
    }
}

////////////////////////////////////////////////////////////////////////

#endif //defined(_MSC_VER)

#endif //defined(_WIN32) || defined(WIN32) || defined(__WIN32__)


#if defined(ANDROID) || defined(__ANDROID__)

/**
*  ndk
*/
#include <android/log.h>
#include <android/asset_manager_jni.h>

/**
* opengl es
*/
#include <egl/egl.h>
#include <gles2/gl2.h>
#include <gles2/gl2ext.h>
#include <gles3/gl32.h>
#include <gles3/gl3ext.h>

/**
* jni
*/
#include <jni.h>

//外部声明
////////////////////////////////////////////////////////////////////////////////////////////////////
extern AAssetManager* g_asset_managerp;                                            //内部资产管理器
                                                                                   ////////////////////////////////////////////////////////////////////////////////////////////////////

#define CREATE_FUNCTION(name) METHOD_NAME(name)
#define internal_args JNIEnv* pEnv,jobject obj

#define METHOD_NAME(name)                                METHOD_CLASS_NAME(CLASS,name)
#define METHOD_CLASS_NAME(cls,name)                      METHOD_PACKAGE_CLASS_NAME(PACKAGE,cls,name)
#define METHOD_PACKAGE_CLASS_NAME(pkg,cls,name)          METHOD_PACKAGE_CLASS_NAME_INTERNAL(pkg,cls,name)
#define METHOD_PACKAGE_CLASS_NAME_INTERNAL(pkg,cls,name) Java_##pkg##_##cls##_##name

#define DEBUG 1

#define LOG_TAG "Native"
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#if DEBUG
#define LOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__))
#else
#define LOGV(...)
#endif

#endif //define(ANDROID) || define(__ANDROID__)

#if defined(DEBUG) || defined(_DEBUG)
inline void __assert_error__(char const* func)
{
    auto err = glGetError();
    if (GL_NO_ERROR != err)
    {
        std::string errmsg;
        switch (err)
        {
        case GL_INVALID_ENUM:errmsg = "枚举参数不合法"s; break;
        case GL_INVALID_VALUE:errmsg = "值参数不合法"s; break;
        case GL_INVALID_OPERATION:errmsg = "一个指令的状态对指令的参数不合法"s; break;
        case GL_STACK_OVERFLOW:errmsg = "压栈操作造成栈上溢(Overflow)"s; break;
        case GL_STACK_UNDERFLOW:errmsg = "弹栈操作时栈在最低点（栈下溢(Underflow)）"s; break;
        case GL_OUT_OF_MEMORY:errmsg = "内存调用操作无法调用（足够的）内存"s; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:errmsg = "读取或写入一个不完整的帧缓冲"s; break;
        }
        LOGE("%s error : %d, msg: %s\n", func, err, errmsg.c_str());
    }
}

inline void __assert_error__(char const* func, char const* name)
{
    auto err = glGetError();
    if (GL_NO_ERROR != err)
    {
        std::string errmsg;
        switch (err)
        {
        case GL_INVALID_ENUM:errmsg = "枚举参数不合法"s; break;
        case GL_INVALID_VALUE:errmsg = "值参数不合法"s; break;
        case GL_INVALID_OPERATION:errmsg = "一个指令的状态对指令的参数不合法"s; break;
        case GL_STACK_OVERFLOW:errmsg = "压栈操作造成栈上溢(Overflow)"s; break;
        case GL_STACK_UNDERFLOW:errmsg = "弹栈操作时栈在最低点（栈下溢(Underflow)）"s; break;
        case GL_OUT_OF_MEMORY:errmsg = "内存调用操作无法调用（足够的）内存"s; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:errmsg = "读取或写入一个不完整的帧缓冲"s; break;
        }
        LOGE("%s error('%s') : %d, msg: %s\n", func, name, err, errmsg.c_str());
    }
}
#define assert_error()  __assert_error__(__FUNCTION__)
#define assert_name_error(name) __assert_error__(__FUNCTION__,name)
#endif

using namespace std::literals;
/**
* freeze lib
*/

#include "core.h"
#include "any_buffer.h"
#include "shader_program.h"
#include "texture.h"
#include "vertex.h"
#include "mesh.h"
#include "model.h"
#include "camera.h"
#include "color.h"
#include "depth.h"
#include "stencil.h"
#include "light.h"
#include "material.h"
#include "quickly.h"

//////////////////////////////////////////////////////////////////////////
#endif //FREEGL_COMMON_H