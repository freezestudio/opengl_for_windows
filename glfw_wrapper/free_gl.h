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
		// ----------------------------------------------------------- ȱʡֵ ----------------------------- ֧�ֵ�ֵ --------
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
		release,press,repeat
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

#pragma region �ڵ��� init() ֮ǰ����

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

	class window
	{
	public:
		using pointer = GLFWwindow*;
		using const_pointer = GLFWwindow const*;
		window() : windowp_(nullptr)
		{

		}

		window(int width, int height, std::string const& title)
		{
			windowp_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
		}

		window(int width, int height, std::string const& title, monitor& _monitor, window& _window)
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

		void process_input_event(key_code key)
		{
			if (get_key(key) == key_mouse_action::press)
			{
				set_should_close();
			}
		}

		pointer get()
		{
			return windowp_;
		}

		const_pointer get() const
		{
			return windowp_;
		}

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

		key_mouse_action get_key(key_code key) const
		{
			int action = glfwGetKey(windowp_, static_cast<int>(key));
			return static_cast<key_mouse_action>(action);
		}

	private:
		GLFWwindow* windowp_;
	};

	class monitor
	{
	public:

		monitor(monitor const& rhs)
			: monitorp_(rhs.monitorp_)
		{

		}

		monitor& operator=(GLFWmonitor* _monitor)
		{
			this->monitorp_ = _monitor;
			return *this;
		}

	protected:
		monitor() : monitorp_(nullptr)
		{

		}
	private:
		GLFWmonitor* monitorp_;
	};

}

//glfw::function
namespace glfw {
	void set_window_should_close(window::pointer win,bool _close = true)
	{
		glfwSetWindowShouldClose(win, _close ? GL_TRUE : GL_FALSE);
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
		stream_draw = GL_STREAM_DRAW,   //����ÿ�λ���ʱ����ı�
		stream_read = GL_STREAM_READ, 	//
		stream_copy = GL_STREAM_COPY, 	//
		static_draw = GL_STATIC_DRAW, 	//���ݲ���򼸺�����ı䡣
		static_read = GL_STATIC_READ, 	//
		static_copy = GL_STATIC_COPY,	//
		dynamic_draw = GL_DYNAMIC_DRAW,	//���ݻᱻ�ı�ܶ�
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

		void enable_vertex_attrib_array(int loc)
		{
			glEnableVertexAttribArray(loc);
		}

		void set_attribute(int loc_in_vs, unsigned loc_t_size, data_type dt, bool normalized, unsigned stride, void* next)
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

		void delete_buffers(unsigned* buffers, int n)
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

		void delete_vertex_arrays(unsigned* buffers, int n)
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

		//texture


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
		void draw_arrays(draw_mode mode,int first,int count)
		{
			glDrawArrays(static_cast<unsigned>(mode), first, count);
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
		shader(shader_type type, std::string const& source_file)
			: shader_id_(detail::create_shader(type))
		{
			if (shader_id_ > 0)
			{
				std::string source = detail::load_shader(source_file);
				const char* source_string = source.c_str();
				detail::shader_source(shader_id_, 1, &source_string, nullptr);
			}

		}

	public:
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

	private:
		unsigned program_id_;
		std::set<unsigned> shader_set_;
		std::string error_message_;
	};


	//
	//����������� Vertex  Array  Object��VAO
	//���㻺����� Vertex  Buffer Object��VBO
	//����������� Element Buffer Object��EBO ��
	//����������� Index   Buffer Object��IBO
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

		struct attribute
		{
			void set(int loc_in_vs, unsigned loc_t_size, data_type dt, bool normalized, unsigned stride, int next)
			{
				detail::set_attribute(loc_in_vs, loc_t_size, dt, normalized, stride, (void*)next);
				detail::enable_vertex_attrib_array(loc_in_vs);
			}
		};

		struct copy_data
		{
			void from(void const* data, int size, buffer_usage_type usage_type = buffer_usage_type::static_draw)
			{
				detail::buffer_data(buffer_type::array_buffer, size, data, usage_type);
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
				detail::bind_vertex_array(BufferT<N>::get(index));
			}

			void unbind()
			{
				detail::unbind_vertex_array();
			}
		};

		template<unsigned N,template<unsigned> typename BufferT>
		struct vbo_t : BufferT<N>, copy_data, attribute
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
				detail::bind_buffer(buffer_type::array_buffer, BufferT<N>::get(index));
			}

			void unbind()
			{
				detail::unbind_buffer(buffer_type::array_buffer);
			}
		};

	}

	//template<unsigned N>
	//constexpr detail::vbo_t<N, detail::buffer_t> vbo_n{};

	//template<unsigned N>
	//constexpr detail::vao_t<N, detail::buffer_t> vao_n{};

	using single_vbo = detail::vbo_t<1, detail::buffer_t>;
	using single_vao = detail::vao_t<1, detail::buffer_t>;

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

	void view_port(int x, int y, int width, int height)
	{
		detail::view_port(x, y, width, height);
	}

	void draw_arrays(draw_mode mode, int first, int count)
	{
		detail::draw_arrays(mode, first, count);
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
		if (!vss.compile())
		{
			err = vss.get_error_message();
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
}