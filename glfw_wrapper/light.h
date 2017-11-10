//
// Created by freezestudio on 2017/10/27.
//

#ifndef FREEGL_LIGHT_H
#define FREEGL_LIGHT_H

namespace freeze {
    struct attenuation{
        int d;
        float c;
        float l;
        float q;
    };

    constexpr attenuation g_attenuation[]={
            7	,    1.0,	0.7,    1.8,
            13	,    1.0,	0.35,	0.44,
            20	,    1.0,	0.22,	0.20,
            32	,    1.0,	0.14,	0.07,
            50	,    1.0,	0.09,	0.032,
            65	,    1.0,	0.07,	0.017,
            100	,    1.0,	0.045,	0.0075,
            160	,    1.0,	0.027,	0.0028,
            200	,    1.0,	0.022,	0.0019,
            325	,    1.0,	0.014,	0.0007,
            600	,    1.0,	0.007,	0.0002,
            3250,	 1.0,	0.0014,	0.000007,
    };

    struct light {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };

    struct direction_light : light {
        glm::vec3 direction;
    };

    //f = 1.0f/(c +l*d+q*d*d)
    struct point_light : light {
        glm::vec3 position;
        float constant;
        float linear;
        float quadratic;
    };

    //i = (theta-gamma)/(phi-gamma)
    struct spot_light : point_light {
        glm::vec3 direction;
        float cut_off;
        float outer_cut_off;
    };
}

#endif //FREEGL_LIGHT_H
