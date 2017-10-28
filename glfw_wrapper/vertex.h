//
// Created by ysb on 2017/10/11.
//

#ifndef CDS_VERTEX_H
#define CDS_VERTEX_H

namespace freeze {
    template<typename = void>
    struct void_vertex{
        // index   顶点属性（位置值）
        // size    顶点大小 (表示n维的向量)
        // type    数据类型(enum)
        // stride  步长，连续顶点属性组之间的间隔
        // pointer 位置数据在缓冲中起始位置的偏移量
        void set(GLuint index, GLint size, GLenum type, GLsizei stride, GLint pointer)
        {
            internal_set(index, size, type, stride, pointer);
            glEnableVertexAttribArray(index);
        }

//        template<typename ArrayBuffer>
//        void set(std::shared_ptr<ArrayBuffer> vao,
//                 GLuint index, GLint size, GLenum type, GLsizei stride, GLint pointer)
//        {
//            internal_set(index, size, type, stride, pointer);
//            glEnableVertexArrayAttrib(vao->ref(), index);
//        }
    private:
        void internal_set(GLuint index,
                          GLint size, GLenum type, GLsizei stride, GLint pointer)
        {
            glVertexAttribPointer(index,
                                  size, type, GL_FALSE, stride,
                                  reinterpret_cast<const void*>(pointer));
            auto err = glGetError();
            if(err != GL_NO_ERROR){
                LOGE("glVertexAttribPointer error : %d",err);
            }
        }
    };
}

namespace freeze {
    using vertex = void_vertex<>;
    constexpr auto make_vertex = make<vertex>;
}

#endif //CDS_VERTEX_H
