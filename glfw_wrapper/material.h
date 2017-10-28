//
// Created by ysb on 2017/10/27.
//

#ifndef VENDOR_MATERIAL_H
#define VENDOR_MATERIAL_H

namespace freeze {
    struct material {
        int ambient;
        int diffuse;
        int specular;
        float shininess;

        material(std::string const& name_from_fragment_shader){

        }
    };
}

#endif //VENDOR_MATERIAL_H
