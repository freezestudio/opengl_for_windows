//
// Created by ysb on 2017/10/11.
//

#ifndef CDS_ANY_BUFFER_H
#define CDS_ANY_BUFFER_H

namespace freeze {

    template<GLenum Target>
    struct any_buffer : make_object<any_buffer<Target>> {
        void create(GLuint *buffers) {
            glGenBuffers(1, buffers);
        }

        void destroy(GLuint const *buffers) {
            glDeleteBuffers(1, buffers);
        }

        void bind() {
            glBindBuffer(Target, this->ref());
        }

        void unbind() {
            glBindBuffer(Target, 0);
        }

        void copy_data(void const *data, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW) {
            glBufferData(Target, size, data, usage);
            auto err = glGetError();
            if(err != GL_NO_ERROR){
                LOGE("copy data to buffer error : %d",err);
            }
        }

        void copy_subdata(void const *data, GLsizeiptr size, GLintptr offset) {
            glBufferSubData(Target, offset, size, data);
            auto err = glGetError();
            if(err != GL_NO_ERROR){
                LOGE("copy sub data to buffer error : %d",err);
            }
        }

        //直接赋值支持
        void *map_buffer(GLintptr offset, GLsizeiptr length, GLbitfield access) const {
            return glMapBufferRange(Target, offset, length, access);
        }

        bool unmap_buffer() {
            auto unmap = glUnmapBuffer(Target);
            return unmap == GL_TRUE;
        }
    };
}

namespace freeze {
    template<typename = void>
    struct void_vertex_array_buffer : make_object<void_vertex_array_buffer<void>>{
        void create(GLuint* buffers){
            glGenVertexArrays(1,buffers);
        }

        void destroy(GLuint const* buffers){
            glDeleteVertexArrays(1,buffers);
        }

        void bind(){
            glBindVertexArray(this->ref());
        }

        void unbind(){
            glBindVertexArray(0);
        }
    };
}

//layout (std140) uniform XXX{ };
//ver4.2 layout(std140,binding=<0,1,2,...>) uniform XXX { };
namespace freeze {
    template<typename = void>
    struct void_uniform_buffer : any_buffer<GL_UNIFORM_BUFFER> {
        void block_binding(GLuint program,std::string const& name,GLuint index){
            auto block_index = glGetUniformBlockIndex(program,name.c_str());
            glUniformBlockBinding(program,block_index,index);
        }

        void bind_base(GLuint index){
            glBindBufferBase(GL_UNIFORM_BUFFER,index,this->ref());
        }

        void bind_range(GLuint index,GLintptr offset,GLsizeiptr size){
            glBindBufferRange(GL_UNIFORM_BUFFER,index,this->ref(),offset,size);
        }
    };
}

namespace freeze {
    template<typename = void>
    struct void_render_buffer : make_object<void_render_buffer<void>>{
        void create(GLuint* buffers){
            glGenRenderbuffers(1,buffers);
        }

        void destroy(GLuint const* buffers){
            glDeleteRenderbuffers(1,buffers);
        }

        void bind(){
            glBindRenderbuffer(GL_RENDERBUFFER,this->ref());
        }

        void unbind(){
            glBindRenderbuffer(GL_RENDERBUFFER,0);
        }

        void storage(GLsizei width,GLsizei height){
            glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8,width,height);
        }
    };
}

namespace freeze {
    //附加至少一个缓冲（颜色、深度或模板缓冲）。
    //至少有一个颜色附件(Attachment)。
    //所有的附件都必须是完整的（保留了内存）。
    //每个缓冲都应该有相同的样本数。
    //GL_FRAMEBUFFER
    //GL_DRAW_FRAMEBUFFER  (== GL_FRAMEBUFFER)
    //GL_READ_FRAMEBUFFER
    template<GLenum FrameBuffer>
    struct void_frame_buffer : make_object<void_frame_buffer<FrameBuffer>>{
        void create(GLuint* buffers){
            glGenFramebuffers(1,buffers);
        }

        void destroy(GLuint const* buffers){
            glDeleteFramebuffers(1,buffers);
        }

        void bind(){
            glBindFramebuffer(FrameBuffer,this->ref());
        }

        void unbind(){
            glBindFramebuffer(FrameBuffer,0);
        }

        // GL_FRAMEBUFFER_COMPLETE 0x8CD5==36053
        // GL_FRAMEBUFFER_UNDEFINED 0x8219
        // GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
        // GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
        // GL_FRAMEBUFFER_UNSUPPORTED
        // GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
        // GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
        GLenum check_status(){
            auto status = glCheckFramebufferStatus(FrameBuffer);
            return status;
        }

        bool is_complete(){
            auto status =  check_status();
            return status == GL_FRAMEBUFFER_COMPLETE;
        }

        template<typename Texture>
        void attachement_color(Texture const& texture,GLint level = 0){
            glFramebufferTexture(FrameBuffer,GL_COLOR_ATTACHMENT0,texture->ref(),level);
        }

        void attachement_texture2d(GLenum attachment,GLuint texture,GLuint level = 0){
            glFramebufferTexture2D(FrameBuffer,attachment,GL_TEXTURE_2D,texture,level);
        }

        void attachement_depth(GLuint texture){
            glFramebufferTexture2D(FrameBuffer,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,texture,0);
        }

        void attachement_stencil(GLuint texture){
            glFramebufferTexture2D(FrameBuffer,GL_STENCIL_ATTACHMENT,GL_TEXTURE_2D,texture,0);
        }

        // glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0,
        //               GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
        void attachement_depth_stencil(GLuint texture){
            glFramebufferTexture2D(FrameBuffer,GL_DEPTH_STENCIL_ATTACHMENT,GL_TEXTURE_2D,texture,0);
        }

        void attachement_render(GLuint renderbuffer){
            glFramebufferRenderbuffer(FrameBuffer,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,renderbuffer);
        }
    };
}


namespace freeze {

    using vertex_buffer = any_buffer<GL_ARRAY_BUFFER>;
    using element_buffer = any_buffer<GL_ELEMENT_ARRAY_BUFFER>;
    using vertex_array_buffer = void_vertex_array_buffer<>;
    using uniform_buffer = void_uniform_buffer<>;
    using render_buffer = void_render_buffer<>;
    using frame_buffer = void_frame_buffer<GL_FRAMEBUFFER>;
    using read_frame_buffer = void_frame_buffer<GL_READ_FRAMEBUFFER>;

    constexpr auto make_vertex_buffer = make<vertex_buffer>;
    constexpr auto make_element_buffer = make<element_buffer>;
    constexpr auto make_vertex_array_buffer = make<vertex_array_buffer>;
    constexpr auto make_uniform_buffer = make<uniform_buffer>;
    constexpr auto make_render_buffer = make<render_buffer>;
    constexpr auto make_frame_buffer = make<frame_buffer>;
    constexpr auto make_read_frame_buffer = make<read_frame_buffer>;
}


#endif //CDS_ANY_BUFFER_H
