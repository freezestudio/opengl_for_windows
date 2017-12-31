//
// Created by freezestudio on 2017/11/11.
//

#ifndef FREEGL_QUICKLY_H
#define FREEGL_QUICKLY_H

namespace freeze
{
    static inline void quickly_set_texture2d(texture2d& tex,
        std::vector<char> const& data)
    {
        tex.bind();
        tex.set_image(data);
        tex.mipmap();
        tex.set_wrap_s(GL_REPEAT);
        tex.set_wrap_t(GL_REPEAT);
        tex.set_min_filter(GL_LINEAR_MIPMAP_NEAREST);
        tex.set_mag_filter(GL_LINEAR);
        tex.unbind();
    }

    static inline void quickly_set_texture_cube(texture_cube& tex)
    {        
        //6 faces ...
    }

	static inline void quickly_set_texture2d_hdr(texture2d& tex,std::string const& file)
	{
		tex.bind();
		int width, height, components;
		stbi_set_flip_vertically_on_load(1);
		auto data = stbi_loadf(file.c_str(), &width, &height, &components, 0);
		if (data)
		{
			tex.set_image(GL_RGB16F, width, height, GL_RGB, GL_FLOAT, data);
			//tex.mipmap();
			tex.set_wrap_s(GL_CLAMP_TO_EDGE);
			tex.set_wrap_t(GL_CLAMP_TO_EDGE);
			tex.set_min_filter(GL_LINEAR);
			tex.set_mag_filter(GL_LINEAR);
			stbi_image_free(data);
		}
        stbi_set_flip_vertically_on_load(0);
		tex.unbind();
	}
	
	template<typename... Args>
	static inline void quickly_set_vbo(vertex_buffer& vbo,
        void const* data,std::size_t size,Args... args)
	{
		vbo.bind();
		vbo.copy_data(data, size);
		auto plane_vertex = freeze::make_vertex();
		set([&plane_vertex](auto i,auto n,auto s,auto d) {
			plane_vertex.set(i, n, s, d);
		}, args...);
		vbo.unbind();
	}

	static inline void clear_color(float r, float g, float b, float a = 1.0f)
	{
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	static inline void clear_color_depth(float r, float g, float b, float a = 1.0f)
	{
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	static inline void clear_color_depth_stencil(float r, float g, float b, float a = 1.0f)
	{
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}
}

namespace freeze
{
    template<typename T>
    T random(T min, T max)
    {
        std::random_device random_device;
        //operator()()
        auto rand = random_device();

        std::default_random_engine random_engine{ rand };
        std::uniform_real_distribution<T> urdist(min, max);
        //operator()()
        return urdist(random_engine);
    }
}

#endif //FREEGL_QUICKLY_H
