# freegl文档

## 概述

不同于我们日常使用文字（文本）来表示对象的名字，`OpenGL`使用无符号整数表示对象名。
因此对于`OpenGL`对象，如果它大于`0`，则表示某个对象存在。（某些对象比如`FBO`、`TFO`等，`0`表示默认对象）

`OpenGL`常用的对象有顶点数组对象(`VAO`)、顶点缓存对象(`VBO`)、索引缓存对象(`EBO`)、全局统一变量缓冲对象(`UBO`)、
缓冲区对象(`FBO`),渲染缓冲区对象(`RBO`)、TransformFeedback(`TFO`)、纹理对象(`Texture`)、着色器(`Shader`)及着色器程序(`Program`)对象等。
它们的创建与使用是类似的。都是从`OpenGL`内部返回表示某对象的无符号整数值。使用`OpenGL`提供的接口来间接操作这些对象。

我们以常用的`VAO`和`VBO`为例，来观察它们的操作。
生成一个或一系列`VBO`的操作如下：

```c++
//一个
GLuint vao;
glGenBuffers(1,&vao);

//多个
GLuint vao[n];
glGenBuffers(n,vao);
```

使用它们时，需要先将它们绑定为当前对象。操作完成后，(可选地)解绑该对象，或者绑定另一个。

```c++
//指定绑定对象的类型，和对象名。（对象名字就是创建对象后返回的数值）
glBindBuffer(GL_ARRAY_BUFFER,vao);
//针对此对象的一些操作
// ...
//绑定到一个无名字的对象，也就是解除了之前对象的绑定。
glBindBuffer(GL_ARRAY_BUFFER,0);
```

因此，我们需要将这些值封装起来，让它们看起来像是真正的对象，而不是无意义的整数值。

```c++
struct opengl_object
{
    GLuint object_id;
};
```

考察上面这个结构，它还不能完整地表达`OpenGL`对象的信息。我们需要

1. 不同类型之间做出区分
1. 同类型之间不同对象的区分

应用C++模板类，恰好可以完成以上任务。

```c++
template<typename T>
struct opengl_object
{
    GLuint object_id;
};
```
