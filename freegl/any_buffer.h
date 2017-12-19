//
// Created by freezestudio on 2017/10/11.
//

#ifndef FREEGL_ANY_BUFFER_H
#define FREEGL_ANY_BUFFER_H

namespace freeze
{
    //
    //Target :
    //
    //GL_ARRAY_BUFFER
    //    缓冲区将被用作顶点数据的源，但只有在调用glVertexAttribPointer时才会进行连接。
    //    该函数的指针字段取自当前绑定到此目标的任何缓冲区的开始的字节偏移量。
    //GL_ELEMENT_ARRAY_BUFFER
    //    gl*Draw*Elements* 形式的所有渲染函数将使用指针字段作为绑定到此目标的缓冲区对象的开头的字节偏移量。
    //    用于索引渲染的索引将取自缓冲区对象。
    //    请注意，此绑定目标是“ 顶点数组对象”状态的一部分，因此必须在此处绑定缓冲区之前绑定VAO。
    //GL_COPY_READ_BUFFER和GL_COPY_WRITE_BUFFER
    //    没有特定的语义。
    //    因为它们没有实际意义，所以它们是用glCopyBufferSubData复制缓冲区对象数据的有用目标。
    //    拷贝时不必使用这些目标，但是通过使用它们，可以避免具有实际语义的令人不安的缓冲区目标。
    //GL_PIXEL_UNPACK_BUFFER和GL_PIXEL_PACK_BUFFER
    //    用于执行异步像素传输操作。
    //    如果缓冲区绑定到GL_PIXEL_UNPACK_BUFFER，
    //    则glTexImage*，glTexSubImage*，glCompressedTexImage*和glCompressedTexSubImage*都将受到影响。
    //    这些函数将从绑定的缓冲区对象读取它们的数据，而不是客户端指针指向的位置。
    //    同样，如果一个缓冲区绑定到GL_PIXEL_PACK_BUFFER，
    //    glGetTexImage和glReadPixels将把它们的数据存储到绑定的缓冲区对象，而不是客户端指针指向的地方。
    //GL_QUERY_BUFFER
    //    用于执行从异步查询到缓冲区对象内存的直接写入。
    //    如果一个缓冲区绑定到GL_QUERY_BUFFER，
    //    那么所有的glGetQueryObject[ui64v]函数调用都会将结果写入一个偏移量到绑定的缓冲区对象中。
    //GL_TEXTURE_BUFFER
    //    没有特殊的语义，
    //    但是如果你打算为缓冲区纹理使用一个缓冲区对象，当你第一次创建它的时候，在这里绑定它是一个好主意。
    //GL_TRANSFORM_FEEDBACK_BUFFER
    //    Transform Feedback操作中使用的缓冲区的索引缓冲区绑定。
    //GL_UNIFORM_BUFFER
    //    用作缓冲区的索引缓冲区绑定，用作统一块的存储。
    //GL_DRAW_INDIRECT_BUFFER
    //    绑定到此目标的缓冲区在执行间接渲染时将用作间接数据的源。仅在核心OpenGL 4.0或ARB_draw_indirect中可用。
    //GL_ATOMIC_COUNTER_BUFFER
    //    用作原子计数器存储的缓冲区的索引缓冲区绑定。需要OpenGL 4.2或ARB_shader_atomic_counters
    //GL_DISPATCH_INDIRECT_BUFFER
    //    绑定到此目标的缓冲区将通过glDispatchComputeIndirect用作间接计算调度操作的源。需要OpenGL 4.3或ARB_compute_shader。
    //GL_SHADER_STORAGE_BUFFER
    //    用作存储着色器存储块的缓冲区的索引缓冲区绑定。
    //
    template<GLenum Target>
    struct buffer_t : make_object<buffer_t<Target>>
    {
        void create(GLuint *buffers)
        {
            glGenBuffers(1, buffers);
            assert_error();
        }

        void destroy(GLuint const *buffers)
        {
            glDeleteBuffers(1, buffers);
            assert_error();
        }

        void bind()
        {
            glBindBuffer(Target, this->ref());
			assert_error();
        }

        void unbind()
        {
            glBindBuffer(Target, 0);
            assert_error();
        }

        //
        //STREAM:  数据存储内容将被修改一次，最多使用几次。
        //STATIC:  数据存储内容将被修改一次，并多次使用。
        //DYNAMIC: 数据存储内容将被重复修改，并多次使用。
        //
        //DRAW：用户将数据写入缓冲区，但用户不会读取它。
        //READ：用户将不会写入数据，但用户将读取数据。
        //COPY：用户既不会写也不能读数据。
        void copy_data(void const *data, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW)
        {
            glBufferData(Target, size, data, usage);
            assert_error();
        }

        void copy_subdata(void const *data, GLsizeiptr size, GLintptr offset)
        {
            glBufferSubData(Target, offset, size, data);
            assert_error();
        }

        void copy_subdata(GLenum readTarget,
            GLenum writeTarget,
            GLintptr readOffset,
            GLintptr writeOffset,
            GLsizeiptr size)
        {
            glCopyBufferSubData(readTarget,
                writeTarget,
                readOffset,
                writeOffset,
                size);
            assert_error();
        }

        void get_subdata(void* data, GLintptr offset, GLsizeiptr size)
        {
            glGetBufferSubData(Target, offset, size, data);
            assert_error();
        }

        void clear_data(GLenum internalformat,
            GLenum format,
            GLenum type,
            const void * data)
        {
            glClearBufferData(Target, internalformat,
                format, type, data);
            assert_error();
        }

        void cleare_subdata(GLenum internalformat,
            GLintptr offset,
            GLsizeiptr size,
            GLenum format,
            GLenum type,
            const void * data)
        {
            glClearBufferSubData(Target,
                internalformat,
                offset,
                size,
                format,
                type,
                data);
            assert_error();
        }

        //直接赋值支持
        void *map_buffer(GLintptr offset, GLsizeiptr length, GLbitfield access) const
        {
            auto pbuffer = glMapBufferRange(Target, offset, length, access);
            assert_error();
            return pbuffer;
        }

        bool unmap_buffer()
        {
            auto unmap = glUnmapBuffer(Target);
            assert_error();
            return unmap == GL_TRUE;
        }

        void flush(GLintptr offset, GLsizeiptr length)
        {
            glFlushMappedBufferRange(Target,offset,length);
            assert_error();
        }

        void invalidate()
        {
            glInvalidateBufferData(this->ref());
            assert_error();
        }

        void invalidate_subdata(GLintptr offset, GLsizeiptr length)
        {
            glInvalidateBufferSubData(this->ref(), offset, length);
            assert_error();
        }
    };
}

namespace freeze
{
    template<bool Delay>
    struct vertex_array_buffer_t
        : make_object<vertex_array_buffer_t<Delay>,true,Delay>
    {
        void create(GLuint* buffers)
        {
            glGenVertexArrays(1, buffers);
            assert_error();
        }

        void destroy(GLuint const* buffers)
        {
            glDeleteVertexArrays(1, buffers);
            assert_error();
        }

        void bind()
        {
            glBindVertexArray(this->ref());
            assert_error();
        }

        void unbind()
        {
            glBindVertexArray(0);
            assert_error();
        }
    };
}

//layout (std140) uniform XXX{ };
//ver4.2 layout(std140,binding = <0,1,2,...>) uniform XXX { };
namespace freeze
{
    template<GLenum UB = GL_UNIFORM_BUFFER>
    struct uniform_buffer_t : buffer_t<UB>
    {
        //ver4.2 layout(std140,binding=x) uniform xxx{...}
        void block_binding(GLuint program, std::string const& name, GLuint index)
        {
            auto block_index = glGetUniformBlockIndex(program, name.c_str());
            assert_error();
            glUniformBlockBinding(program, block_index, index);
            assert_error();
        }

        void bind_base(GLuint index)
        {
            glBindBufferBase(UB, index, this->ref());
            assert_error();
        }

        void bind_range(GLuint index, GLintptr offset, GLsizeiptr size)
        {
            glBindBufferRange(UB, index, this->ref(), offset, size);
            assert_error();
        }
    };
}

namespace freeze
{
	namespace detail
	{
		constexpr GLenum color_render[] = {
			GL_RED,
			GL_RG,
			GL_RGB,
			GL_RGBA,
		};
		constexpr auto color_render_size = sizeof(color_render) / sizeof(color_render[0]);

		constexpr GLenum depth_render[] = {
			GL_DEPTH_COMPONENT,
			GL_DEPTH_COMPONENT16,
			GL_DEPTH_COMPONENT24,
			GL_DEPTH_COMPONENT32,
			GL_DEPTH_COMPONENT32F,
		};
		constexpr auto depth_render_size = sizeof(depth_render) / sizeof(depth_render[0]);

		constexpr GLenum depth_stencil_render[] = {
			GL_DEPTH_STENCIL,
			GL_DEPTH24_STENCIL8,
			GL_DEPTH32F_STENCIL8,
		};
		constexpr auto depth_stencil_render_size = sizeof(depth_stencil_render) / sizeof(depth_stencil_render[0]);

		constexpr GLenum stencil_render = GL_STENCIL_INDEX8;

		constexpr GLenum get_attachment_type(GLenum render_type)
		{
			for (auto i = 0; i < color_render_size; ++i)
			{
				if (render_type == color_render[i])
				{
					return GL_COLOR_ATTACHMENT0;
				}
			}

			for (auto i = 0; i < depth_render_size; ++i)
			{
				if (render_type == depth_render[i])
				{
					return GL_DEPTH_ATTACHMENT;
				}
			}

			for (auto i = 0; i < depth_stencil_render_size; ++i)
			{
				if (render_type == depth_stencil_render[i])
				{
					return GL_DEPTH_STENCIL_ATTACHMENT;
				}
			}

			if (render_type == stencil_render)
			{
				return GL_STENCIL_ATTACHMENT;
			}

			return -1;
		}
	}
}

namespace freeze
{
    template<GLenum RB = GL_RENDERBUFFER>
    struct render_buffer_t
        : make_object<render_buffer_t<RB>>
    {
        void create(GLuint* buffers)
        {
            glGenRenderbuffers(1, buffers);
            assert_error();
        }

        void destroy(GLuint const* buffers)
        {
            glDeleteRenderbuffers(1, buffers);
            assert_error();
        }

        void bind()
        {
            glBindRenderbuffer(RB, this->ref());
            assert_error();
        }

        void unbind()
        {
            glBindRenderbuffer(RB, 0);
            assert_error();
        }

		//color-renderable  -- GL_RED, GL_RG, GL_RGB, GL_RGBA
		//depth-renderable  -- GL_DEPTH_COMPONENT, GL_STENCIL_INDEX, GL_DEPTH_STENCIL, 
		//                     GL_DEPTH_COMPONENT<16,24,32,32F>, GL_DEPTH24_STENCIL8, 
		//                     GL_DEPTH32F_STENCIL8
		//stencil-renderable-- GL_STENCIL_INDEX8
        void storage(GLsizei width, GLsizei height,GLenum internal_format = GL_DEPTH24_STENCIL8)
        {
            glRenderbufferStorage(RB, internal_format, width, height);
			attachment_type_ = detail::get_attachment_type(internal_format);
            assert_error();
        }

        void storage_multisample(GLsizei samples,GLenum internalformat,GLsizei width,GLsizei height)
        {
            glRenderbufferStorageMultisample(RB, samples, internalformat, width, height);
            assert_error();
        }

		GLenum attachment_type() const
		{
			return attachment_type_;
		}
	private:
		GLenum attachment_type_ = -1;
    };
}

namespace freeze
{
    //
    // 附加至少一个缓冲（颜色、深度或模板缓冲）。
    // 至少有一个颜色附件(Attachment)。
    // 所有的附件都必须是完整的（保留了内存）。
    // 每个缓冲都应该有相同的样本数。
	// FrameBuffer:
    //     GL_FRAMEBUFFER
    //     GL_DRAW_FRAMEBUFFER  (= GL_FRAMEBUFFER)
    //     GL_READ_FRAMEBUFFER
    //
    template<GLenum FrameBuffer>
    struct frame_buffer_t
        : make_object<frame_buffer_t<FrameBuffer>>
    {
        void create(GLuint* buffers)
        {
            glGenFramebuffers(1, buffers);
            assert_error();
        }

        void destroy(GLuint const* buffers)
        {
            glDeleteFramebuffers(1, buffers);
            assert_error();
        }

        void bind()
        {
            glBindFramebuffer(FrameBuffer, this->ref());
            assert_error();
        }

        void spec_bind(GLenum target)
        {
            glBindFramebuffer(tagert, this->ref());
            assert_error();
        }

        void unbind()
        {
            glBindFramebuffer(FrameBuffer, 0);
            assert_error();
        }

        void spec_unbind(GLenum target)
        {
            glBindFramebuffer(target, 0);
            assert_error();
        }

		// status:
        //     GL_FRAMEBUFFER_COMPLETE  0x8CD5==36053
        //     GL_FRAMEBUFFER_UNDEFINED 0x8219
        //     GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
        //     GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
        //     GL_FRAMEBUFFER_UNSUPPORTED
        //     GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
        //     GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
        GLenum check_status() const
        {
            auto status = glCheckFramebufferStatus(FrameBuffer);
            assert_error();
            return status;
        }

        bool is_complete() const
        {
            auto status = check_status();
            assert_error();
            return status == GL_FRAMEBUFFER_COMPLETE;
        }

		//textarget:
		//    GL_TEXTURE_2D,
		//    GL_TEXTURE_RECTANGLE, 
		//    GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 
		//    GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		//    GL_TEXTURE_2D_MULTISAMPLE
        template<typename Texture>
        void attachement_color(Texture&& texture,GLuint index = 0,GLenum textarget= GL_TEXTURE_2D,GLint level=0)
        {
            glFramebufferTexture2D(FrameBuffer, GL_COLOR_ATTACHMENT0+index, textarget, texture.ref(), level);
            assert_error();
        }

        template<typename Texture>
        void attachement_texture(Texture&& texture, GLenum attachment, GLint level = 0)
        {
            glFramebufferTexture(FrameBuffer, attachment, texture.ref(), level);
            assert_error();
        }

		void attachement_texture1d(GLuint texture,GLenum attachment = GL_COLOR_ATTACHMENT0, GLint level = 0)
		{
			glFramebufferTexture1D(FrameBuffer,attachment, GL_TEXTURE_1D,texture,level);
			assert_error();
		}

		//textarget:
		//    GL_TEXTURE_2D,
		//    GL_TEXTURE_RECTANGLE, 
		//    GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 
		//    GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		//    GL_TEXTURE_2D_MULTISAMPLE
        void attachement_texture2d(GLuint texture, GLenum attachment, GLenum textarget = GL_TEXTURE_2D, GLuint level = 0)
        {
            glFramebufferTexture2D(FrameBuffer, attachment, textarget, texture, level);
            assert_error();
        }

		void attachement_texture3d(GLuint texture, GLenum attachment = GL_COLOR_ATTACHMENT0, GLint level = 0,GLint zoffset = 0)
		{
			glFramebufferTexture3D(FrameBuffer,GL_COLOR_ATTACHMENT0,GL_TEXTURE_3D,texture,level,zoffset);
		}

        void attachement_depth(GLuint texture)
        {
            glFramebufferTexture2D(FrameBuffer, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
            assert_error();
        }

        void attachement_stencil(GLuint texture)
        {
            glFramebufferTexture2D(FrameBuffer, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
            assert_error();
        }

        // glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0,
        //               GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
        void attachement_depth_stencil(GLuint texture)
        {
            glFramebufferTexture2D(FrameBuffer, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
            assert_error();
        }

		//attachment -- GL_COLOR_ATTACHMENT<i>
		//              GL_DEPTH_ATTACHMENT
		//              GL_STENCIL_ATTACHMENT
		//              GL_DEPTH_STENCIL_ATTACHMENT
        void attachement_render_buffer(GLuint renderbuffer,GLenum attachment = GL_DEPTH_STENCIL_ATTACHMENT)
        {
            glFramebufferRenderbuffer(FrameBuffer, attachment, GL_RENDERBUFFER, renderbuffer);
            assert_error();
        }

		void attachement_render_buffer(render_buffer_t<GL_RENDERBUFFER> const& renderbuffer)
		{			
			glFramebufferRenderbuffer(FrameBuffer, renderbuffer.attachment_type(), GL_RENDERBUFFER, renderbuffer);
			assert_error();
		}

        void draw_buffer(GLenum buf)
        {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
            glDrawBuffer(buf);
#endif
#if defined(ANDROID) || defined(__ANDROID__)
            glDrawBuffers(1, &buf);
#endif
            assert_error();
        }

        void read_buffer(GLenum mode)
        {
            glReadBuffer(mode);
            assert_error();
        }

        void read_pixels(GLint x,GLint y,GLsizei width,GLsizei height,GLenum format,GLenum type,GLvoid* data)
        {
            glReadPixels(x,y,width,height,format,type,data);
            assert_error();
        }

		//usage:
		//    src:GL_DRAW_FRAMEBUFFER
		//    dst:GL_READ_FRAMEBUFFER
        //mask   -- GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT, GL_STENCIL_BUFFER_BIT
        //filter -- GL_NEAREST, GL_LINEAR
        void blit(GLint srcx0,GLint srcy0,GLint srcx1,GLint srcy1,
            GLint dstx0,GLint dsty0,GLint dstx1,GLint dsty1,
            GLbitfield mask,GLenum filter)
        {
            glBlitFramebuffer(srcx0,srcy0,srcx1,srcy1,dstx0,dsty0,dstx1,dsty1,mask,filter);
            assert_error();
        }
    };
}

namespace freeze
{
	template<GLenum TFB = GL_TRANSFORM_FEEDBACK>
	struct transform_feedback_t
		: make_object<transform_feedback_t<TFB>>
	{
		void create(GLuint* buffers)
		{
			glGenTransformFeedbacks(1, buffers);
			assert_error();
		}

		void destroy(GLuint const* buffers)
		{
			glDeleteTransformFeedbacks(1, buffers);
			assert_error();
		}

		void bind()
		{
			glBindTransformFeedback(TFB, this->ref());
			assert_error();
		}

		void unbind()
		{
			glBindTransformFeedback(TFB, 0);
			assert_error();
		}

		void bind_base(GLuint buffer, GLuint index)
		{
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, index, buffer);
			assert_error();
		}

		void bind_range(GLuint buffer,GLuint index, GLintptr offset, GLsizeiptr size)
		{
			glBindBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, index, buffer, offset, size);
			assert_error();
		}

		void copy_data(void const *data, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW)
		{
			glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, size, data, usage);
			assert_error();
		}

		void copy_subdata(void const *data, GLsizeiptr size, GLintptr offset)
		{
			glBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, offset, size, data);
			assert_error();
		}

        void get_subdata(void* data,GLintptr offset,GLsizeiptr size)
        {
            glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, offset, size, data);
            assert_error();
        }

        //void varyings(GLuint program, GLsizei count, const char **varyings, GLenum bufferMode)
        //{
        //    void glTransformFeedbackVaryings(program, count,varyings,bufferMode);
        //    assert_error();
        //}

        //GL_POINTS,GL_LINES,GL_TRIANGLES
        static void begin(GLenum primitiveMode)
        {
            glBeginTransformFeedback(primitiveMode);
            assert_error();
        }

        static void end()
        {
            glEndTransformFeedback();
            assert_error();
        }

        static void pause()
        {
            glPauseTransformFeedback();
            assert_error();
        }

        static void resume()
        {
            glResumeTransformFeedback();
            assert_error();
        }

        //GL_POINTS, 
        //GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY, 
        //GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY, GL_TRIANGLES_ADJACENCY,
        //GL_PATCHES
        void draw(GLenum mode)
        {
            glDrawTransformFeedback(mode,this->ref());
            assert_error();
        }

        void draw_instanced(GLenum mode, GLsizei primcount)
        {
            glDrawTransformFeedbackInstanced(mode,this->ref(), primcount);
            assert_error();
        }

        void draw_stream(GLenum mode, GLuint stream)
        {
            glDrawTransformFeedbackStream(GLenum mode,this->ref(), stream);
            assert_error();
        }

        void draw_stream_instanced(GLenum mode, GLuint stream, GLsizei primcount)
        {
            glDrawTransformFeedbackStreamInstanced(GLenum mode,this->ref(), tream, primcount);
            assert_error();
        }
	};
}

namespace freeze
{
    template<GLenum TQ = GL_QUERY>
    struct query_t : make_object<query_t<TQ>>
    {
        void create(GLuint* buffer)
        {
            glGenQueries(1, buffer);
            assert_error();
        }

        void destroy(GLuint const* buffer)
        {
            glDeleteQueries(1, buffer);
            assert_error();
        }

        void begin(GLenum buffer)
        {
            glBeginQuery(buffer, this->ref());
            assert_error();
        }

        void end(GLenum buffer)
        {
            glEndQuery(buffer);
        }

        //pname:
        //    GL_QUERY_RESULT
        //    GL_QUERY_RESULT_NO_WAIT
        //    GL_QUERY_RESULT_AVAILABLE
        GLuint get(GLenum pname)
        {
            GLuint ret;
            glGetQueryObjectuiv(this->ref(), pname, &ret);
            assert_error();
            return ret;
        }
    };
}

namespace freeze
{

    using vertex_buffer = buffer_t<GL_ARRAY_BUFFER>;
    using element_buffer = buffer_t<GL_ELEMENT_ARRAY_BUFFER>;
    using vertex_array_buffer = vertex_array_buffer_t<false>;
    using uniform_buffer = uniform_buffer_t<>;
    using render_buffer = render_buffer_t<>;
    using frame_buffer = frame_buffer_t<GL_FRAMEBUFFER>;
    using read_frame_buffer = frame_buffer_t<GL_READ_FRAMEBUFFER>;
    using draw_frame_buffer = frame_buffer_t<GL_DRAW_FRAMEBUFFER>;
	using transform_feedback = transform_feedback_t<>;

    constexpr auto make_vertex_buffer = make<vertex_buffer>;
    constexpr auto make_element_buffer = make<element_buffer>;
    constexpr auto make_vertex_array_buffer = make<vertex_array_buffer>;
    constexpr auto make_uniform_buffer = make<uniform_buffer>;
    constexpr auto make_render_buffer = make<render_buffer>;
    constexpr auto make_frame_buffer = make<frame_buffer>;
    constexpr auto make_read_frame_buffer = make<read_frame_buffer>;
    constexpr auto make_draw_frame_buffer = make<draw_frame_buffer>;
    constexpr auto make_transform_feedback = make<transform_feedback>;

    constexpr auto make_vbo = make<vertex_buffer>;
    constexpr auto make_ebo = make<element_buffer>;
    constexpr auto make_vao = make<vertex_array_buffer>;
    constexpr auto make_ubo = make<uniform_buffer>;
    constexpr auto make_rbo = make<render_buffer>;
    constexpr auto make_tfo = make<transform_feedback>;

    using vbo = vertex_buffer;
    using ebo = element_buffer;
    using vao = vertex_array_buffer;
    using ubo = uniform_buffer;
    using rbo = render_buffer;
    using fbo = frame_buffer;
    //using tbo = transform_feedback_buffer;

    using tfo = transform_feedback;
}


#endif //FREEGL_ANY_BUFFER_H
