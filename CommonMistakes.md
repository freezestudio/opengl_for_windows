# 常见问题

## 面向对象语言问题

像C++那样的面向对象语言，常常将状态封装到对象中。但是OpenGL的状态是全局的。当将状态封装到对象中时，会产生一些问题。例如，可能有一个纹理对象具有如下的构造函数和析构函数：

```c++
MyTexture::MyTexture(const char *pfilePath)
{
  if(LoadFile(pfilePath)==ERROR)
	 return;
  textureID=0;
  glGenTextures(1, &textureID);
  //More GL code...
}

MyTexture::~MyTexture()
{
  if(textureID)
	 glDeleteTextures(1, &textureID);
}
```

这样做有一个问题。除非创建了`OpenGL`上下文，并且在该线程中处于活动状态，否则`OpenGL`函数将不起作用。因此，在上下文创建之前`glGenTextures`将无法正常工作，并且在上下文销毁之后`glDeleteTextures`将无法正常工作。

当用户在全局范围内创建一个纹理对象或类似的`OpenGL`对象包装时，这个问题通常会以构造函数的形式出现。有几个可能的解决方案：

1. 不要使用构造函数/析构函数来初始化/销毁`OpenGL`对象。相反，为了这些目的，使用这些类的成员函数。这违反了`RAII`的原则，所以这不是最好的行动。
1. 如果尚未创建上下文，请`让OpenGL`对象构造函数引发异常。这就需要在上下文创建功能中添加一个新的功能，该功能会在创建并激活上下文时告诉您的代码。
1. 创建一个拥有所有其他`OpenGL`相关对象的类。这个类还应该负责在其构造函数中创建上下文。
1. 如果在上下文不是最新的情况下创建/销毁对象，则允许程序崩溃。这使用户有责任正确使用它们，但是这也使得他们的工作代码看起来更加自然。

### RAII和隐藏的析构函数调用

`C++`的`RAII`原则说，如果一个对象封装了一个资源（比如一`OpenGL`对象），构造函数应该创建资源，而析构函数应该销毁它。 这看起来不错：

```c++
MyTexture::MyTexture(const char *pfilePath)
{
  if(LoadFile(pfilePath)==ERROR)
	 return;
  textureID=0;
  glGenTextures(1, &textureID);
  //More GL code...
}

MyTexture::~MyTexture()
{
  if(textureID)
	 glDeleteTextures(1, &textureID);
}
```

这个问题发生在你想要传递这个对象到其它地方的时候，或者是在像`vector`这样的`C++`容器中创建它的时候。 考虑这个功能：

```c++
MyTexture CreateTexture()
{
  MyTexture tex;

  //Initialize `tex` with data.

  return tex;
}
```

这里发生了什么？按照`C++`的规则，在这个函数调用结束时，`tex`将被销毁。返回的不是`tex`本身，而是此对象的副本。但是`tex`管理一个资源：一个`OpenGL`对象。而这个资源将会被析构函数破坏。

返回的副本将因此具有已被销毁的`OpenGL`对象名称。

发生这种情况是因为我们违反了`C++`的`3/5`原则：如果你为一个类写了析构函数，复制/移动构造函数，或者复制/移动赋值操作符，那么你必须写所有这些。

编译器生成的拷贝构造函数是错误的;它复制`OpenGL`对象名称，而不是`OpenGL`对象本身。这留下了两个`C++`对象，每个`C++`对象都打算销毁相同的OpenGL对象。

理想情况下，复制`RAII`包装应该将`OpenGL`对象的数据复制到新的`OpenGL`对象中。这将使每个`C++`对象具有自己独特的`OpenGL`对象。但是，将`OpenGL`对象的数据复制到一个新的对象是非常昂贵的;这也是基本上不可能做到的，这要归功于扩展的能力来添加你可能不会静态知道的状态。

所以相反，我们应该禁止复制`OpenGL`包装对象。这种类型应该是仅移动类型;在移动中，我们从移动的对象中窃取资源。

```c++
class MyTexture
{
private:
  GLuint obj_ = 0; //Cannot leave this uninitialized.

  void Release()
  {
    glDeleteTextures(1, &obj_);
    obj_ = 0;
  }

public:
  //Other constructors as normal.

  //Free up the texture.
  ~MyTexture() {Release();}

  //Delete the copy constructor/assignment.
  MyTexture(const MyTexture &) = delete;
  MyTexture &operator=(const MyTexture &) = delete;

  MyTexture(MyTexture &&other) : obj_(other.obj_)
  {
    other.obj_ = 0; //Use the "null" texture for the old object.
  }

  MyTexture &operator=(MyTexture &&other)
  {
    //ALWAYS check for self-assignment.
    if(this != &other)
    {
      Release();
      //obj_ is now 0.
      std::swap(obj_, other.obj_);
    }
  }
};
```

现在，上面的代码可以工作。 返回`tex`; 将会引起`tex`的移动，在`tex`被破坏之前，`tex`将被视为`0`。 用`0`纹理调用`glDeleteTextures`是被允许的。

### OOP和隐藏的绑定

在使用`OpenGL`和`c++`等语言时还有另外一个问题。 考虑以下功能：

```c++
void MyTexture::TexParameter(GLenum pname, GLint param)
{
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, pname, param);
}
```

问题是纹理的绑定对类的用户是隐藏的。 对重复绑定对象可能会有性能影响（特别是因为`API`对外部用户而言似乎不重要）。 但主要关心的是正确性; 绑定的对象是全局状态，现在一个本地成员函数已经改变了。

这可能会导致许多隐藏的破损来源。 实现这一目标的安全方法如下：

```c++
void MyTexture::TexParameter(GLenum pname, GLint param)
{
    GLuint boundTexture = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*) &boundTexture);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, pname, param);
    glBindTexture(GL_TEXTURE_2D, boundTexture);
}
```

请注意，此解决方案强调性能的正确性; `glGetIntegerv`调用可能不会特别快。

更有效的解决方案是使用直接状态访问，它需要`OpenGL 4.5`或`ARB_direct_state_access`或更早的`EXT_direct_state_access`扩展：

```c++
void MyTexture::TexParameter(GLenum pname, GLint param)
{
    glTextureParameteri(textureID, GL_TEXTURE_2D, pname, param);
}
```

## 纹理上传和像素读取

您可以为纹理创建存储，并使用`glTexImage2D`（或类似的功能，根据纹理的类型）上传像素。如果程序在上载期间崩溃，或对角线出现在所得图像中，这是因为你的像素阵列的每一水平线的取向不是`4`这通常发生于用户加载图像是将`RGB`的或多个`BGR`格式（例如，`24 BPP`图像），这取决于图像数据的来源。

例如，你的图像`宽度 = 401` `高度 = 500`.高度是无关的;重要的是宽度。我们算一下，`401像素×3字节= 1203`，这是不能被`4`整除。一些图像文件格式可能固有地将每行对齐到`4`个字节，但有些不。对于那些不行，每行将从最后一个开始正好开始`1203`个字节。可以更改`OpenGL`的行对齐，以适合图像数据的行对齐。这是通过调用`glPixelStorei(GL_UNPACK_ALIGNMENT,#)`完成的，其中`#`是您想要的对齐方式。默认对齐方式是`4`。

如果你有兴趣，大多数`GPU`像`4`个字节的块。换句话说，当每个组件是一个字节时，`GL_RGBA`或`GL_BGRA`是首选。 `GL_RGB`和`GL_BGR`被认为是奇怪的，因为大多数`GPU`，大多数`CPU`和任何其他类型的芯片不处理`24`位。这意味着，驱动程序将您的`GL_RGB`或`GL_BGR`转换为`GPU`所喜欢的，通常是`BGRA`。

同样，如果你用`glReadPixels`读取一个缓冲区，你可能会遇到类似的问题。有一个`GL_PACK_ALIGNMENT`就像`GL_UNPACK_ALIGNMENT`一样。默认对齐方式也是`4`，这意味着每个水平线必须是`4`的倍数。如果你阅读的格式，如`GL_BGRA`或`GL_RGBA`您缓冲区将不会有任何问题，因为该线将永远是`4`的倍数，如果在格式，如`GL_BGR`或`GL_RGB`读它，那么你的风险运行到这个问题。

`GL_PACK / UNPACK_ALIGNMENTs`只能是`1,2,4或8`.所以`3`的对齐方式是不允许的。

## 图像精度

您可以（但不建议这样做）调用`glTexImage2D(GL_TEXTURE_2D, 0, X, width, height, 0, format, type, pixels)`，并将`X`设置为`1，2，3或4`。到组件的数量（`GL_RED`将是`1`，`GL_RG`将是`2`，`GL_RGB`将是`3`，`GL_RGBA`将是`4`）。

实际上最好是给出一个真实的图像格式，一个具有特定的内部精度。如果`OpenGL`实现不支持您选择的特定格式和精度，那么驱动程序将在内部将其转换成它支持的东西。

`OpenGL`版本`3.x`及以上版本有一套所需的图像格式，所有符合的实现必须执行。

注意：不可变存储纹理的创建主动禁止使用未经处理的图像格式。或如上所述的整数。
我们也应该说在教程网站上看到以下内容是很常见的：

```c++
glTexImage2D(GL_TEXTURE_2D，0，GL_RGB，width，height，0，GL_RGB，GL_UNSIGNED_BYTE，pixels);
```

虽然`GL`会接受`GL_RGB`，但由驱动来决定适当的精度。我们建议您具体写成`GL_RGB8`：

```c++
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
```

这意味着您需要驱动程序实际将其存储在`R8G8B8`格式中。我们还应该说，大多数`GPU`将`GL_RGB8`在内部转换成`GL_RGBA8`。所以最好避开`GL_RGB8`。我们还应该说明，在一些平台上，比如`Windows`，像素上传格式`GL_BGRA`是首选。

```c++
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
```

这使用`GL_RGBA8`作为内部格式。 `GL_BGRA`和`GL_UNSIGNED_BYTE`（或`GL_UNSIGNED_INT_8_8_8_8`是以像素数组为单位的数据，驱动程序可能不需要执行任何基于`CPU`的转换，并直接将`DMA`数据直接传输到显卡。基准测试显示，在`Windows`和`nVidia`和`ATI/AMD`，这是最佳的格式。

首选的像素传输格式和类型可以从实现中查询。

## 深度缓冲精度

当为窗口选择像素格式，并要求深度缓冲区时，深度缓冲区通常以`16`位，`24`位或`32`位的位深度存储为标准化整数。

注意：您可以使用真正的浮点深度格式创建图像。但这些只能用于帧缓冲区对象，而不能用于默认帧缓冲区。
在`OpenGL`中，所有的深度值都在`[0，1]`的范围内。整数规范化过程只是将这个浮点范围转换为适当精度的整数值。它是存储在深度缓冲区中的整数值。

通常，`24`位深度缓冲区会将每个深度值填充到`32`位，因此每像素`8`位将不会使用。但是，如果要求`8`位模板缓冲区以及深度缓冲区，则两个单独的图像通常会合并为一个深度/模板图像。 `24`位将用于深度，其余`8`位用于模板。

现在深度缓冲浮点的误解已经解决了，这个调用有什么问题呢？

```c++
glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, mypixels);
```

由于深度格式是标准化的整数格式，因此驱动程序必须使用`CPU`将标准化的整数数据转换为浮点值。 这很慢。

处理这个问题的首选方法是用这个代码：

```c++
if(depth_buffer_precision == 16)
  {
    GLushort mypixels[width*height];
    glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, mypixels);
  }
  else if(depth_buffer_precision == 24)
  {
    GLuint mypixels[width*height];    //There is no 24 bit variable, so we'll have to settle for 32 bit
    glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT_24_8, mypixels);  //No upconversion.
  }
  else if(depth_buffer_precision == 32)
  {
    GLuint mypixels[width*height];
    glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, mypixels);
  }
```

如果您有深度/模板格式，则可以通过这种方式获取深度/模板数据：

```c++
GLuint mypixels[width*height];
glReadPixels(0, 0, width, height, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, mypixels);
```

## 创建一个完整的纹理

这个代码有什么问题？

```c++
glGenTextures(1, &textureID);
glBindTexture(GL_TEXTURE_2D, textureID);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
```

纹理将不起作用，因为它是不完整的。 默认的`GL_TEXTURE_MIN_FILTER`状态是`GL_NEAREST_MIPMAP_LINEAR`。 而且因为`OpenGL`定义默认的`GL_TEXTURE_MAX_LEVEL`为`1000`，所以`OpenGL`会期望定义`mipmap`级别。 由于您只定义了一个`mipmap`级别，所以`OpenGL`会考虑纹理不完整，直到`GL_TEXTURE_MAX_LEVEL`设置正确，或者`GL_TEXTURE_MIN_FILTER`参数设置为不使用`mipmap`。

更好的代码是使用纹理存储功能（如果你有`OpenGL 4.2`或`ARB_texture_storage`）分配纹理的存储，然后用`glTexSubImage2D`上传：

```c++
glGenTextures(1, &textureID);
glBindTexture(GL_TEXTURE_2D, textureID);
glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
glTexSubImage2D(GL_TEXTURE_2D, 0​, 0, 0, width​, height​, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
```

这将创建具有单个`mipmap`级别的纹理，并适当地设置所有参数。 如果你想有多个`mipmap`，那么你应该把`1`改成你想要的`mipmap`。 您还需要单独的`glTexSubImage2D`调用来上传每个`mipmap`。

如果不可用，你可以从这个代码中得到类似的效果：

```c++
glGenTextures(1, &textureID);
glBindTexture(GL_TEXTURE_2D, textureID);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
```

同样，如果你使用多个`mipmap`，你应该改变`GL_TEXTURE_MAX_LEVEL`来表示你将使用多少（`-1`,因为`base/max`级别是一个闭区间）。然后执行一个`glTexImage2D`（注意没有“Sub”）。 为每个`mipmap`。

## 自动生成`mipmap`

可以使用`glGenerateMipmap`函数自动生成纹理的`Mipmap`。 此函数（或扩展`GL_ARB_framebuffer_object`）需要`OpenGL 3.0`或更高版本。 该功能很简单， 当你为纹理调用它时，会为该纹理生成`mipmap`：

```c++
glGenTextures(1, &textureID);
glBindTexture(GL_TEXTURE_2D, textureID);
glTexStorage2D(GL_TEXTURE_2D, num_mipmaps, GL_RGBA8, width, height);
glTexSubImage2D(GL_TEXTURE_2D, 0​, 0, 0, width​, height​, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
glGenerateMipmap(GL_TEXTURE_2D);  //Generate num_mipmaps number of mipmaps here.
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
```

如果纹理存储不可用，则可以使用较旧的`API`：

```c++
glGenTextures(1, &textureID);
glBindTexture(GL_TEXTURE_2D, textureID);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
```

警告：据报道，在某些`ATI`驱动程序中，`glGenerateMipmap(GL_TEXTURE_2D)`除非在此特定情况下调用`glEnable(GL_TEXTURE_2D)`，否则无效。 再次，要清楚，绑定纹理，`glEnable`，然后`glGenerateMipmap`。 这是一个一直在`ATI`驱动程序中的`bug`。 也许现在已经修复了。 （`glGenerateMipmap`在`2011`年不适用于`ATI`）

## glGetError

为什么要检查错误？ 为什么你应该调用`glGetError()`?

```c++
glGenTextures(1, &textureID);
glBindTexture(GL_TEXTURE_2D, textureID);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);   //Requires GL 1.4. Removed from GL 3.1 and above.
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
```

该代码没有调用`glGetError()`。 如果你要调用`glGetError`，它会返回`GL_INVALID_ENUM`。 
如果在每个函数调用之后放置一个`glGetError`调用，
则会注意到在 `glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR)`上引发了错误。 
`magnification filter`不能指定使用`mipmap`; 只有`minification filter`可以做到这一点。

## 在编译着色器时检查错误

在编译/链接着色器或程序对象时总是检查错误。

## 创建Cubemap纹理

最好将过滤模式设置为`GL_CLAMP_TO_EDGE`，而不是其他格式。 别忘了定义全部`6`个面，否则纹理被认为是不完整的。 不要忘记设置`GL_TEXTURE_WRAP_R`，因为立方体贴图需要`3D`纹理坐标。

```c++
glGenTextures(1, &textureID);
glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0); 
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0); 
//Define all 6 faces
glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels_face0);
glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels_face1);
glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels_face2);
glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels_face3);
glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels_face4);
glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels_face5);
```

当使用`glTexStorage2D`代替`glTexImage2D`时，应该使用目标`GL_TEXTURE_CUBE_MAP`调用`glTexStorage2D`一次，然后调用`glTexSubImage2D`来上传每个面的数据。

如果要自动生成`mipmap`，可以使用上述任何一种机制，使用目标`GL_TEXTURE_CUBE_MAP`。 生成立方体贴图的`mipmap`时，`OpenGL`将不会混合使用多个纹理，而在较低的`mip`级别留下可见的接缝。 除非启用无缝的立方体贴图纹理。

## 更新纹理

要在现有的`2D`纹理中更改纹理元素，请使用`glTexSubImage2D`：

```c++
glBindTexture(GL_TEXTURE_2D, textureID);    //A texture you have already created storage for
glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
```

`glTexImage2D`创建纹理的存储，定义大小/格式，并删除所有以前的像素数据。 `glTexSubImage2D`只修改纹理内的像素数据。 它可以用来更新所有的纹素，或者只是其中的一部分。

要从帧缓冲区中复制纹理元素，请使用`glCopyTexSubImage2D`。

```c
glBindTexture（GL_TEXTURE_2D，textureID）; //你已经为创建了存储的纹理
glCopyTexSubImage2D（GL_TEXTURE_2D，0，0，0，0，width，height）; //将当前读缓冲区复制到纹理
```

请注意，有一个`glCopyTexImage2D`函数，该函数执行复制以填充图像，但也定义了图像大小，格式等等，就像`glTexImage2D`一样。

## 渲染到纹理

要直接渲染到纹理，而不是像上面那样进行复制，请使用帧缓冲区对象。

> **警告**：`NVIDIA`的`OpenGL`驱动程序使用不完整的纹理有一个已知的问题。如果纹理不是纹理完整的，则`FBO`本身将被视为`GL_FRAMEBUFFER_UNSUPPORTED`，或者将具有`GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT`。这是一个驱动程序错误，因为`OpenGL`规范不允许实现仅仅因为纹理尚未完成而返回这些值中的任何一个。在`NVIDIA`驱动程序中解决这个问题之前，建议确保所有纹理都具有`mipmap`级别，并且所有的`glTexParameteri`值都是针对纹理格式设置的。例如，如果`mag`和`min`过滤器有任何`LINEAR`字段，则整数值纹理(`integral textures`)不完整。

## 深度测试不起作用

首先，检查深度测试是否激活。确保`glEnable`已被调用并且相应的`glDepthFunc`处于活动状态。还要确保`glDepthRange`与深度函数匹配。

假设所有的设置都正确，你的帧缓冲区可能根本就没有深度缓冲区。对于您创建的`Framebuffer`对象很容易看到。对于默认帧缓冲区，这完全取决于你如何创建你的`OpenGL`上下文。

例如，如果您正在使用`GLUT`，则需要确保将`GLUT_DEPTH`传递给`glutInitDisplayMode`函数。

## 帧缓冲区中没有Alpha

如果你正在做`Blending`，你需要一个目标`alpha`，你需要确保你的渲染目标有一个。渲染到`Framebuffer`对象时很容易确保。但是使用默认帧缓冲区，这取决于你如何创建你的`OpenGL`上下文。

例如，如果您正在使用`GLUT`，则需要确保将`GLUT_ALPHA`传递给`glutInitDisplayMode`函数。

## glFinish和glFlush

如果您正在渲染到默认帧缓冲区的前端缓冲区，请使用`glFlush`。 有一个双缓冲窗口更好，但如果你有一个情况，你想直接渲染到窗口，然后继续。

有很多教程网站建议你这样做：

```c++
glFlush();
SwapBuffers();
```

这是不必要的。 `SwapBuffer`命令负责`flushing`和命令处理。

`glFlush`和`glFinish`函数用于将`CPU`操作与`GPU`命令同步。

在许多情况下，这样的显式同步是不必要的。` Sync Objects`的使用可以使其成为必要，也可以使用`arbitrary reads/writes from/to images`。

因此，当你正在做的事情规范明确指出不会是同步的，你应该只使用`glFinish`。

## 像素传输速度慢

要实现良好的像素传输性能，您需要使用实现可以直接使用的像素传输格式。考虑一下：

```c
glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
```

问题是像素传输格式`GL_RGBA`可能不直接支持`GL_RGBA8`格式。在某些平台上，`GPU`倾向于交换红色和蓝色（`GL_BGRA`）。

如果您提供`GL_RGBA`，则驱动程序可能需要为您进行缓慢的交换。如果您确实使用`GL_BGRA`，则对像素传输的调用将快得多。

请记住，对于第三个参数，它必须保持为`GL_RGBA8`。这定义了纹理的图像格式;最后三个参数描述了像素数据的存储方式。图像格式没有定义纹理存储的顺序，因此`GPU`仍然可以在内部存储为`BGRA`。

请注意，`GL_BGRA`像素传输格式仅在上传到`GL_RGBA8`图像时才是首选。在处理其他格式时，如`GL_RGBA16`，`GL_RGBA8UI`甚至`GL_RGBA8_SNORM`，那么常规的`GL_RGBA`排序可能是首选。

在哪个平台上首选`GL_BGRA`列表会太长，但是`Microsoft Windows`就是一个例子。请注意，使用`GL 4.3`或`ARB_internalformat_query2`，您可以简单地询问`glGetInternalFormativ(GL_TEXTURE_2D,GL_RGBA8,GL_TEXTURE_IMAGE_FORMAT,1,&preferred_format)`的首选格式。

## 交换缓冲区

一个现代的`OpenGL`程序应该总是使用双缓冲。一个现代的`3D OpenGL`程序也应该有一个深度缓冲区。

渲染序列应该是这样的：

```c++
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
RenderScene();
SwapBuffers(HDC); //对于Windows
```

应始终清除缓冲区。在更老的硬件上，有一种技术可以在不清除场景的情况下离开，但即使是在最近的硬件上，这也会让事情变得更慢。所以一定要清楚。

## 像素所有权问题

如果您的窗口被覆盖，或者部分覆盖了窗口，或者窗口位于桌面区域之外，则`GPU`可能无法渲染到这些部分。从这些区域读取也可能产生垃圾数据。

这是因为这些像素不能通过“像素所有权测试”。只有通过此测试的像素才具有有效的数据。那些失败的内容没有定义。

如果这对您是一个问题（注意：如果您需要从覆盖区域读回数据，这只是一个问题），解决方案是呈现给`Framebuffer`对象并渲染到该对象。如果需要显示图像，则可以使用默认帧缓冲区。

## glEnable(GL_POLYGON_SMOOTH)

这不是推荐的抗锯齿方法。 改用`Multisampling`。

## 位字段枚举器

一些`OpenGL`枚举器代表特定位域中的位。所有这些都以`_BIT`结尾（在任何扩展后缀之前）。看看这个例子：

```c++
glEnable(GL_BLEND | GL_DRAW_BUFFER); // invalid
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // valid
```

第一行是错误的。因为这些枚举器都不以`_BIT`结尾，所以它们不是位域，因此不应该一起进行`OR`运算。

相比之下，第二行是非常好的。所有这些都以`_BIT`结尾，所以这是有道理的。

## 三重缓冲

您无法控制驱动程序是否执行三重缓冲。你可以尝试使用`FBO`自己实现它。但是，如果驱动程序已经在执行三重缓冲，那么您的代码只能将其变成四重缓冲。这通常是矫枉过正

## 调色板的纹理

主要的`GL`供应商已经放弃了对`EXT_paletted_texture`扩展的支持。如果您真的需要在新硬件上使用调色板纹理，则可以使用着色器来实现这一效果。

着色器示例：

```c
//Fragment shader
#version 110
uniform sampler2D ColorTable;     //256 x 1 pixels
uniform sampler2D MyIndexTexture;
varying vec2 TexCoord0;

void main()
{
  //What color do we want to index?
  vec4 myindex = texture2D(MyIndexTexture, TexCoord0);
  //Do a dependency texture read
  vec4 texel = texture2D(ColorTable, myindex.xy);
  gl_FragColor = texel;   //Output the color
}
```

`ColorTable`可能采用您选择的格式，例如`GL_RGBA8`。 `ColorTable`可以是`256 x 1`像素大小的纹理。

`MyIndexTexture`可以是任何格式，虽然`GL_R8`非常合适（`GL_R8`在`GL 3.0`中可用）。 `MyIndexTexture`可以是任何维度，如`64 x 32`。

我们阅读MyIndexTexture，并使用这个结果作为texcoord来读取ColorTable。 如果您希望执行调色板动画，或仅更新颜色表中的颜色，则可以使用glTexSubImage2D将新值提交给ColorTable。 假设颜色表格是GL_RGBA格式：

```c
glBindTexture(GL_TEXTURE_2D, myColorTableID);
glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 1, GL_BGRA, GL_UNSIGNED_BYTE, mypixels);
```

## 禁用深度测试，并允许深度写入

在某些情况下，您可能想要禁用深度测试，并在渲染对象时仍允许更新深度缓冲区。事实证明，如果您禁用深度测试（`glDisable(GL_DEPTH_TEST)`），则`GL`也禁止写入深度缓冲区。正确的解决方案是告诉`GL`使用`glDepthFunc(GL_ALWAYS)`忽略深度测试结果。要小心，因为在这种状态下，如果最后渲染一个远处的对象，则深度缓冲区将包含该远处对象的值。

## glGetFloatv glGetBooleanv glGetDoublev glGetIntegerv

你发现这些功能很慢。

这很正常。 `glGet`表单的任何函数都可能会很慢。 `nVidia`和`ATI/AMD`建议您避免使用它们。 `GL`驱动程序（也是`GPU`）倾向于接收信息。如果你自己跟踪信息，你可以避免所有的`glGet`调用。

## y轴

几乎在`OpenGL`中的所有东西都使用一个坐标系，`X`指向右，`Y`指向上。这包括像素传输函数和纹理坐标。

例如，`glReadPixels`采用`x`和`y`的位置。 `y`轴从底部被认为是`0`，顶部是一些值。对于一些习惯了操作系统的操作系统来说，这对`y`轴是颠倒过来的（你的窗口的`y`轴是从上到下，你的鼠标的坐标是`y`轴从上到下）看起来可能不直观。解决方案对于鼠标是显而易见的：`windowHeight - mouseY`。

对于纹理，`GL`认为`y`轴是从下到上，最下面是`0.0`，最上面是`1.0`。有些人将他们的位图加载到`GL`纹理，并想知道为什么它们在模型上反转。解决方法很简单：通过执行`1.0 - v`来反转位图或反转模型的`texcoord`。

## 渲染方法中的glGenTextures

似乎有些人在渲染功能中创建了一个纹理。不要在渲染函数中创建资源。这也适用于所有其他`glGen`函数调用。不要读取模型文件，并在渲染函数中使用它们创建`VBO`。尝试在程序开始时分配资源。当程序终止时释放这些资源。

最糟糕的是，有些在渲染函数中创建纹理（或任何其他`GL`对象），并且从不调用`glDeleteTextures`。每次渲染函数被调用时，都会创建一个新纹理而不会释放旧纹理！

## 错误的数组大小

我们将用`GL 1.1`给出这个例子，但是如果您使用的是未来版本的`OpenGL`的`VBO`或任何其他功能，则同样的原则适用。

这个代码有什么问题？

```c++
GLfloat vertex[] = {0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0};
GLfloat normal[] = {0.0, 0.0, 1.0};
GLfloat color[] = {1.0, 0.7, 1.0, 1.0};
GLushort index[] = {0, 1, 2, 3};
glVertexPointer(3, GL_FLOAT, sizeof(GLfloat)*3, vertex);
glNormalPointer(GL_FLOAT, sizeof(GLfloat)*3, normal);
glColorPointer(4, GL_FLOAT, sizeof(GLfloat)*4, color);
glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, index);
```

目的是呈现一个四元组，但你的数组大小不匹配。 只有`1`个法线，而`GL`每个顶点都需要`1`个法线。 你的四边形只有一个`RGBA`颜色，而`GL`每个顶点都需要一种颜色。 您可能会导致系统崩溃，因为`GL`驱动程序将从您提供的正常和颜色数组的大小之外进行读取。

常见问题也解释了这个问题。

