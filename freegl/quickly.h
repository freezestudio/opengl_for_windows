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
		stbi_set_flip_vertically_on_load(true);
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
		tex.unbind();
	}
	
	template<typename... Args>
	static inline void quickly_set_vbo(vertex_buffer& vbo,void const* data,std::size_t size,Args... args)
	{
		vbo.bind();
		vbo.copy_data(data, size);
		auto plane_vertex = freeze::make_vertex();
		set([&plane_vertex](auto i,auto n,auto s,auto d) {
			plane_vertex.set(i, n, s, d);
		}, args...);
		vbo.unbind();
	}
}

#endif //FREEGL_QUICKLY_H