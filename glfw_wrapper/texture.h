//
// Created by ysb on 2017/10/11.
//

#ifndef CDS_TEXTURE_H
#define CDS_TEXTURE_H

namespace freeze {
    template<bool>
    struct parameter
    {
        static void set(GLenum target, GLenum pname, GLfloat param)
        {
            glTexParameterf(target, pname, param);
        }
    };

    template<>
    struct parameter<false>
    {
        static void set(GLenum target, GLenum pname, GLint param)
        {
            glTexParameteri(target, pname, param);
        }
    };

    //Target:
    //    GL_TEXTURE_1D
    //    GL_TEXTURE_2D
    //    GL_TEXTURE_3D
    //    GL_TEXTURE_1D_ARRAY
    //    GL_TEXTURE_2D_ARRAY
    //    GL_TEXTURE_RECTANGLE
    //    GL_TEXTURE_CUBE_MAP
    //    GL_TEXTURE_CUBE_MAP_ARRAY
    //    GL_TEXTURE_BUFFER
    //    GL_TEXTURE_2D_MULTISAMPLE
    //    GL_TEXTURE_2D_MULTISAMPLE_ARRAY
    template<GLenum Target>
    struct texture_base : make_object<texture_base<Target>>
    {
    public:
        void create(GLuint* textures)
        {
            glGenTextures(1, textures);
        }

        void destroy(GLuint const* textures)
        {
            glDeleteTextures(1, textures);
        }

    public:
        void active(std::size_t index = 0)
        {
            glActiveTexture(GL_TEXTURE0 + index);
        }

        void bind()
        {
            glBindTexture(Target, this->ref());
        }

        void unbind(){
            glBindTexture(Target,0);
        }

        void mipmap()
        {
            glGenerateMipmap(Target);
        }

        //GL_TEXTURE_BORDER_COLOR or GL_TEXTURE_SWIZZLE_RGBA
        void set_parameter(GLenum pname, const GLfloat* params)
        {
            glTexParameterfv(Target, pname, params);
        }

        //T: GLfloat,GLint
        template<typename T>
        void set_parameter(GLenum pname, T param)
        {
            //ver>=4.5
            //glTextureParameteri(this->ref(), pname, param);
            //need c++17
            if constexpr(std::is_same<T, GLfloat>::value){
                    glTexParameterf(Target, pname, param);
            }else{
                glTexParameteri(Target, pname, param);
            }

            //parameter<std::is_same<T, GLfloat>::value>::set(Target, pname, param);
        }

    public:
        //用于从深度-模板格式的纹理中读取的模式 ver>=4.3
        //GL_DEPTH_COMPONENT,GL_STENCIL_COMPONENT
//        void set_depth_stencil_mode(GLint param = GL_DEPTH_COMPONENT)
//        {
//            set_parameter( GL_DEPTH_STENCIL_TEXTURE_MODE, param);
//        }


        void set_level(GLint level = 0)
        {
            set_parameter(GL_TEXTURE_BASE_LEVEL, level);
        }

//        void set_border_color(const GLfloat* rgba)
//        {
//            set_parameter(GL_TEXTURE_BORDER_COLOR, rgba);
//        }

        //glTexImage2D调用中的internal format 为 GL_DEPTH_COMPONENT_*时
        //    GL_COMPARE_REF_TO_TEXTURE (设为此值时，需随后调用set_compare_func)
        //    GL_NONE
        void set_compare_mode(GLint param)
        {
            set_parameter(GL_TEXTURE_COMPARE_MODE, param);
        }

        //GL_LEQUAL,GL_GEQUAL,GL_LESS,GL_GREATER,GL_EQUAL,GL_NOTEQUAL,GL_ALWAYS,GL_NEVER
        void set_compare_func(GLint param)
        {
            set_parameter(GL_TEXTURE_COMPARE_FUNC, param);
        }

//        void set_lod_bias(GLfloat param = 0.0f)
//        {
//            if (param > -GL_MAX_TEXTURE_LOD_BIAS && param < GL_MAX_TEXTURE_LOD_BIAS)
//            {
//                set_parameter(GL_TEXTURE_LOD_BIAS, param);
//            }
//        }

        //纹理环绕方式(S,T,R,Q)
        //GL_REPEAT	              对纹理的默认行为。重复纹理图像。
        //GL_MIRRORED_REPEAT	  和GL_REPEAT一样，但每次重复图片是镜像放置的。
        //GL_MIRROR_CLAMP_TO_EDGE (ver >= 4.4)
        //GL_CLAMP_TO_EDGE	      纹理坐标会被约束在0到1之间，超出的部分会重复纹理坐标的边缘，产生一种边缘被拉伸的效果。
        //GL_CLAMP_TO_BORDER	  超出的坐标为用户指定的边缘颜色。(需随后调用set_border_color，以设置一种边框色)
        void set_wrap_s(GLint param)
        {
            set_parameter(GL_TEXTURE_WRAP_S, param);
        }

        void set_wrap_t(GLint param)
        {
            set_parameter(GL_TEXTURE_WRAP_T, param);
        }

        void set_wrap_r(GLint param)
        {
            set_parameter(GL_TEXTURE_WRAP_R, param);
        }

        //param: 纹理过滤
        //    邻近过滤 GL_NEAREST,线性过滤 GL_LINEAR,
        //
        //GL_NEAREST_MIPMAP_NEAREST 使用最邻近的多级渐远纹理来匹配像素大小，并使用邻近插值进行纹理采样
        //GL_LINEAR_MIPMAP_NEAREST  使用最邻近的多级渐远纹理级别，并使用线性插值进行采样
        //GL_NEAREST_MIPMAP_LINEAR  在两个最匹配像素大小的多级渐远纹理之间进行线性插值，使用邻近插值进行采样
        //GL_LINEAR_MIPMAP_LINEAR   在两个邻近的多级渐远纹理之间使用线性插值，并使用线性插值进行采样
        void set_min_filter(GLint param)
        {
            set_parameter(GL_TEXTURE_MIN_FILTER, param);
        }

        //邻近过滤 GL_NEAREST,线性过滤 GL_LINEAR
        void set_mag_filter(GLint param)
        {
            set_parameter(GL_TEXTURE_MAG_FILTER, param);
        }

        void set_min_lod(GLfloat value = -1000.f)
        {
            set_parameter(GL_TEXTURE_MIN_LOD, value);
        }

        void set_max_lod(GLfloat value = 1000.f)
        {
            set_parameter(GL_TEXTURE_MAX_LOD, value);
        }

        void set_max_level(GLint level = 1000)
        {
            set_parameter(GL_TEXTURE_MAX_LEVEL, level);
        }

        void set_swizzle_r(GLint param = GL_RED)
        {
            set_parameter(GL_TEXTURE_SWIZZLE_R, param);
        }

        void set_swizzle_g(GLint param = GL_GREEN)
        {
            set_parameter(GL_TEXTURE_SWIZZLE_G, param);
        }

        void set_swizzle_b(GLint param = GL_BLUE)
        {
            set_parameter(GL_TEXTURE_SWIZZLE_B, param);
        }

        void set_swizzle_a(GLint param = GL_ALPHA)
        {
            set_parameter(GL_TEXTURE_SWIZZLE_A, param);
        }

//        void set_swizzle_rgba(GLfloat const* param)
//        {
//            set_parameter(GL_TEXTURE_SWIZZLE_RGBA, param);
//        }

    };
}

namespace freeze
{
    template<typename=void>
    struct texture2d_impl : texture_base<GL_TEXTURE_2D>
    {
        //别忘了先绑定
        void set_image(GLint internalFormat,
                       GLsizei width, GLsizei height, GLenum format,
                       GLenum type, const void* pixels)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
                         width, height, 0, format, type, pixels);
        }
        //别忘了先绑定
        void set_image(std::vector<char> const& data){
            int x,y,channels;
            GLenum format;
            auto image_data = stbi_load_from_memory((stbi_uc*)data.data(),data.size(),&x,&y,&channels,0);
            if(!image_data)return;

            switch (channels){
                default:
                case 3:format=GL_RGB;break;
                case 4:format=GL_RGBA;break;
            }
            set_image(format,x,y,format,GL_UNSIGNED_BYTE,image_data);
            stbi_image_free(image_data);
        }

//        template<typename DataType>
//        void set_image(DataType&& data){
//            set_image(data.format,data.x,data.y,data.format,GL_UNSIGNED_BYTE,data.data.data());
//        }
    };

    template<typename = void>
    struct texture2dmultisample_impl : texture_base<GL_TEXTURE_2D_MULTISAMPLE>{
        //别忘了先绑定
        void set_image(GLsizei samples,GLint internalFormat,
                       GLsizei width, GLsizei height,GLboolean fixedsamplelocations)
        {
            glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,samples,internalFormat,
            width,height,fixedsamplelocations);
        }
    };

    template<typename=void>
    struct texture_cube_impl : texture_base<GL_TEXTURE_CUBE_MAP>{
        //绑定一次
        //调用6次分别设置+-x,+-y,+-z
        void set_image(GLsizei index,GLint internalFormat,
                       GLsizei width, GLsizei height, GLenum format,
                       GLenum type, const void* pixels ){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index,0,internalFormat,
            width,height,0,format,type,pixels);
        }

        void set_image(std::vector<std::vector<char>> const& datas){
            int x,y,channels;
            GLenum format;
            auto index = 0;
            for(auto data : datas){
                auto image_data = stbi_load_from_memory((const stbi_uc*) data.data(),data.size(),&x,&y,&channels,0);
                if(!image_data)return;
                switch (channels){
                    default:
                    case 3:format=GL_RGB;break;
                    case 4:format=GL_RGBA;break;
                }
                set_image(index++,format,x,y,format,GL_UNSIGNED_BYTE,image_data);
                stbi_image_free(image_data);
            }
        }
    };

    using texture2d = texture2d_impl<>;
    constexpr auto make_texture2d = make<texture2d>;
    using texture2dmultisample = texture2dmultisample_impl<>;
    constexpr auto make_texture2dmultisample = make<texture2dmultisample>;
    using texture_cube = texture_cube_impl<>;
    constexpr auto make_texture_cube = make<texture_cube>;
}

namespace  freeze {

    template<typename TextureType,typename AssetManager>
    inline TextureType load_texture_from_assets(AssetManager const& assetmgr,std::string const& file){
        TextureType texture;
        if(!texture){
            LOGE("load_texture_from_assets(...) create texture 2d failed");
        }

        auto file_size = assetmgr.get_file_size(file);
        if(file_size == -1)return texture;

        auto buffer = malloc(file_size);
        auto read_size = assetmgr.get_file_data(file,buffer,file_size);
        if(read_size == -1)return texture;

        int x,y,channels;
        GLenum format;
        auto data = stbi_load_from_memory((stbi_uc*)buffer,read_size,&x,&y,&channels,0);
        if(!data){
            free(buffer);
            return texture;
        }

        switch (channels){
            default:
            case 3:format=GL_RGB;break;
            case 4:format=GL_RGBA;break;
        }
        texture.bind();
        texture.set_image(format,x,y,format,GL_UNSIGNED_BYTE,data);
        free(buffer);
        stbi_image_free(data);

        texture.mipmap();
        texture.set_wrap_s(GL_REPEAT);
        texture.set_wrap_t(GL_REPEAT);
        texture.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
        texture.set_mag_filter(GL_LINEAR);

        return std::move(texture);
    }

    template<typename TextureType,typename AssetManager>
    inline TextureType load_cubetexture_from_assets(AssetManager const& assetmgr,std::vector<std::string> const& files){
        TextureType texture;
        if(!texture){
            LOGE("load_cubetexture_from_assets(...) create cube texture failed");
        }

        texture.bind();

        int x,y,channels;
        GLenum format;
        int index = 0;
        for(auto file : files){
            auto file_size = assetmgr.get_file_size(file);
            if(file_size == -1)return texture;

            auto buffer = malloc(file_size);
            auto read_size = assetmgr.get_file_data(file,buffer,file_size);
            if(read_size == -1)return texture;

            auto data = stbi_load_from_memory((stbi_uc*)buffer,read_size,&x,&y,&channels,0);
            if(!data){
                free(buffer);
                return texture;
            }

            switch (channels){
                default:
                case 3:format=GL_RGB;break;
                case 4:format=GL_RGBA;break;
            }
            texture.set_image(index++,format,x,y,format,GL_UNSIGNED_BYTE,data);
            free(buffer);
            stbi_image_free(data);
        }

        //texture.mipmap();
        texture.set_wrap_s(GL_CLAMP_TO_EDGE);
        texture.set_wrap_t(GL_CLAMP_TO_EDGE);
        texture.set_wrap_r(GL_CLAMP_TO_EDGE);
        texture.set_min_filter(GL_LINEAR);
        texture.set_mag_filter(GL_LINEAR);

        return std::move(texture);
    }

    template<typename TextureType>
    inline TextureType load_texture_from_sdcard(std::string const& file){
        TextureType texture;
        if(!texture){
            LOGE("load_texture_from_sdcard(...) create texture failed");
        }

        int x,y,channels;
        GLenum format;
        auto data = stbi_load(file.c_str(),&x,&y,&channels,0);
        if(data){
            switch (channels){
                default:
                case 3:format=GL_RGB;break;
                case 4:format=GL_RGBA;break;
            }
            texture.bind();
            texture.set_image(format,x,y,format,GL_UNSIGNED_BYTE,data);
            stbi_image_free(data);

            texture.mipmap();
            texture.set_wrap_s(GL_REPEAT);
            texture.set_wrap_t(GL_REPEAT);
            texture.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
            texture.set_mag_filter(GL_LINEAR);
        }

        return std::move(texture);
    }
}

#endif //CDS_TEXTURE_H
