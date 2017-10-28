//
// Created by ysb on 2017/10/26.
//

#ifndef VENDOR_STENCIL_H
#define VENDOR_STENCIL_H

namespace freeze {
    struct stencil {
        static inline void enable(){
            glEnable(GL_STENCIL_TEST);
        }

        static inline void disable(){
            glDisable(GL_STENCIL_TEST);
        }

        static inline void clear(){
            glClear(GL_STENCIL_BUFFER_BIT);
        }

        //face -- GL_FRONT, GL_BACK, GL_FRONT_AND_BACK.
        static inline void mask_separate(GLenum face,GLuint mask){
            glStencilMaskSeparate(face,mask);
        }

        static inline void mask(GLuint sm = 0xFF){
            glStencilMask(sm);
        }

        static inline void mask_default(){
            mask();
        }

        static inline void mask_disable_write(){
            mask(0x00);
        }

        static inline void test_separate(GLenum face,GLenum func,GLint ref,GLuint mask){
            glStencilFuncSeparate(face,func,ref,mask);
        }

        //ref [0,2^n−1] n=指定的模板位数，通常指定8位
        static inline void test(GLenum func,GLint ref,GLuint mask){
            glStencilFunc(func,ref,mask);
        }

        static inline void test_default(){
            glStencilFunc(GL_ALWAYS,0,0xFF);
        }

        static inline void test_never(){
            test(GL_NEVER,1,0xFF);
        }

        //Passes if ( ref & mask ) < ( stencil & mask )
        static inline void test_less(){
            test(GL_LESS,1,0xFF);
        }

        //Passes if ( ref & mask ) <= ( stencil & mask )
        static inline void test_less_equal(){
            test(GL_LEQUAL,1,0xFF);
        }

        //Passes if ( ref & mask ) > ( stencil & mask )
        static inline void test_greater(){
            test(GL_GREATER,1,0xFF);
        }

        //Passes if ( ref & mask ) >= ( stencil & mask )
        static inline void test_greater_equal(){
            test(GL_GEQUAL,1,0xFF);
        }

        //Passes if ( ref & mask ) = ( stencil & mask )
        static inline void test_equal(){
            test(GL_EQUAL,1,0xFF);
        }

        //Passes if ( ref & mask ) != ( stencil & mask )
        static inline void test_not_equal(){
            test(GL_NOTEQUAL,1,0xFF);
        }

        static inline void test_always(){
            test(GL_ALWAYS,1,0xFF);
        }

        static inline void op_separate(GLenum face,GLenum sfail,GLenum dpfail,GLenum dppass){
            glStencilFuncSeparate(face,sfail,dpfail,dppass);
        }

        //sfail,dpfail,dppass  -- GL_KEEP, GL_ZERO, GL_REPLACE, GL_INCR, GL_INCR_WRAP, GL_DECR, GL_DECR_WRAP, GL_INVERT
        // +--------------+----------------------------------------------------------+
        // |   行为       |                      描述                                |
        // +--------------+----------------------------------------------------------+
        // | GL_KEEP	  |  保持当前储存的模板值                                    |
        // +--------------+----------------------------------------------------------+
        // | GL_ZERO	  |  将模板值设置为0                                         |
        // +--------------+----------------------------------------------------------+
        // | GL_REPLACE	  |  将模板值设置为glStencilFunc函数设置的ref值              |
        // +--------------+----------------------------------------------------------+
        // | GL_INCR	  |  如果模板值小于最大值则将模板值加1                       |
        // +--------------+----------------------------------------------------------+
        // | GL_INCR_WRAP |	 与GL_INCR一样，但如果模板值超过了最大值则归零           |
        // +--------------+----------------------------------------------------------+
        // | GL_DECR	  |  如果模板值大于最小值则将模板值减1                       |
        // +--------------+----------------------------------------------------------+
        // | GL_DECR_WRAP |	 与GL_DECR一样，但如果模板值小于0则将其设置为最大值      |
        // +--------------+----------------------------------------------------------+
        // | GL_INVERT	  |  按位翻转当前的模板缓冲值                                |
        // +--------------+----------------------------------------------------------+
        static inline void op(GLenum sfail = GL_KEEP,GLenum dpfail = GL_KEEP,GLenum dppass = GL_KEEP){
            glStencilOp(sfail,dpfail,dppass);
        }
    };
}
#endif //VENDOR_STENCIL_H
