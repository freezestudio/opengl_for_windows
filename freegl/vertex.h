//
// Created by freezestudio on 2017/10/11.
//

#ifndef FREEGL_VERTEX_H
#define FREEGL_VERTEX_H

namespace freeze
{
    template<typename = void>
    struct vertex_t
    {
        // index   顶点属性（位置值）
        // size    顶点大小 (表示n维的向量)
        // type    数据类型(enum)
        // stride  步长，连续顶点属性组之间的间隔
        // pointer 位置数据在缓冲中起始位置的偏移量
        void set(GLuint index, GLint size, GLenum type, GLsizei stride, GLint pointer)
        {
            internal_set(index, size, type, stride, pointer);
            glEnableVertexAttribArray(index);
            assert_error();
        }

        void set(GLuint index, GLint size, GLsizei stride, GLint pointer)
        {
            set(index, size, GL_FLOAT, stride * sizeof(GLfloat), pointer * sizeof(GLfloat));
        }

        // div -- 0表示每顶点，n表示希望每n个实例更新一次属性
        void divisor(GLuint index, GLuint div)
        {
            glVertexAttribDivisor(index,div);
			assert_error();
        }
    private:
        void internal_set(GLuint index,
            GLint size, GLenum type, GLsizei stride, GLint pointer)
        {
            glVertexAttribPointer(index, size, type, GL_FALSE, stride,
                reinterpret_cast<const void*>(pointer));
            assert_error();
        }
    };
}

namespace freeze
{
    using vertex = vertex_t<>;
    constexpr auto make_vertex = make<vertex>;
}

#endif //FREEGL_VERTEX_H