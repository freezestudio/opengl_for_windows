//
// Created by ysb on 2017/10/26.
//

#ifndef VENDOR_DEPTH_H
#define VENDOR_DEPTH_H

namespace freeze {
    //
    // 深度缓冲，在每个片段中储存了信息，
    // 并且（通常）和颜色缓冲有着一样的宽度和高度。
    // 深度缓冲是由窗口系统自动创建的，
    // 它会以16、24或32位float的形式储存它的深度值。
    // 在大部分的系统中，深度缓冲的精度都是24位的。
    //
    // 当深度测试(Depth Testing)被启用的时候，
    // OpenGL会将一个片段的的深度值与深度缓冲的内容进行对比。
    // OpenGL会执行一个深度测试，
    // 如果这个测试通过了的话，深度缓冲将会更新为新的深度值。
    // 如果深度测试失败了，片段将会被丢弃。
    //
    // 深度缓冲是在片段着色器运行之后（以及模板测试(Stencil Testing)运行之后）在屏幕空间中运行的。
    // 屏幕空间坐标与通过OpenGL的glViewport所定义的视口密切相关，
    // 并且可以直接使用GLSL内建变量gl_FragCoord从片段着色器中直接访问。
    // gl_FragCoord的x和y分量代表了片段的屏幕空间坐标（其中(0, 0)位于左下角）。
    // gl_FragCoord中也包含了一个z分量，它包含了片段真正的深度值。
    // z值就是需要与深度缓冲内容所对比的那个值。
    //
    struct depth {
        static inline void enable(){
            glEnable(GL_DEPTH_TEST);
        }

        static inline void disable(){
            glDisable(GL_DEPTH_TEST);
        }

        static inline void clear(){
            glClear(GL_DEPTH_BUFFER_BIT);
        }

        static inline void test(GLenum func = GL_LESS){
            glDepthFunc(func);
        }

        static inline void test_default(){
            test();
        }

        static inline void test_always(){
            test(GL_ALWAYS);
        }

        static inline void test_never(){
            test(GL_NEVER);
        }

        static inline void test_less(){
            test(GL_LESS);
        }

        static inline void test_equal(){
            test(GL_EQUAL);
        }

        static inline void test_less_equal(){
            test(GL_LEQUAL);
        }

        static inline void test_greater(){
            test(GL_GREATER);
        }

        static inline void test_not_equal(){
            test(GL_NOTEQUAL);
        }

        static inline void test_greater_equal(){
            test(GL_GEQUAL);
        }

#if defined(ANDROID) || defined(__ANDROID__)
        static inline void ranger(GLfloat near = 0.0f,GLfloat far = 1.0f){
            glDepthRangef(near,far);
        }
#endif
        //static inline void range_array(GLuint first,GLsizei count,GLfloat const* v){
        //    glDepthRangeArrayv(first,count,v);
        //}

        static inline float linear_buffer(float z,float fnear,float ffar){
            return (z-fnear)/(ffar-fnear);
        }

        static inline float nonlinear_buffer(float z,float fnear,float ffar){
            return (1/z - 1/fnear)/(1/ffar-1/fnear);
        }

        static inline float nonlinear_to_linear(float linear,float fnear,float ffar){
            float z = linear *2.0f -1.0f; //back to NDC
            return (2.0f * fnear * ffar)/(ffar + fnear - z *(ffar - fnear));
        }
    };
}

#endif //VENDOR_DEPTH_H
