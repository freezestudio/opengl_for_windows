//
// Created by freezestudio on 2017/10/26.
//

#ifndef FREEGL_STENCIL_H
#define FREEGL_STENCIL_H

namespace freeze
{
    //
    // ģ����� -- Ƭ����ɫ��֮����Ȳ���֮ǰ
    // ʹ�÷�ʽ��
    //    ����ģ�建���д�롣
    //    ��Ⱦ���壬����ģ�建������ݡ�
    //    ����ģ�建���д�롣
    //    ��Ⱦ�����������壬��θ���ģ�建������ݶ����ض���Ƭ�Ρ�
    //�ؼ��㣺
    //     1.ģ�建���д�����
    //        enable()��disable()�����Ƿ�����ģ����Թ���
    //        glStencilMask(...)���ƴ�д���ֵ��ͨ����Ϊ0xff����ԭ��д�룬��Ϊ0x00�����ֹд�롣
    //        glStencilOp(s,d,z)����ʲô�����д��
    //     2.����ͨ������
    //        glStencilFunc(f,r,m)���Ʋ����Ƿ�ͨ����
    //        f��ʾ�Ƚϲ�������r��ֵ���Ӧ�����е�ֵ(o)���бȽϣ�m����r��o�Ƚ�ǰ�ֱ���m�Ƚ���'��'����
    //
    struct stencil
    {
        static inline void enable()
        {
            glEnable(GL_STENCIL_TEST);
        }

        static inline void disable()
        {
            glDisable(GL_STENCIL_TEST);
        }

        static inline void clear()
        {
            glClear(GL_STENCIL_BUFFER_BIT);
        }

        //face -- GL_FRONT, GL_BACK, GL_FRONT_AND_BACK.
        static inline void mask_separate(GLenum face, GLuint mask)
        {
            glStencilMaskSeparate(face, mask);
        }

        static inline void mask(GLuint sm = 0xFF)
        {
            glStencilMask(sm);
        }

        static inline void mask_default()
        {
            mask();
        }

        static inline void mask_enable_write()
        {
            mask_default();
        }

        static inline void mask_disable_write()
        {
            mask(0x00);
        }

        static inline void test_separate(GLenum face, GLenum func, GLint ref, GLuint mask)
        {
            glStencilFuncSeparate(face, func, ref, mask);
        }

        //ref [0,2^n?1] n=ָ����ģ��λ����ͨ��ָ��8λ
        static inline void test(GLenum func, GLint ref, GLuint mask)
        {
            glStencilFunc(func, ref, mask);
        }

        static inline void test_default()
        {
            test(GL_ALWAYS, 0, 0xFF);
        }

        static inline void test_always()
        {
            test(GL_ALWAYS, 1, 0xFF);
        }

        static inline void test_never()
        {
            test(GL_NEVER, 1, 0xFF);
        }

        //Passes if ( ref & mask ) < ( stencil & mask )
        static inline void test_less()
        {
            test(GL_LESS, 1, 0xFF);
        }

        //Passes if ( ref & mask ) <= ( stencil & mask )
        static inline void test_less_equal()
        {
            test(GL_LEQUAL, 1, 0xFF);
        }

        //Passes if ( ref & mask ) > ( stencil & mask )
        static inline void test_greater()
        {
            test(GL_GREATER, 1, 0xFF);
        }

        //Passes if ( ref & mask ) >= ( stencil & mask )
        static inline void test_greater_equal()
        {
            test(GL_GEQUAL, 1, 0xFF);
        }

        //Passes if ( ref & mask ) = ( stencil & mask )
        static inline void test_equal()
        {
            test(GL_EQUAL, 1, 0xFF);
        }

        //Passes if ( ref & mask ) != ( stencil & mask )
        static inline void test_not_equal()
        {
            test(GL_NOTEQUAL, 1, 0xFF);
        }

        static inline void op_separate(GLenum face, 
            GLenum sfail, GLenum dpfail, GLenum dppass)
        {
            glStencilFuncSeparate(face, sfail, dpfail, dppass);
        }

        //sfail,dpfail,dppass  
        //    -- GL_KEEP, 
        //       GL_ZERO, 
        //       GL_REPLACE, 
        //       GL_INCR, 
        //       GL_INCR_WRAP, 
        //       GL_DECR, 
        //       GL_DECR_WRAP, 
        //       GL_INVERT
        // +--------------+----------------------------------------------------------+
        // |   ��Ϊ       |                      ����                                 |
        // +--------------+----------------------------------------------------------+
        // | GL_KEEP	  |  ���ֵ�ǰ�����ģ��ֵ                                        |
        // +--------------+----------------------------------------------------------+
        // | GL_ZERO	  |  ��ģ��ֵ����Ϊ0                                            |
        // +--------------+----------------------------------------------------------+
        // | GL_REPLACE	  |  ��ģ��ֵ����ΪglStencilFunc�������õ�refֵ                   |
        // +--------------+----------------------------------------------------------+
        // | GL_INCR	  |  ���ģ��ֵС�����ֵ��ģ��ֵ��1                             |
        // +--------------+----------------------------------------------------------+
        // | GL_INCR_WRAP |	 ��GL_INCRһ���������ģ��ֵ���������ֵ�����                  |
        // +--------------+----------------------------------------------------------+
        // | GL_DECR	  |  ���ģ��ֵ������Сֵ��ģ��ֵ��1                             |
        // +--------------+----------------------------------------------------------+
        // | GL_DECR_WRAP |	 ��GL_DECRһ���������ģ��ֵС��0��������Ϊ���ֵ              |
        // +--------------+----------------------------------------------------------+
        // | GL_INVERT	  |  ��λ��ת��ǰ��ģ�建��ֵ                                     |
        // +--------------+----------------------------------------------------------+
        static inline void op(GLenum sfail = GL_KEEP, 
            GLenum dpfail = GL_KEEP, GLenum dppass = GL_KEEP)
        {
            glStencilOp(sfail, dpfail, dppass);
        }
    };
}
#endif //FREEGL_STENCIL_H
