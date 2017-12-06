//
// Created by freezestudio on 2017/10/27.
//

#ifndef FREEGL_MATERIAL_H
#define FREEGL_MATERIAL_H

namespace freeze
{
    struct material
    {
        int ambient;//环境
        int diffuse;//漫射
        int specular;//高光
        int bump;//法线/凹凸贴图
        int reflection;//反射贴图
        int emission;//放射贴图
        float shininess;//高光散射

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

namespace freeze
{
	//菲涅尔方程，近似反射率系数
	namespace Fo
	{
		constexpr glm::vec3 linear_water = glm::vec3{ 0.02f };
		constexpr glm::vec3 linear_plastic = glm::vec3{ 0.03f };
		constexpr glm::vec3 linear_glass_low = glm::vec3{ 0.03f };
		constexpr glm::vec3 linear_plastic_high = glm::vec3{ 0.05f };
		constexpr glm::vec3 linear_glass_high = glm::vec3{ 0.08f };
		constexpr glm::vec3 linear_ruby = glm::vec3{ 0.08f };
		constexpr glm::vec3 linear_diamond = glm::vec3{ 0.17f };
		constexpr glm::vec3 linear_iron = glm::vec3{ 0.56f,0.57f,0.58f };
		constexpr glm::vec3 linear_copper = glm::vec3{ 0.95f,0.64f,0.54f };
		constexpr glm::vec3 linear_gold = glm::vec3{ 1.0f,0.71f,0.29f };
		constexpr glm::vec3 linear_aluminium = glm::vec3{ 0.91f,0.92f,0.92f };
		constexpr glm::vec3 linear_silver = glm::vec3{ 0.95f,0.93f,0.88f };

		constexpr glm::vec3 srgb_water = glm::vec3{ 0.15f };
		constexpr glm::vec3 srgb_plastic = glm::vec3{ 0.21f };
		constexpr glm::vec3 srgb_glass_low = glm::vec3{ 0.21f };
		constexpr glm::vec3 srgb_plastic_high = glm::vec3{ 0.24f };
		constexpr glm::vec3 srgb_glass_high = glm::vec3{ 0.31f };
		constexpr glm::vec3 srgb_ruby = glm::vec3{ 0.31f };
		constexpr glm::vec3 srgb_diamond = glm::vec3{ 0.45f };
		constexpr glm::vec3 srgb_iron = glm::vec3{ 0.77f,0.78f,0.78f };
		constexpr glm::vec3 srgb_copper = glm::vec3{ 0.98f,0.82f,0.76f };
		constexpr glm::vec3 srgb_gold = glm::vec3{ 1.0f,0.86f,0.57f };
		constexpr glm::vec3 srgb_aluminium = glm::vec3{ 0.96f,0.96f,0.97f };
		constexpr glm::vec3 srgb_silver = glm::vec3{ 0.98f,0.97f,0.95f };
	}
}

#endif //FREEGL_MATERIAL_H
