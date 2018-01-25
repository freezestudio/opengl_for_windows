//
// Created by freezestudio on 2017/10/26.
//

#ifndef FREEGL_DEPTH_H
#define FREEGL_DEPTH_H

namespace freeze
{
    //
    // ��Ȼ��壬��ÿ��Ƭ���д�������Ϣ��
    // ���ң�ͨ��������ɫ��������һ���Ŀ�Ⱥ͸߶ȡ�
    // ��Ȼ������ɴ���ϵͳ�Զ������ģ�
    // ������16��24��32λfloat����ʽ�����������ֵ��
    // �ڴ󲿷ֵ�ϵͳ�У���Ȼ���ľ��ȶ���24λ�ġ�
    //
    // ����Ȳ���(Depth Testing)�����õ�ʱ��
    // OpenGL�Ὣһ��Ƭ�εĵ����ֵ����Ȼ�������ݽ��жԱȡ�
    // OpenGL��ִ��һ����Ȳ��ԣ�
    // ����������ͨ���˵Ļ�����Ȼ��彫�����Ϊ�µ����ֵ��
    // �����Ȳ���ʧ���ˣ�Ƭ�ν��ᱻ������
    //
    // ��Ȼ�������Ƭ����ɫ������֮���Լ�ģ�����(Stencil Testing)����֮������Ļ�ռ������еġ�
    // ��Ļ�ռ�������ͨ��OpenGL��glViewport��������ӿ�������أ�
    // ���ҿ���ֱ��ʹ��GLSL�ڽ�����gl_FragCoord��Ƭ����ɫ����ֱ�ӷ��ʡ�
    // gl_FragCoord��x��y����������Ƭ�ε���Ļ�ռ����꣨����(0, 0)λ�����½ǣ���
    // gl_FragCoord��Ҳ������һ��z��������������Ƭ�����������ֵ��
    // zֵ������Ҫ����Ȼ����������Աȵ��Ǹ�ֵ��
    //
    // 1. enable()��disable()�����Ƿ�������Ȳ���
    // 2. glDepthFunc(f)���ƱȽϲ�����f�ǱȽϲ�������Ƭ�ε����ֵ���Ӧ��Ȼ����е�ֵ��f���бȽϡ�
    // 3. glDepthMask(b)�������ֵ��д��
    //
    struct depth
    {
        static inline void enable()
        {
            glEnable(GL_DEPTH_TEST);
        }

        static inline void disable()
        {
            glDisable(GL_DEPTH_TEST);
        }

        static inline void clear()
        {
            glClear(GL_DEPTH_BUFFER_BIT);
        }

        static inline void mask(GLboolean flag)
        {
            glDepthMask(flag);
        }

        static inline void read_only(bool ro = true)
        {
            mask(ro ? GL_FALSE : GL_TRUE);
        }

        static inline void test(GLenum func = GL_LESS)
        {
            glDepthFunc(func);
        }

        static inline void test_default()
        {
            test();
        }

        static inline void test_always()
        {
            test(GL_ALWAYS);
        }

        static inline void test_never()
        {
            test(GL_NEVER);
        }

        static inline void test_less()
        {
            test(GL_LESS);
        }

        static inline void test_equal()
        {
            test(GL_EQUAL);
        }

        static inline void test_less_equal()
        {
            test(GL_LEQUAL);
        }

        static inline void test_greater()
        {
            test(GL_GREATER);
        }

        static inline void test_not_equal()
        {
            test(GL_NOTEQUAL);
        }

        static inline void test_greater_equal()
        {
            test(GL_GEQUAL);
        }

#if defined(ANDROID) || defined(__ANDROID__)
        static inline void ranger(GLfloat near = 0.0f, GLfloat far = 1.0f)
        {
            glDepthRangef(near, far);
        }
#endif
        //static inline void range_array(GLuint first,GLsizei count,GLfloat const* v)
        //{
        //    glDepthRangeArrayv(first,count,v);
        //}

        static inline float linear_buffer(float z, float fnear, float ffar)
        {
            return (z - fnear) / (ffar - fnear);
        }

        static inline float nonlinear_buffer(float z, float fnear, float ffar)
        {
            return (1 / z - 1 / fnear) / (1 / ffar - 1 / fnear);
        }

        static inline float nonlinear_to_linear(float linear, float fnear, float ffar)
        {
            float z = linear * 2.0f - 1.0f; //back to NDC
            return (2.0f * fnear * ffar) / (ffar + fnear - z * (ffar - fnear));
        }
    };
}

#endif //FREEGL_DEPTH_H
