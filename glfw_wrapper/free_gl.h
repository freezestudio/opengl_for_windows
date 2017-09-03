#pragma once

#include <Windows.h>

#include <iostream>
#include <string>
#include <utility>
#include <memory>
#include <future>
#include <chrono>
#include <type_traits>
#include <functional>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <set>
#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>//translate,rotate,scale,perpective
#include <glm/gtc/constants.hpp>//PI
#include <glm/gtc/type_ptr.hpp>

using namespace std::literals;

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
//constexpr auto make_t = [](auto&&... x) {
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
}

//glfw::detail
namespace glfw
{
    namespace detail {

        bool init()
        {
            return glfwInit() == GLFW_TRUE;
        }

#pragma region 在调用 init() 之前可用

        void terminate()
        {
            glfwTerminate();
        }

        void get_version(int& major, int& minor, int& rev)
        {
            glfwGetVersion(&major, &minor, &rev);
        }

        std::string get_version_string()
        {
            return glfwGetVersionString();
        }

#pragma endregion

        //GLFWwindow* create_window(int width, int height, std::string const& title, GLFWmonitor* _monitor,GLFWwindow* share)
        //{
        //	return glfwCreateWindow(width, height, title.c_str(), _monitor, share);
        //}

        void window_hint(window_hint_type hint, int value)
        {
            glfwWindowHint(static_cast<int>(hint), value);
        }
        bool load_gl_loader()
        {
            return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == GLFW_TRUE;
        }

        void poll_events()
        {
            glfwPollEvents();
        }

        void wait_events()
        {
            glfwWaitEvents();
        }

        void swap_interval(int interval = 0) //fast 0,normal 1
        {
            glfwSwapInterval(interval);
        }

        double get_time()
        {
            return glfwGetTime();
        }

    }

}

//glfw::class
namespace glfw {

    class glfw
    {
    public:
        glfw()
            : inited_(detail::init())
        {
        }

        ~glfw()
        {
            detail::terminate();
        }

        //feature
    public:
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

    class monitor;
    class cursor;

    class window
    {
    public:
        using pointer = GLFWwindow*;
        using const_pointer = GLFWwindow const*;

        enum class cursor_mode
        {
            normal = GLFW_CURSOR_NORMAL,
            hidden = GLFW_CURSOR_HIDDEN,
            disabled = GLFW_CURSOR_DISABLED,
        };

        window() : windowp_(nullptr)
        {

        }

        window(int width, int height, std::string const& title)
            : windowp_(glfwCreateWindow(width, height, title.c_str(), nullptr,nullptr))
        {
            
        }

        window(int width, int height, std::string const& title, monitor* _monitor, window* _window)
        {

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

        bool create(int width, int height, std::string const& title)
        {
            windowp_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
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
            glfwSetKeyCallback(windowp_, std::forward<decltype(handler)>(handler));
        }

        //void handler(GLFWwindow* window,int width,int height)
        template<typename FramebufferSizeHandler>
        void set_frame_buffer_size_callback(FramebufferSizeHandler&& handler)
        {
            glfwSetFramebufferSizeCallback(windowp_, std::forward<decltype(handler)>(handler));
        }

        //void handler(GLFWwindow* window,int entered)
        template<typename CursorEnterHandler>
        void set_cursor_enter_callback(CursorEnterHandler&& handler)
        {
            glfwSetCursorEnterCallback(windowp_, std::forward<decltype(handler)>(handler));
        }

        //void handler(GLFWwindow* window,double xpos,double ypos)
        template<typename CursorPosHandler>
        void set_cursor_pos_callback(CursorPosHandler&& handler)
        {
            glfwSetCursorPosCallback(windowp_, std::forward<decltype(handler)>(handler));
        }

        //void handler(GLFWwindow* window,double xoffset,double yoffset)
        template<typename ScrollHandler>
        void set_scroll_callback(ScrollHandler&& handler)
        {
            glfwSetScrollCallback(windowp_, std::forward<decltype(handler)>(handler));
        }

    private:
        GLFWwindow* windowp_;
    };

    class monitor
    {
    public:
        using pointer = GLFWmonitor*;
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
        GLFWmonitor* monitorp_;
    };

    class cursor
    {

    };

}

//glfw::function
namespace glfw {
    void set_window_should_close(window::pointer win, bool _close = true)
    {
        glfwSetWindowShouldClose(win, _close ? GL_TRUE : GL_FALSE);
    }

    int get_key(window::pointer win, int key)
    {
        return glfwGetKey(win, key);
    }

    void set_cursor_mode(window::pointer win, window::cursor_mode mode)
    {
        glfwSetInputMode(win, static_cast<int>(input_mode::cursor), static_cast<int>(mode));
    }

    void set_cursor_normal(window::pointer win)
    {
        glfwSetInputMode(win, static_cast<int>(input_mode::cursor), static_cast<int>(window::cursor_mode::normal));
    }

    void set_cursor_disable(window::pointer win)
    {
        glfwSetInputMode(win, static_cast<int>(input_mode::cursor), static_cast<int>(window::cursor_mode::disabled));
    }

    void set_cursor_hidden(window::pointer win)
    {
        glfwSetInputMode(win, static_cast<int>(input_mode::cursor), static_cast<int>(window::cursor_mode::hidden));
    }
}

////////////////////////////////////////////////////////////////////////

//gl::enum
namespace gl {
    enum class shader_type
    {
        fragment_shader = GL_FRAGMENT_SHADER,
        vertex_shader = GL_VERTEX_SHADER,
        //compute_shader         = GL_COMPUTE_SHADER,
        //tess_control_shader    = GL_TESS_CONTROL_SHADER,
        //tess_evaluation_shader = GL_TESS_EVALUATION_SHADER,
        //geometry_shader        = GL_GEOMETRY_SHADER,

    };

    enum class shader_status
    {
        shader_type = GL_SHADER_TYPE,
        delete_status = GL_DELETE_STATUS,
        compile_status = GL_COMPILE_STATUS,
        info_log_length = GL_INFO_LOG_LENGTH,
        shader_source_length = GL_SHADER_SOURCE_LENGTH,
    };

    enum class program_status
    {
        delete_status = GL_DELETE_STATUS,
        link_status = GL_LINK_STATUS,
        validate_status = GL_VALIDATE_STATUS,
        info_log_length = GL_INFO_LOG_LENGTH,
        attached_shaders = GL_ATTACHED_SHADERS,
        //active_atomic_counter_buffers = GL_ACTIVE_ATOMIC_COUNTER_BUFFERS,
        active_attributes = GL_ACTIVE_ATTRIBUTES,
        active_attribute_max_length = GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
        active_uniforms = GL_ACTIVE_UNIFORMS,
        active_uniform_blocks = GL_ACTIVE_UNIFORM_BLOCKS,
        active_uniform_block_max_name_length = GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH,
        active_uniform_max_length = GL_ACTIVE_UNIFORM_MAX_LENGTH,
        //compute_work_group_size = GL_COMPUTE_WORK_GROUP_SIZE,
        //program_binary_length = GL_PROGRAM_BINARY_LENGTH,
        transform_feedback_buffer_mode = GL_TRANSFORM_FEEDBACK_BUFFER_MODE,
        transform_feedback_varyings = GL_TRANSFORM_FEEDBACK_VARYINGS,
        transform_feedback_varing_max_length = GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH,
        geometry_vertices_out = GL_GEOMETRY_VERTICES_OUT,
        geometry_input_type = GL_GEOMETRY_INPUT_TYPE,
        geometry_output_type = GL_GEOMETRY_OUTPUT_TYPE,
    };

    enum class buffer_type
    {
        //Buffer ---------------- Binding Target --------------------- Purpose -------------
        array_buffer = GL_ARRAY_BUFFER,                           //Vertex attributes
        //atomic_counter_buffer = GL_ATOMIC_COUNTER_BUFFER,       //Atomic counter storage
        copy_read_buffer = GL_COPY_READ_BUFFER,                   //Buffer copy source
        copy_write_buffer = GL_COPY_WRITE_BUFFER,                 //Buffer copy destination
        //dispatch_indirect_buffer = GL_DISPATCH_INDIRECT_BUFFER, //Indirect compute dispatch commands
        //draw_indirect_buffer = GL_DRAW_INDIRECT_BUFFER,         //Indirect command arguments
        element_array_buffer = GL_ELEMENT_ARRAY_BUFFER,           //Vertex array indices
        pixel_pack_buffer = GL_PIXEL_PACK_BUFFER,                 //Pixel read target
        pixel_unpack_buffer = GL_PIXEL_UNPACK_BUFFER,             //Texture data source
        //query_buffer = GL_QUERY_BUFFER,                         //Query result buffer
        //shader_storage_buffer = GL_SHADER_STORAGE_BUFFER,       //Read - write storage for shaders
        texture_buffer = GL_TEXTURE_BUFFER,                       //Texture data buffer
        transform_feedback_buffer = GL_TRANSFORM_FEEDBACK_BUFFER, //Transform feedback buffer
        uniform_buffer = GL_UNIFORM_BUFFER,                       //Uniform block storage
    };

    enum class buffer_usage_type
    {
        stream_draw = GL_STREAM_DRAW,   //数据每次绘制时都会改变
        stream_read = GL_STREAM_READ, 	//
        stream_copy = GL_STREAM_COPY, 	//
        static_draw = GL_STATIC_DRAW, 	//数据不会或几乎不会改变。
        static_read = GL_STATIC_READ, 	//
        static_copy = GL_STATIC_COPY,	//
        dynamic_draw = GL_DYNAMIC_DRAW,	//数据会被改变很多
        dynamic_read = GL_DYNAMIC_READ,	//
        dynamic_copy = GL_DYNAMIC_COPY,	//
    };

    enum class data_type
    {
        //glVertexAttribPointer and glVertexAttribIPointer
        _byte = GL_BYTE,
        _unsigned_byte = GL_UNSIGNED_BYTE,
        _short = GL_SHORT,
        _unsigned_short = GL_UNSIGNED_SHORT,
        _int = GL_INT,
        _unsigned_int = GL_UNSIGNED_INT,

        //glVertexAttribPointer
        _half_float = GL_HALF_FLOAT,
        _float = GL_FLOAT,
        _double = GL_DOUBLE, //glVertexAttribLPointer
        //_fixed = GL_FIXED, 
        //_int_2_10_10_10_rev =GL_INT_2_10_10_10_REV,
        _unsigned_int_2_10_10_10_rev = GL_UNSIGNED_INT_2_10_10_10_REV,
        unsigned_int_10f_11f_11f_rev = GL_UNSIGNED_INT_10F_11F_11F_REV,
    };

    enum class enable_status
    {
        blend = GL_BLEND,
        clip_distance_0 = GL_CLIP_DISTANCE0,
        clip_distance_1 = GL_CLIP_DISTANCE1,
        clip_distance_2 = GL_CLIP_DISTANCE2,
        clip_distance_3 = GL_CLIP_DISTANCE3,
        clip_distance_4 = GL_CLIP_DISTANCE4,
        clip_distance_5 = GL_CLIP_DISTANCE5,
        clip_distance_6 = GL_CLIP_DISTANCE6,
        clip_distance_7 = GL_CLIP_DISTANCE7,
        max_clip_distances = GL_MAX_CLIP_DISTANCES,
        color_logic_op = GL_COLOR_LOGIC_OP,
        cull_face = GL_CULL_FACE,
        depth_clamp = GL_DEPTH_CLAMP,
        debug_output = GL_DEBUG_OUTPUT,
        debug_output_synchronous = GL_DEBUG_OUTPUT_SYNCHRONOUS,
        depth_test = GL_DEPTH_TEST,
        dither = GL_DITHER,
        framebuffer_srgb = GL_FRAMEBUFFER_SRGB,
        line_smooth = GL_LINE_SMOOTH,
        multisample = GL_MULTISAMPLE,
        polygon_smooth = GL_POLYGON_SMOOTH,
        polygon_offset_fill = GL_POLYGON_OFFSET_FILL,
        polygon_offset_line = GL_POLYGON_OFFSET_LINE,
        polygon_offset_point = GL_POLYGON_OFFSET_POINT,
        program_point_size = GL_PROGRAM_POINT_SIZE,
        primitive_restart = GL_PRIMITIVE_RESTART,
        sample_alpha_to_coverage = GL_SAMPLE_ALPHA_TO_COVERAGE,
        sample_alpha_to_one = GL_SAMPLE_ALPHA_TO_ONE,
        sample_coverage = GL_SAMPLE_COVERAGE,
        sample_mask = GL_SAMPLE_MASK,
        scissor_test = GL_SCISSOR_TEST,
        stencil_test = GL_STENCIL_TEST,
        texture_cube_map_seamless = GL_TEXTURE_CUBE_MAP_SEAMLESS,
    };

    enum class bit_field
    {
        color_buffer_bit = GL_COLOR_BUFFER_BIT,
        depth_buffer_bit = GL_DEPTH_BUFFER_BIT,
        stencil_buffer_bit = GL_STENCIL_BUFFER_BIT,
    };

    enum class draw_mode
    {
        points = GL_POINTS,
        line_strip = GL_LINE_STRIP,
        line_loop = GL_LINE_LOOP,
        lines = GL_LINES,
        line_strip_adjacency = GL_LINE_STRIP_ADJACENCY,
        lines_adjacency = GL_LINES_ADJACENCY,
        triangle_strip = GL_TRIANGLE_STRIP,
        triangle_fan = GL_TRIANGLE_FAN,
        triangles = GL_TRIANGLES,
        triangle_strip_adjacency = GL_TRIANGLE_STRIP_ADJACENCY,
        triangles_adjacency = GL_TRIANGLES_ADJACENCY,
        //patches = GL_PATCHES,
    };

    enum class texture_type
    {
        _1d = GL_TEXTURE_1D,
        _1d_array = GL_TEXTURE_1D_ARRAY,
        _2d = GL_TEXTURE_2D,
        _2d_array = GL_TEXTURE_2D_ARRAY,
        _2d_multisample = GL_TEXTURE_2D_MULTISAMPLE,
        _2d_multisample_array = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
        _3d = GL_TEXTURE_3D,
        cube_map = GL_TEXTURE_CUBE_MAP,
        //cube_map_array = GL_TEXTURE_CUBE_MAP_ARRAY,
        rectangle = GL_TEXTURE_RECTANGLE,
    };

    enum class texture_feature
    {
        //depth_stencil_texture_mode = GL_DEPTH_STENCIL_TEXTURE_MODE,
        base_level = GL_TEXTURE_BASE_LEVEL,
        compare_func = GL_TEXTURE_COMPARE_FUNC,
        compare_mode = GL_TEXTURE_COMPARE_MODE,
        lod_bais = GL_TEXTURE_LOD_BIAS,
        min_filter = GL_TEXTURE_MIN_FILTER,
        mag_filter = GL_TEXTURE_MAG_FILTER,
        min_lod = GL_TEXTURE_MIN_LOD,
        max_lod = GL_TEXTURE_MAX_LOD,
        max_level = GL_TEXTURE_MAX_LEVEL,
        //swizzle_r = GL_TEXTURE_SWIZZLE_R, 
        //swizzle_g = GL_TEXTURE_SWIZZLE_G, 
        //swizzle_b = GL_TEXTURE_SWIZZLE_B, 
        //swizzle_a = GL_TEXTURE_SWIZZLE_A,
        wrap_s = GL_TEXTURE_WRAP_S,
        wrap_t = GL_TEXTURE_WRAP_T,
        wrap_r = GL_TEXTURE_WRAP_R,

        //For the vector commands(glTexParameter*v), pname can also be one of 
        border_color = GL_TEXTURE_BORDER_COLOR,
        //swizzle_rgba = GL_TEXTURE_SWIZZLE_RGBA,
    };

    enum class texture_format
    {
        red = GL_RED,
        rg = GL_RG,
        rgb = GL_RGB,
        bgr = GL_BGR,
        rgba = GL_RGBA,
        bgra = GL_BGRA,
        red_integer = GL_RED_INTEGER,
        rg_integer = GL_RG_INTEGER,
        rgb_integer = GL_RGB_INTEGER,
        bgr_integer = GL_BGR_INTEGER,
        rbga_integer = GL_RGBA_INTEGER,
        bgra_integer = GL_BGRA_INTEGER,
        stencil_index = GL_STENCIL_INDEX,
        depth_component = GL_DEPTH_COMPONENT,
        depth_stencil = GL_DEPTH_STENCIL,
    };

    enum class pixel_data_type
    {
        _unsigned_byte = GL_UNSIGNED_BYTE,
        _byte = GL_BYTE,
        _unsigned_short = GL_UNSIGNED_SHORT,
        _short = GL_SHORT,
        _unsigned_int = GL_UNSIGNED_INT,
        _int = GL_INT,
        _float = GL_FLOAT,
        _unsigned_byte_3_3_2 = GL_UNSIGNED_BYTE_3_3_2,
        _unsigned_byte_2_3_3_rev = GL_UNSIGNED_BYTE_2_3_3_REV,
        _unsigned_short_5_6_5 = GL_UNSIGNED_SHORT_5_6_5,
        _unsigned_short_5_6_5_rev = GL_UNSIGNED_SHORT_5_6_5_REV,
        _unsigned_short_4_4_4_4 = GL_UNSIGNED_SHORT_4_4_4_4,
        _unsigned_short_4_4_4_4_rev = GL_UNSIGNED_SHORT_4_4_4_4_REV,
        _unsigned_short_5_5_5_1 = GL_UNSIGNED_SHORT_5_5_5_1,
        _unsigned_short_1_5_5_5_rev = GL_UNSIGNED_SHORT_1_5_5_5_REV,
        _unsigned_int_8_8_8_8 = GL_UNSIGNED_INT_8_8_8_8,
        _unsigned_int_8_8_8_8_rev = GL_UNSIGNED_INT_8_8_8_8_REV,
        _unsigned_int_10_10_10_2 = GL_UNSIGNED_INT_10_10_10_2,
        _unsigned_int_2_10_10_10_rev = GL_UNSIGNED_INT_2_10_10_10_REV
    };
}

//gl::detail
namespace gl
{
    namespace detail {

        //misc

        void view_port(int x, int y, int width, int height)
        {
            glViewport(x, y, width, height);
        }

        int get_uniform_location(unsigned program_id, char const* name)
        {
            return glGetUniformLocation(program_id, name);
        }

        void uniform(int loc, int value)
        {
            glUniform1i(loc, value);
        }

        void uniform(int loc, float value)
        {
            glUniform1f(loc, value);
        }

        void uniform(int loc, unsigned count, int const* values)
        {
            glUniform1iv(loc, count, values);
        }

        void uniform(int loc, unsigned count, float const* values)
        {
            glUniform1fv(loc, count, values);
        }

        void uniform_matrix(int loc, unsigned count, bool transpose, float const* value)
        {
            glUniformMatrix4fv(loc, count, transpose ? GL_TRUE : GL_FALSE, value);
        }

        void enable_vertex_attrib_array(int loc)
        {
            glEnableVertexAttribArray(loc);
        }

        void set_attribute(int loc_in_vs, unsigned loc_t_size, data_type dt, bool normalized, unsigned stride, void const* next)
        {
            glVertexAttribPointer(loc_in_vs, loc_t_size, static_cast<unsigned>(dt), normalized ? GL_TRUE : GL_FALSE, stride, next);
        }

        void clear_color(float red, float green, float blue, float alpha)
        {
            glClearColor(red, green, blue, alpha);
        }

        void clear(bit_field mask)
        {
            glClear(static_cast<unsigned>(mask));
        }

        //buffers

        void create_buffers(unsigned* buffers, int n)
        {
            glGenBuffers(n, buffers);
        }

        void delete_buffers(unsigned const* buffers, int n)
        {
            glDeleteBuffers(n, buffers);
        }

        void bind_buffer(buffer_type buffer_t, unsigned buffer)
        {
            glBindBuffer(static_cast<int>(buffer_t), buffer);
        }

        void unbind_buffer(buffer_type buffer_t)
        {
            glBindBuffer(static_cast<int>(buffer_t), 0);
        }

        void buffer_data(buffer_type buffer_t, int size, void const* data, buffer_usage_type usage)
        {
            glBufferData(static_cast<int>(buffer_t), size, data, static_cast<int>(usage));
        }

        void create_vertex_arrays(unsigned* buffers, int n)
        {
            glGenVertexArrays(n, buffers);
        }

        void delete_vertex_arrays(unsigned const* buffers, int n)
        {
            glDeleteVertexArrays(n, buffers);
        }

        void bind_vertex_array(unsigned buffer)
        {
            glBindVertexArray(buffer);
        }

        void unbind_vertex_array()
        {
            glBindVertexArray(0);
        }

        //texture

        void create_textures(unsigned* textures, int n)
        {
            glGenTextures(n, textures);
        }

        void bind_texture(texture_type type, unsigned texture_id)
        {
            glBindTexture(static_cast<unsigned>(type), texture_id);
        }

        //level -- Specifies the level-of-detail number. 
        //         Level 0 is the base image level.
        //         Level n is the nth mipmap reduction image. 
        //         If target is GL_TEXTURE_RECTANGLE or GL_PROXY_TEXTURE_RECTANGLE,level must be 0.
        void texture_image_2d(int level,
            texture_format internel_format,
            int width, int height,
            texture_format format,
            pixel_data_type type,
            void const* pixels)
        {
            glTexImage2D(static_cast<unsigned>(texture_type::_2d),
                level,
                static_cast<int>(internel_format),
                width, height, 0,
                static_cast<unsigned>(format),
                static_cast<unsigned>(type), pixels);
        }

        void texture_create_mipmap()
        {
            glGenerateMipmap(static_cast<unsigned>(texture_type::_2d));
        }

        void texture_parameter(texture_type type, texture_feature feature, int value)
        {
            glTexParameteri(static_cast<unsigned>(type), static_cast<unsigned>(feature), value);
        }

        void texture_parameter(texture_type type, texture_feature feature, float value)
        {
            glTexParameterf(static_cast<unsigned>(type), static_cast<unsigned>(feature), value);
        }

        void texture_parameter_vector(texture_type type, texture_feature feature, const float* values)
        {
            glTexParameterfv(static_cast<unsigned>(type), static_cast<unsigned>(feature), values);
        }

        void texture_active(int id)
        {
            if (id >= GL_TEXTURE0 && id < GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS)
            {
                glActiveTexture(id);
            }
        }

        //shader

        unsigned create_shader(shader_type shader_t)
        {
            auto res = glCreateShader(static_cast<int>(shader_t));
            if (res == 0/*error*/ || res == GL_INVALID_ENUM/*parameter error*/)
            {

            }

            return res;
        }

        void shader_source(unsigned shader_id, int count, char const* const* string, int const* length)
        {
            glShaderSource(shader_id, count, string, length);
        }

        void compile_shader(unsigned shader_id)
        {
            glCompileShader(shader_id);
        }

        bool get_shader_status(unsigned shader_id, shader_status status)
        {
            int success;
            glGetShaderiv(shader_id, static_cast<unsigned>(status), &success);
            return success == GL_TRUE;
        }

        std::string get_shader_info_log(unsigned shader_id)
        {
            std::string ret_string;

            int length;
            glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
            if (length > 0)
            {
                char* err = new char[length + 1];
                glGetShaderInfoLog(shader_id, length + 1, nullptr, err);
                ret_string = err;
                delete[] err;
            }
            return ret_string;

        }

        void delete_shader(unsigned shader_id)
        {
            glDeleteShader(shader_id);
        }

        bool is_shader(unsigned shader_id)
        {
            return glIsShader(shader_id) == GL_TRUE;
        }

        //program

        unsigned create_program()
        {
            return glCreateProgram();
        }

        void attach_shader(unsigned program_id, unsigned shader_id)
        {
            glAttachShader(program_id, shader_id);
        }

        void detach_shadeer(unsigned program_id, unsigned shader_id)
        {
            glDetachShader(program_id, shader_id);
        }

        void link_program(unsigned program_id)
        {
            glLinkProgram(program_id);
        }

        void use_program(unsigned program_id)
        {
            glUseProgram(program_id);
        }

        bool get_program_status(unsigned program_id, program_status status)
        {
            int success;
            glGetProgramiv(program_id, static_cast<unsigned>(status), &success);
            return success == GL_TRUE;
        }

        std::string get_program_info_log(unsigned program_id)
        {
            std::string ret_string;

            int length;
            glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);
            if (length > 0)
            {
                char* err = new char[length + 1];
                glGetProgramInfoLog(program_id, length + 1, nullptr, err);
                ret_string = err;
                delete[] err;
            }

            return ret_string;
        }

        void delete_program(unsigned program_id)
        {
            glDeleteProgram(program_id);
        }

        bool is_program(unsigned program_id)
        {
            return glIsProgram(program_id) == GL_TRUE;
        }

        //misc
        bool is_enabled(enable_status status, int index = -1)
        {
            return index >= 0 ?
                glIsEnabledi(static_cast<unsigned>(status), index) == GL_TRUE :
                glIsEnabled(static_cast<unsigned>(status)) == GL_TRUE;
        }

        bool enabled(enable_status status, int index = -1, bool enable = true)
        {
            index >= 0 ?
                enable ? glEnablei(static_cast<unsigned>(status), index) : glDisablei(static_cast<unsigned>(status), index) :
                enable ? glEnable(static_cast<unsigned>(status)) : glDisable(static_cast<unsigned>(status));


            return is_enabled(status, index);

        }

        //render
        void draw_arrays(draw_mode mode, int first, int count)
        {
            glDrawArrays(static_cast<unsigned>(mode), first, count);
        }

        void draw_elements(draw_mode mode, int count, data_type dt, void const* indices)
        {
            glDrawElements(static_cast<unsigned>(mode), count, static_cast<unsigned>(dt), indices);
        }
    }

}

//gl::detail::function
namespace gl {

    namespace detail {
        std::string load_shader(std::string const& glslFile)
        {
            std::string str;
            std::ostringstream oss;

            std::ifstream ifs{ glslFile };
            oss << ifs.rdbuf();
            str = oss.str();

            ifs.clear();
            ifs.close();

            return str;
        }
    }
}

//gl::class
namespace gl {

    class shader
    {
    public:
        shader(shader_type type)
            : shader(type, "")
        {

        }

        shader(shader_type type, std::string const& source_file)
            : shader_id_(detail::create_shader(type))
        {
            if (shader_id_ > 0 && !source_file.empty())
            {
                load_source(source_file);
            }
        }

    public:
        void load_source(std::string const& source_file)
        {
            std::string source = detail::load_shader(source_file);
            const char* source_string = source.c_str();
            detail::shader_source(shader_id_, 1, &source_string, nullptr);
        }

        void load_source_code(std::string const& source_code)
        {
            const char* source_string = source_code.c_str();
            detail::shader_source(shader_id_, 1, &source_string, nullptr);
        }

        bool compile()
        {
            detail::compile_shader(shader_id_);

            bool success = detail::get_shader_status(shader_id_, shader_status::compile_status);
            if (!success)
            {
                error_message_ = detail::get_shader_info_log(shader_id_);
            }

            return success;
        }

        bool enabled() const
        {
            return detail::is_shader(shader_id_);
        }

        unsigned get() const
        {
            return shader_id_;
        }

        void reset()
        {
            detail::delete_shader(shader_id_);
        }

        std::string get_error_message() const
        {
            return error_message_;
        }

    private:
        unsigned shader_id_;
        std::string error_message_;
    };

    class shader_program
    {
    public:
        shader_program()
            : program_id_(detail::create_program())
        {

        }

        ~shader_program()
        {
            shader_set_.clear();
        }

    public:
        void attach(shader const& _shader)
        {
            unsigned shader_id = _shader.get();
            detail::attach_shader(program_id_, shader_id);
            shader_set_.emplace(shader_id);
        }

        void detach(shader const& _shader)
        {
            unsigned shader_id = _shader.get();
            detail::attach_shader(program_id_, shader_id);
            shader_set_.erase(shader_id);
        }

        bool link()
        {
            detail::link_program(program_id_);
            bool success = detail::get_program_status(program_id_, program_status::link_status);

            if (!success)
            {
                error_message_ = detail::get_program_info_log(program_id_);
            }
            else
            {
                for (auto shader_id : shader_set_)
                {
                    detail::delete_shader(shader_id);
                }

                shader_set_.clear();
            }

            return success;
        }

        bool enabled() const
        {
            return detail::is_program(program_id_);
        }

        void use()
        {
            detail::use_program(program_id_);
        }

        unsigned get() const
        {
            return program_id_;
        }

        void reset()
        {
            detail::delete_program(program_id_);
        }

        std::string get_error_message() const
        {
            return error_message_;
        }

    public:
        template<typename ValueType>
        void set_uniform(std::string const& uniform_name, ValueType value)
        {
            auto loc = detail::get_uniform_location(program_id_, uniform_name.c_str());
            detail::uniform(loc, value);
        }

        template<unsigned Count, typename ValueType>
        void set_uniform(std::string const& uniform_name, ValueType value)
        {
            auto loc = detail::get_uniform_location(program_id_, uniform_name.c_str());
            detail::uniform(loc, Count, std::forward<ValueType>(value));
        }

        void set_uniform_matrix(std::string const& matrix_name, unsigned count, float const* value)
        {
            auto loc = detail::get_uniform_location(program_id_, matrix_name.c_str());
            detail::uniform_matrix(loc, count, false, value);
        }

        void set_uniform_matrix(std::string const& matrix_name, glm::mat4 const& mat)
        {
            auto loc = detail::get_uniform_location(program_id_, matrix_name.c_str());
            detail::uniform_matrix(loc, 1, false, glm::value_ptr(mat));
        }

    private:
        unsigned program_id_;
        std::set<unsigned> shader_set_;
        std::string error_message_;
    };

    //
    //顶点数组对象： Vertex  Array  Object，VAO
    //顶点缓冲对象： Vertex  Buffer Object，VBO
    //索引缓冲对象： Element Buffer Object，EBO 或
    //索引缓冲对象： Index   Buffer Object，IBO
    //

    namespace detail
    {
        template<unsigned N>
        struct buffer_t
        {
            using value_type = decltype(N);
            constexpr static auto value = N;

            std::array<value_type, value> buffer{};

            value_type get(value_type index)
            {
                return buffer.at(index);
            }
        };

        template<unsigned N>
        struct texture_t
        {
            using value_type = decltype(N);
            constexpr static auto value = N;

            std::array<value_type, value> texture{};

            value_type get(value_type index)
            {
                return texture.at(index);
            }
        };

        struct vertex_attribute
        {
            void set(int loc_in_vs, unsigned loc_t_size, data_type dt, bool normalized, unsigned stride, int next)
            {
                detail::set_attribute(loc_in_vs, loc_t_size, dt, normalized, stride, reinterpret_cast<void*>(next));
                detail::enable_vertex_attrib_array(loc_in_vs);
            }
        };

        struct texture_attribute
        {
            void set(texture_feature feature, int value)
            {
                detail::texture_parameter(texture_type::_2d, feature, value);
            }

            void set(texture_feature feature, float value)
            {
                detail::texture_parameter(texture_type::_2d, feature, value);
            }

            void set(texture_feature feature, float const* values)
            {
                detail::texture_parameter_vector(texture_type::_2d, feature, values);
            }
        };

        struct copy_buffer_data
        {
            void from(void const* data, int size, buffer_usage_type usage_type = buffer_usage_type::static_draw)
            {
                detail::buffer_data(buffer_type::array_buffer, size, data, usage_type);
            }
        };

        struct copy_element_data
        {
            void from(void const* data, int size, buffer_usage_type usage_type = buffer_usage_type::static_draw)
            {
                detail::buffer_data(buffer_type::element_array_buffer, size, data, usage_type);
            }
        };

        struct copy_texture_data
        {
            int width;
            int height;

            void from(std::string const& image_file = ""s,
                texture_format _internal = texture_format::rgb,
                texture_format format = texture_format::rgb,
                bool flip = false)
            {
                if (!image_file.empty())
                {
                    if (flip)
                    {
                        stbi_set_flip_vertically_on_load(flip);
                    }

                    int comp;
                    auto data = stbi_load(image_file.c_str(), &width, &height, &comp, 0);
                    if (data)
                    {
                        detail::texture_image_2d(0,
                            _internal,
                            width, height,
                            format,
                            pixel_data_type::_unsigned_byte,
                            data);
                    }
                    stbi_image_free(data);
                }
            }

            void create_mipmap()
            {
                detail::texture_create_mipmap();
            }
        };

        template<unsigned N, template<unsigned> typename BufferT>
        struct vao_t : BufferT<N>
        {
            vao_t()
            {
                detail::create_vertex_arrays(buffer.data(), N);

            }

            ~vao_t()
            {
                detail::delete_vertex_arrays(static_cast<unsigned*>(buffer.data()), N);
            }

            void bind(typename BufferT<N>::value_type index)
            {
                detail::bind_vertex_array(get(index));
            }

            void unbind()
            {
                detail::unbind_vertex_array();
            }
        };

        template<unsigned N, template<unsigned> typename BufferT>
        struct vbo_t : BufferT<N>, copy_buffer_data, vertex_attribute
        {
            vbo_t()
            {
                detail::create_buffers(buffer.data(), N);
            }

            ~vbo_t()
            {
                detail::delete_buffers(buffer.data(), N);
            }

            void bind(typename BufferT<N>::value_type index)
            {
                detail::bind_buffer(buffer_type::array_buffer, get(index));
            }

            void unbind()
            {
                detail::unbind_buffer(buffer_type::array_buffer);
            }
        };

        template<unsigned N, template<unsigned> typename BufferT>
        struct ebo_t : BufferT<N>, copy_element_data
        {
            ebo_t()
            {
                detail::create_buffers(buffer.data(), N);
            }

            ~ebo_t()
            {
                detail::delete_buffers(buffer.data(), N);
            }

            void bind(typename BufferT<N>::value_type index)
            {
                detail::bind_buffer(buffer_type::element_array_buffer, get(index));
            }
        };

        template<unsigned N, template<unsigned> typename TextureT>
        struct texture_2d_t : texture_t<N>, texture_attribute, copy_texture_data
        {
            texture_2d_t()
            {
                detail::create_textures(texture.data(), N);
            }

            void bind(typename texture_t<N>::value_type index)
            {
                detail::bind_texture(texture_type::_2d, get(index));
            }

            void active(typename texture_t<N>::value_type index)
            {
                auto actived = get(index);
                if (actived > 0)
                {
                    detail::texture_active(GL_TEXTURE0 + actived - 1);
                }
            }
        };
    }

    template<unsigned N>
    using multi_vao = detail::vao_t<N, detail::buffer_t>;
    template<unsigned N>
    using multi_vbo = detail::vbo_t<N, detail::buffer_t>;
    template<unsigned N>
    using multi_ebo = detail::ebo_t<N, detail::buffer_t>;
    template<unsigned N>
    using multi_texture2d = detail::texture_2d_t<N, detail::texture_t>;

    using single_vao = detail::vao_t<1, detail::buffer_t>;
    using single_vbo = detail::vbo_t<1, detail::buffer_t>;
    using single_ebo = detail::ebo_t<1, detail::buffer_t>;
    using single_texture2d = detail::texture_2d_t<1, detail::texture_t>;

    class camera
    {
    public:
        enum class movement
        {
            forwrad,
            backward,
            left,
            right,
        };

        //欧拉角：俯仰角(Pitch)、偏航角(Yaw)和滚转角(Roll)
        constexpr static float yaw = -90.0f;
        constexpr static float pitch = 0.0f;

        constexpr static float speed = 2.5f;
        constexpr static float sensitivty = 0.1f; //灵敏度
        constexpr static float zoom = 45.0f;
    public:
        camera(glm::vec3 _pos={ 0.0f,0.0f,0.0f })
            : position_{_pos}
            , up_{0.0f,1.0f,0.0f}
            , front_{0.0f,0.0f,-1.0f}
            , right_{1.0f,0.0f,0.0f}
            , world_up_{0.0f,1.0f,0.0f}
        {
            update();
        }
    public:
        glm::mat4 get_view() const
        {
            auto view = glm::lookAt(position_, position_ + front_, up_);
            return view;
        }

        float get_zoom() const
        {
            return zoom_;
        }

    public:
        void key_press(movement direction, float delta_time)
        {
            auto velocity = speed * delta_time;
            switch (direction)
            {
            case movement::forwrad:
                position_ += front_ * velocity;
                break;
            case movement::backward:
                position_ -= front_ * velocity;
                break;
            case movement::left:
                position_ -= right_ * velocity;
                break;
            case movement::right:
                position_ += right_ * velocity;
                break;
            default:
                break;
            }
        }

        void mouse_move(float xoffset, float yoffset, bool constrain_pitch = true)
        {
            xoffset *= sensitivty;
            yoffset *= sensitivty;

            yaw_ += xoffset;
            pitch_ += yoffset;

            if (constrain_pitch)
            {
                if (pitch_ > 89.9f)pitch_ = 89.9f;
                if (pitch_ < -89.9f)pitch_ = -89.9f;
            }

            update();
        }

        void scroll(float yoffset)
        {
            if (zoom_ >= 1.0f && zoom_ <= 45.0f) zoom_ -= yoffset;
            if (zoom_ <= 1.0f) zoom_ = 1.0f;
            if (zoom_ >= 45.0f) zoom_ = 45.0f;
        }
    private:
        void update()
        {
            auto vfront = glm::vec3{
                std::cos(glm::radians(yaw_)) * std::cos(glm::radians(pitch_)),
                std::sin(glm::radians(pitch_)),
                std::sin(glm::radians(yaw_)) * std::cos(glm::radians(pitch_)),
            };
            
            front_ = glm::normalize(vfront);
            right_ = glm::normalize(glm::cross(front_, world_up_));
            up_    = glm::normalize(glm::cross(right_, front_));
        }
    private:
        glm::vec3 position_;
        glm::vec3 up_;
        glm::vec3 front_;
        glm::vec3 right_;
        glm::vec3 world_up_;

        float yaw_ = camera::yaw;
        float pitch_ = camera::pitch;
        float zoom_ = camera::zoom;
    };
}

//gl::function
namespace gl {

    void clear_color(float red, float green, float blue, float alpha = 1.0f)
    {
        detail::clear_color(red, green, blue, alpha);
    }

    void clear(bit_field mask)
    {
        detail::clear(mask);
    }

    bool enable_depth_test(bool enable = true)
    {
        return detail::enabled(enable_status::depth_test, -1, enable);
    }

    void view_port(int x, int y, int width, int height)
    {
        detail::view_port(x, y, width, height);
    }

    void draw_arrays(draw_mode mode, int first, int count)
    {
        detail::draw_arrays(mode, first, count);
    }

    void draw_elements(draw_mode mode, int count, data_type dt, void const* indices)
    {
        detail::draw_elements(mode, count, dt, indices);
    }

    shader make_vertex_shader(std::string const& source)
    {
        return make<shader>(shader_type::vertex_shader, source);
    }

    shader make_fragment_shader(std::string const& source)
    {
        return make<shader>(shader_type::fragment_shader, source);
    }

    auto make_empty_program()
    {
        return make<shader_program>();
    }

    auto make_shader_program(std::string const& vs, std::string const& fs)
    {
        auto sp = make_empty_program();
        std::string err;

        auto vss = make_vertex_shader(vs);
        if (!vss.compile())
        {
            err = vss.get_error_message();
            sp.reset();
            return sp;
        }

        auto fss = make_fragment_shader(fs);
        if (!fss.compile())
        {
            err = fss.get_error_message();
            sp.reset();
            return sp;
        }

        sp.attach(vss);
        sp.attach(fss);

        if (!sp.link())
        {
            err = sp.get_error_message();
            sp.detach(vss);
            sp.detach(fss);
            sp.reset();
            return sp;
        }

        return sp;
    }

    auto make_single_texture(std::string const& texture_file,
        texture_format _internal = texture_format::rgb,
        texture_format format = texture_format::rgb,
        bool flip = false)
    {
        auto single_texture2d = make<gl::single_texture2d>();
        single_texture2d.bind(0);

        single_texture2d.set(gl::texture_feature::wrap_s, GL_REPEAT);
        single_texture2d.set(gl::texture_feature::wrap_t, GL_REPEAT);
        single_texture2d.set(gl::texture_feature::min_filter, GL_LINEAR);
        single_texture2d.set(gl::texture_feature::mag_filter, GL_LINEAR);

        single_texture2d.from(texture_file.c_str(), _internal, format, flip);
        single_texture2d.create_mipmap();

        return single_texture2d;
    }
}