//
// Created by freezestudio on 2017/10/27.
//

#ifndef FREEGL_LIGHT_H
#define FREEGL_LIGHT_H

namespace freeze 
{
    struct attenuation
    {
        int d;
        float c;
        float l;
        float q;
    };

    constexpr attenuation g_attenuation[] = {
            7	,    1.0f,	0.7f,    1.8f,
            13	,    1.0f,	0.35f,	0.44f,
            20	,    1.0f,	0.22f,	0.20f,
            32	,    1.0f,	0.14f,	0.07f,
            50	,    1.0f,	0.09f,	0.032f,
            65	,    1.0f,	0.07f,	0.017f,
            100	,    1.0f,	0.045f,	0.0075f,
            160	,    1.0f,	0.027f,	0.0028f,
            200	,    1.0f,	0.022f,	0.0019f,
            325	,    1.0f,	0.014f,	0.0007f,
            600	,    1.0f,	0.007f,	0.0002f,
            3250,	 1.0f,	0.0014f,0.000007f,
    };

    struct light 
    {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };

    struct direction_light
        : light 
    {
        glm::vec3 direction;
    };

    //f = 1.0f/(c +l*d+q*d*d)
    struct point_light
        : light
    {
        glm::vec3 position;
        float constant;
        float linear;
        float quadratic;
    };

    //i = (theta-gamma)/(phi-gamma)
    struct spot_light
        : point_light
    {
        glm::vec3 direction;
        float cut_off;
        float outer_cut_off;
    };
}

#endif //FREEGL_LIGHT_H
