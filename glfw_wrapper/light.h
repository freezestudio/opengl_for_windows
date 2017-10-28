//
// Created by ysb on 2017/10/27.
//

#ifndef VENDOR_LIGHT_H
#define VENDOR_LIGHT_H

namespace freeze {
    struct light {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };

    struct direction_light : light {
        glm::vec3 direction;
    };

    struct point_light : light {
        glm::vec3 position;
        float constant;
        float linear;
        float quadratic;
    };

    struct spot_light : light {
        glm::vec3 position;
        glm::vec3 direction;
        float cut_off;
        float outer_cut_off;
        float constant;
        float linear;
        float quadratic;
    };
}

#endif //VENDOR_LIGHT_H
