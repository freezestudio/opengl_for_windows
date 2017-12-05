//
// Created by freezestudio on 2017/10/11.
//

#ifndef FREEGL_ANY_BUFFER_H
#define FREEGL_ANY_BUFFER_H

namespace freeze
{

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
        }

        void unbind()
        {
            glBindBuffer(Target, 0);
            assert_error();
        }

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
    template<typename = void>
    struct uniform_buffer_t
        : buffer_t<GL_UNIFORM_BUFFER>
    {
        void block_binding(GLuint program, std::string const& name, GLuint index)
        {
            auto block_index = glGetUniformBlockIndex(program, name.c_str());
            assert_error();
            glUniformBlockBinding(program, block_index, index);
            assert_error();
        }

        void bind_base(GLuint index)
        {
            glBindBufferBase(GL_UNIFORM_BUFFER, index, this->ref());
            assert_error();
        }

        void bind_range(GLuint index, GLintptr offset, GLsizeiptr size)
        {
            glBindBufferRange(GL_UNIFORM_BUFFER, index, this->ref(), offset, size);
            assert_error();
        }
    };
}

namespace freeze
{
    template<typename = void>
    struct render_buffer_t
        : make_object<render_buffer_t<void>>
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
            glBindRenderbuffer(GL_RENDERBUFFER, this->ref());
            assert_error();
        }

        void unbind()
        {
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            assert_error();
        }

		//color-renderable GL_RED GL_RG GL_RGB GL_RGBA
		//depth-renderable GL_DEPTH_COMPONENT GL_STENCIL_INDEX GL_DEPTH_STENCIL
		//                 GL_DEPTH_COMPONENT<16,24,32,32F> GL_DEPTH24_STENCIL8 GL_DEPTH32F_STENCIL8
		//stencil-renderable GL_STENCIL_INDEX8
        void storage(GLsizei width, GLsizei height,GLenum internal_format = GL_DEPTH24_STENCIL8)
        {
            glRenderbufferStorage(GL_RENDERBUFFER, internal_format, width, height);
            assert_error();
        }

        void storage_multisample(GLsizei samples,GLenum internalformat,GLsizei width,GLsizei height)
        {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalformat, width, height);
            assert_error();
        }
    };
}

namespace freeze
{
    //
    // 附加至少一个缓冲（颜色、深度或模板缓冲）。
    // 至少有一个颜色附件(Attachment)。
    // 所有的附件都必须是完整的（保留了内存）。
    // 每个缓冲都应该有相同的样本数。
    // GL_FRAMEBUFFER
    // GL_DRAW_FRAMEBUFFER  (= GL_FRAMEBUFFER)
    // GL_READ_FRAMEBUFFER
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

        // GL_FRAMEBUFFER_COMPLETE 0x8CD5==36053
        // GL_FRAMEBUFFER_UNDEFINED 0x8219
        // GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
        // GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
        // GL_FRAMEBUFFER_UNSUPPORTED
        // GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
        // GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
        GLenum check_status()
        {
            auto status = glCheckFramebufferStatus(FrameBuffer);
            assert_error();
            return status;
        }

        bool is_complete()
        {
            auto status = check_status();
            assert_error();
            return status == GL_FRAMEBUFFER_COMPLETE;
        }

        template<typename Texture>
        void attachement_texture(GLenum attachment, Texture const& texture, GLint level = 0)
        {
            glFramebufferTexture(FrameBuffer, attachment, texture.ref(), level);
            assert_error();
        }

        template<typename Texture>
        void attachement_color(Texture const& texture,GLenum textarget= GL_TEXTURE_2D,GLint level=0)
        {
            glFramebufferTexture2D(FrameBuffer, GL_COLOR_ATTACHMENT0, textarget, texture.ref(), level);
            assert_error();
        }

        void attachement_texture2d(GLenum attachment, GLuint texture, GLenum textarget = GL_TEXTURE_2D, GLuint level = 0)
        {
            glFramebufferTexture2D(FrameBuffer, attachment, textarget, texture, level);
            assert_error();
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

        //mask -- GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT, GL_STENCIL_BUFFER_BIT
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

    using vertex_buffer = buffer_t<GL_ARRAY_BUFFER>;
    using element_buffer = buffer_t<GL_ELEMENT_ARRAY_BUFFER>;
    using vertex_array_buffer = vertex_array_buffer_t<false>;
    using uniform_buffer = uniform_buffer_t<>;
    using render_buffer = render_buffer_t<>;
    using frame_buffer = frame_buffer_t<GL_FRAMEBUFFER>;
    using read_frame_buffer = frame_buffer_t<GL_READ_FRAMEBUFFER>;
    using draw_frame_buffer = frame_buffer_t<GL_DRAW_FRAMEBUFFER>;

    constexpr auto make_vertex_buffer = make<vertex_buffer>;
    constexpr auto make_element_buffer = make<element_buffer>;
    constexpr auto make_vertex_array_buffer = make<vertex_array_buffer>;
    constexpr auto make_uniform_buffer = make<uniform_buffer>;
    constexpr auto make_render_buffer = make<render_buffer>;
    constexpr auto make_frame_buffer = make<frame_buffer>;
    constexpr auto make_read_frame_buffer = make<read_frame_buffer>;
    constexpr auto make_draw_frame_buffer = make<draw_frame_buffer>;
}


#endif //FREEGL_ANY_BUFFER_H
