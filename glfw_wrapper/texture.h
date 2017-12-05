//
// Created by freezestudio on 2017/10/11.
//

#ifndef FREEGL_TEXTURE_H
#define FREEGL_TEXTURE_H

namespace freeze
{
    template<bool>
    struct parameter
    {
        static void set(GLenum target, GLenum pname, GLfloat param)
        {
            glTexParameterf(target, pname, param);
            assert_error();
        }
    };

    template<>
    struct parameter<false>
    {
        static void set(GLenum target, GLenum pname, GLint param)
        {
            glTexParameteri(target, pname, param);
            assert_error();
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
            assert_error();
        }

        void destroy(GLuint const* textures)
        {
            glDeleteTextures(1, textures);
            assert_error();
        }

    public:
        void active(std::size_t index = 0)
        {
            glActiveTexture(GL_TEXTURE0 + index);
            assert_error();
        }

        void bind()
        {
            glBindTexture(Target, this->ref());
            assert_error();
        }

        void unbind() {
            glBindTexture(Target, 0);
            assert_error();
        }

        void mipmap()
        {
            glGenerateMipmap(Target);
            assert_error();
        }

        //GL_TEXTURE_BORDER_COLOR or GL_TEXTURE_SWIZZLE_RGBA
        void set_parameter(GLenum pname, const GLfloat* params)
        {
            glTexParameterfv(Target, pname, params);
            assert_error();
        }

        //T: GLfloat,GLint
        template<typename T>
        void set_parameter(GLenum pname, T param)
        {
            //ver>=4.5
            //glTextureParameteri(this->ref(), pname, param);
            //need c++17
            if constexpr(std::is_same<T, GLfloat>::value) {
                glTexParameterf(Target, pname, param);
            }
            else {
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

                //纹理环绕方式S,T,R (S,T,P,Q)(X,Y,Z,W)
                //GL_REPEAT	              对纹理的默认行为。重复纹理图像。
                //GL_MIRRORED_REPEAT	  和GL_REPEAT一样，但每次重复图片是镜像放置的。
                //GL_MIRROR_CLAMP_TO_EDGE (ver >= 4.4)
                //GL_CLAMP_TO_EDGE	      纹理坐标会被约束在0到1之间，超出的部分会重复纹理坐标的边缘，产生一种边缘被拉伸的效果。
                //GL_CLAMP_TO_BORDER	  超出的坐标为用户指定的边缘颜色。(需随后调用set_border_color，以设置一种边框色)
        void set_wrap_s(GLint param)
        {
            set_parameter(GL_TEXTURE_WRAP_S, param);
        }

        //GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER, GL_MIRRORED_REPEAT, GL_REPEAT, 
        //GL_MIRROR_CLAMP_TO_EDGE
        void set_wrap_t(GLint param)
        {
            set_parameter(GL_TEXTURE_WRAP_T, param);
        }

        //GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER, GL_MIRRORED_REPEAT, GL_REPEAT, 
        //GL_MIRROR_CLAMP_TO_EDGE
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

        void set_border_color(GLfloat const* colors)
        {
            glTexParameterfv(Target, GL_TEXTURE_BORDER_COLOR, colors);
        }

        //        void set_swizzle_rgba(GLfloat const* param)
        //        {
        //            set_parameter(GL_TEXTURE_SWIZZLE_RGBA, param);
        //        }

    };
}

namespace freeze
{
    template<typename = void>
    struct texture2d_impl
        : texture_base<GL_TEXTURE_2D>
    {
        //别忘了先绑定
        // internalFormat -- GL_DEPTH_COMPONENT,GL_DEPTH_STENCIL,GL_RED,GL_RG,GL_RGB,GL_RGBA
        // widht
        // height
        // format         -- GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_RED_INTEGER, 
        //                   GL_RG_INTEGER, GL_RGB_INTEGER, GL_BGR_INTEGER, GL_RGBA_INTEGER, 
        //                   GL_BGRA_INTEGER, GL_STENCIL_INDEX, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL.
        //
        // type           -- GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, 
        //                   GL_INT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, 
        //                   GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4, 
        //                   GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, 
        //                   GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, 
        //                   GL_UNSIGNED_INT_2_10_10_10_REV.
        void set_image(GLint internalFormat,
            GLsizei width, GLsizei height, GLenum format,
            GLenum type, const void* pixels)
        {
            this->format = format;
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
                width, height, 0, format, type, pixels);
            assert_error();
        }

        //别忘了先绑定
        bool set_image(std::vector<char> const& data)
        {
            int x, y, channels;

            auto image_data = stbi_load_from_memory((stbi_uc*)data.data(), data.size(), &x, &y, &channels, 0);
            if (!image_data)return false;

            switch (channels)
            {
            default:
            case 1:this->format = GL_RED; break;
            case 3:this->format = GL_RGB; break;
            case 4:this->format = GL_RGBA; break;
            }
            glTexImage2D(GL_TEXTURE_2D, 0, this->format,
                x, y, 0, this->format, GL_UNSIGNED_BYTE, (void const*)image_data);
            stbi_image_free(image_data);
            return true;
        }

        //template<typename DataType>
        //void set_image(DataType&& data){
        //    set_image(data.format,data.x,data.y,data.format,GL_UNSIGNED_BYTE,data.data.data());
        //}

        GLenum get_format() const
        {
            return format;
        }
    private:
        GLenum format = GL_RGB;
    };

    template<typename = void>
    struct texture2dmultisample_impl
        : texture_base<GL_TEXTURE_2D_MULTISAMPLE>
    {
        //别忘了先绑定
        void set_image(GLsizei samples, GLint internalFormat,
            GLsizei width, GLsizei height, GLboolean fixedsamplelocations)
        {
            glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat,
                width, height, fixedsamplelocations);
        }
    };

    template<typename = void>
    struct texture_cube_impl : texture_base<GL_TEXTURE_CUBE_MAP>
    {
        //绑定一次
        //调用6次分别设置+-x,+-y,+-z
        void set_image(GLsizei index, GLint internalFormat,
            GLsizei width, GLsizei height, GLenum format,
            GLenum type, const void* pixels)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, 0, internalFormat,
                width, height, 0, format, type, pixels);
            assert_error();
        }

        void set_image(std::vector<std::vector<char>> const& datas)
        {
            int x, y, channels;
            GLenum format;
            auto index = 0;
            for (auto data : datas)
            {
                auto image_data = stbi_load_from_memory((const stbi_uc*)data.data(), data.size(), &x, &y, &channels, 0);
                if (!image_data)return;
                switch (channels)
                {
                default:
                case 3:format = GL_RGB; break;
                case 4:format = GL_RGBA; break;
                }
                set_image(index++, format, x, y, format, GL_UNSIGNED_BYTE, image_data);
                stbi_image_free(image_data);
            }
        }
    };

    using texture2d = texture2d_impl<>;
    using texture2dmultisample = texture2dmultisample_impl<>;
    using texture_cube = texture_cube_impl<>;

    constexpr auto make_texture2d = make<texture2d>;
    constexpr auto make_texture2dmultisample = make<texture2dmultisample>;
    constexpr auto make_texture_cube = make<texture_cube>;
}

namespace freeze
{
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
    inline std::vector<char> load_image_from_file(std::string const& file)
    {
        std::vector<char> buffer;

        auto path = fs::path{ file };
        auto file_size = fs::file_size(path);
        if (file_size <= 0)return buffer;

        std::ifstream ifs{ file, std::ios::in | std::ios::binary };
        if (!ifs.is_open() || !ifs.good())
        {
            ifs.clear();
            ifs.close();
            ifs.open(file, std::ios::in | std::ios::binary);
            if (ifs.bad())return buffer;
        }


        buffer.resize(static_cast<std::size_t>(file_size));
        ifs.read(buffer.data(), file_size);
        ifs.close();

        return buffer;
    }
#endif

#if defined(ANDROID) || defined(__ANDROID__)
    template<typename TextureType, typename AssetManager>
    inline TextureType load_texture_from_assets(AssetManager const& assetmgr, std::string const& file)
    {
        TextureType texture;
        if (!texture)
        {
            LOGE("load_texture_from_assets(...) create texture 2d failed");
        }

        auto file_size = assetmgr.get_file_size(file);
        if (file_size == -1)return texture;

        auto buffer = malloc(file_size);
        auto read_size = assetmgr.get_file_data(file, buffer, file_size);
        if (read_size == -1)return texture;

        int x, y, channels;
        GLenum format;
        auto data = stbi_load_from_memory((stbi_uc*)buffer, read_size, &x, &y, &channels, 0);
        if (!data)
        {
            free(buffer);
            return texture;
        }

        switch (channels)
        {
        default:break;
        case 1:format = GL_RED; break;
        case 3:format = GL_RGB; break;
        case 4:format = GL_RGBA; break;
        }
        texture.bind();
        texture.set_image(format, x, y, format, GL_UNSIGNED_BYTE, data);
        free(buffer);
        stbi_image_free(data);

        texture.mipmap();
        texture.set_wrap_s(GL_REPEAT);
        texture.set_wrap_t(GL_REPEAT);
        texture.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
        texture.set_mag_filter(GL_LINEAR);

        return std::move(texture);
    }

    template<typename TextureType, typename AssetManager>
    inline TextureType load_cubetexture_from_assets(AssetManager const& assetmgr, std::vector<std::string> const& files)
    {
        TextureType texture;
        if (!texture)
        {
            LOGE("load_cubetexture_from_assets(...) create cube texture failed");
        }

        texture.bind();

        int x, y, channels;
        GLenum format;
        int index = 0;
        for (auto file : files)
        {
            auto file_size = assetmgr.get_file_size(file);
            if (file_size == -1)return texture;

            auto buffer = malloc(file_size);
            auto read_size = assetmgr.get_file_data(file, buffer, file_size);
            if (read_size == -1)return texture;

            auto data = stbi_load_from_memory((stbi_uc*)buffer, read_size, &x, &y, &channels, 0);
            if (!data)
            {
                free(buffer);
                return texture;
            }

            switch (channels)
            {
            default:
            case 3:format = GL_RGB; break;
            case 4:format = GL_RGBA; break;
            }
            texture.set_image(index++, format, x, y, format, GL_UNSIGNED_BYTE, data);
            free(buffer);
            stbi_image_free(data);
        }

        //texture.mipmap();
        texture.set_wrap_s(GL_CLAMP_TO_EDGE);
        texture.set_wrap_t(GL_CLAMP_TO_EDGE);
        texture.set_wrap_r(GL_CLAMP_TO_EDGE);
        texture.set_min_filter(GL_LINEAR);
        texture.set_mag_filter(GL_LINEAR);
        texture.unbind();

        return std::move(texture);
    }

    template<typename TextureType>
    inline TextureType load_texture_from_sdcard(std::string const& file)
    {
        TextureType texture;
        if (!texture)
        {
            LOGE("load_texture_from_sdcard(...) create texture failed");
        }

        int x, y, channels;
        GLenum format;
        auto data = stbi_load(file.c_str(), &x, &y, &channels, 0);
        if (data)
        {
            switch (channels)
            {
            default:break;
            case 1:format = GL_RED; break;
            case 3:format = GL_RGB; break;
            case 4:format = GL_RGBA; break;
            }
            texture.bind();
            texture.set_image(format, x, y, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);

            texture.mipmap();
            texture.set_wrap_s(GL_REPEAT);
            texture.set_wrap_t(GL_REPEAT);
            texture.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
            texture.set_mag_filter(GL_LINEAR);
            texture.unbind();
        }

        return std::move(texture);
    }

#endif //#if defined(ANDROID) || defined(__ANDROID__)
}

#endif //FREEGL_TEXTURE_H
