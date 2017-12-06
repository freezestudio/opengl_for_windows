//
// Created by freezestudio on 2017/10/27.
//

#ifndef FREEGL_MATERIAL_H
#define FREEGL_MATERIAL_H

namespace freeze
{
    struct material
    {
        int ambient;//����
        int diffuse;//����
        int specular;//�߹�
        int bump;//����/��͹��ͼ
        int reflection;//������ͼ
        int emission;//������ͼ
        float shininess;//�߹�ɢ��

        void set_ambient(program const& shader, std::string const& ambient)
        {
            shader.set_int(ambient, this->ambient);
        }

        void set_diffuse(program const& shader, std::string const& diffuse)
        {
            shader.set_int(diffuse, this->diffuse);
        }

        void set_specular(program const& shader, std::string const& specular)
        {
            shader.set_int(specular, this->specular);
        }

        void set_bump(program const& shader, std::string const& bump)
        {
            shader.set_int(bump, this->bump);
        }

        void set_reflection(program const& shader, std::string const& reflection)
        {
            shader.set_int(reflection, this->reflection);
        }

        void set_emission(program const& shader, std::string const& emission)
        {
            shader.set_int(emission, this->emission);
        }

        void set_shininess(program const& shader, std::string const& shininess)
        {
            shader.set_float(shininess, this->shininess);
        }
    };

    struct pbr_material
    {
        int albedo;
        int normal;
        int metallic;
        int roughness;
        int occlusion;
    };
}

#endif //FREEGL_MATERIAL_H
