//
// Created by freezestudio on 2017/11/11.
//

#ifndef FREEGL_QUICKLY_H
#define FREEGL_QUICKLY_H

namespace freeze 
{
    static inline texture2d quickly_texture2d(
    std::vector<char> const& data)
    {
        auto tex = make_texture2d();

        tex.bind();
        tex.set_image(data);
        tex.mipmap();
        tex.set_wrap_s(GL_REPEAT);
        tex.set_wrap_t(GL_REPEAT);
        tex.set_min_filter(GL_LINEAR_MIPMAP_NEAREST);
        tex.set_mag_filter(GL_LINEAR);
        tex.unbind();

        return tex;
    }

    static inline texture_cube quickly_texture_cube()
    {
        auto tex = make_texture_cube();
        //6 faces ...
        return tex;
    }
}

#endif //FREEGL_QUICKLY_H
