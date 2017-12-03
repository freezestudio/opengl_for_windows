# freegl文档

## 概述

不同于我们日常使用文字（文本）来表示对象的名字，openGL使用无符号整数表示对象名。
因此对于openGL对象，如果它大于0，则表示某个对象存在。

openGL常用的对象有顶点数组对象(VAO)、顶点缓存对象(VBO)、索引缓存对象(EBO)、全局统一变量缓冲对象(UBO)、
缓冲区对象(FBO),渲染缓冲区对象(RBO)、纹理对象(Texture)、着色器(Shader)及着色器程序(Program)对象等。
它们的创建与使用是类似的。都是从openGL内部返回表示某对象的无符号整数值。使用openGL提供的接口来间接操作这些对象。

我们以常用的VAO和VBO为例，来观察它们的操作。
生成一个或一系列VBO的操作如下：

//一个
GLuint vao;
glGenBuffers(1,&vao);

//多个
GLuint vao[n];
glGenBuffers(n,vao);

使用它们时，需要先将它们绑定为当前对象。操作完成后，解绑该对象，或者绑定另一个。

//指定绑定对象的类型，和对象名。（对象名字就是创建对象后返回的数值）
glBindBuffer(GL_ARRAY_BUFFER,vao);
//针对此对象的一些操作
// ...
//绑定到一个无名字的对象，也就是解除了之前对象的绑定。
glBindBuffer(GL_ARRAY_BUFFER,0);