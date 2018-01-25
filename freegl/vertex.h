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
		// index   �������ԣ�λ��ֵ��
		// size    �����С (��ʾnά������) 1,2,3,4,GL_BGRA
		// type    ��������(enum)
		// stride  ��������������������֮��ļ��
		// pointer λ�������ڻ�������ʼλ�õ�ƫ����
		static void set(GLuint index, GLint size, GLenum type, 
            GLboolean normalized, GLsizei stride, GLint pointer)
		{
			glVertexAttribPointer(index, size, type, normalized, stride,
				reinterpret_cast<GLvoid const*>(pointer));
			assert_error();
		}

		static void set(GLuint index, GLint size, GLsizei stride, GLint pointer)
		{
			set(index, size, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), 
                pointer * sizeof(GLfloat));
		}

		static void set_enable(GLuint index, GLint size, GLenum type, 
            GLboolean normalized, GLsizei stride, GLint pointer)
		{
			set(index, size, type, normalized, stride,pointer);
			enable(index);
		}

		static void set_enable(GLuint index, GLint size, GLsizei stride, GLint pointer)
		{
			set_enable(index, size,GL_FLOAT,GL_FALSE, stride * sizeof(GLfloat), 
                pointer * sizeof(GLfloat));
		}

		// div -- 0��ʾÿ���㣬n��ʾϣ��ÿn��ʵ������һ������
		static void divisor(GLuint index, GLuint div)
		{
			glVertexAttribDivisor(index, div);
			assert_error();
		}

		static void enable(GLuint index)
		{
			glEnableVertexAttribArray(index);
			assert_error();
		}

		static void disable(GLuint index)
		{
			glDisableVertexAttribArray(index);
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
