#include "pbr_renderer.h"



pbr_renderer::pbr_renderer()
    : scene_camera{0.0f,0.0f,3.0f}
{
}


pbr_renderer::~pbr_renderer()
{
}

// lights
// ------
glm::vec3 lightPositions[] = {
    glm::vec3(-10.0f,  10.0f, 10.0f),
    glm::vec3(10.0f,  10.0f, 10.0f),
    glm::vec3(-10.0f, -10.0f, 10.0f),
    glm::vec3(10.0f, -10.0f, 10.0f),
};
glm::vec3 lightColors[] = {
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f)
};
int nrRows = 7;
int nrColumns = 7;
float spacing = 2.5;
std::size_t index_count = 0U;

void pbr_renderer::do_init()
{
    ms_instance = shared_from_this();
    freeze::depth::enable();

    pbr_shader.compile_file("resource/shaders/pbr.vert"s, "resource/shaders/pbr.frag");
    pbr_shader.use();
    pbr_shader.set_int("albedoMap"s, 0);
    pbr_shader.set_int("normalMap"s, 1);
    pbr_shader.set_int("metallicMap"s, 2);
    pbr_shader.set_int("roughnessMap"s, 3);
    pbr_shader.set_int("aoMap"s, 4);

    albedo_tex.bind();
    {
        auto albedo_data = freeze::load_image_from_file(
            "resource/textures/pbr/rusted_iron/albedo.png"s);
        albedo_tex.set_image(albedo_data);
        albedo_tex.mipmap();
        albedo_tex.set_wrap_s(GL_REPEAT);
        albedo_tex.set_wrap_s(GL_REPEAT);
        albedo_tex.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
        albedo_tex.set_mag_filter(GL_LINEAR);
    }    
    albedo_tex.unbind();

    normal_tex.bind();
    {
        auto normal_data = freeze::load_image_from_file(
            "resource/textures/pbr/rusted_iron/normal.png"s);
        normal_tex.set_image(normal_data);
        normal_tex.mipmap();
        normal_tex.set_wrap_s(GL_REPEAT);
        normal_tex.set_wrap_s(GL_REPEAT);
        normal_tex.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
        normal_tex.set_mag_filter(GL_LINEAR);
    }    
    normal_tex.unbind();

    metallic_tex.bind();
    {
        auto metallic_data = freeze::load_image_from_file(
            "resource/textures/pbr/rusted_iron/metallic.png"s);
        metallic_tex.set_image(metallic_data);
        metallic_tex.mipmap();
        metallic_tex.set_wrap_s(GL_REPEAT);
        metallic_tex.set_wrap_s(GL_REPEAT);
        metallic_tex.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
        metallic_tex.set_mag_filter(GL_LINEAR);
    }    
    metallic_tex.unbind();

    roughness_tex.bind();
    {
        auto roughness_data = freeze::load_image_from_file(
            "resource/textures/pbr/rusted_iron/roughness.png"s);
        roughness_tex.set_image(roughness_data);
        roughness_tex.mipmap();
        roughness_tex.set_wrap_s(GL_REPEAT);
        roughness_tex.set_wrap_s(GL_REPEAT);
        roughness_tex.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
        roughness_tex.set_mag_filter(GL_LINEAR);
    }    
    roughness_tex.unbind();

    ao_tex.bind();
    {
        auto ao_data = freeze::load_image_from_file(
            "resource/textures/pbr/rusted_iron/ao.png"s);
        ao_tex.set_image(ao_data);
        ao_tex.mipmap();
        ao_tex.set_wrap_s(GL_REPEAT);
        ao_tex.set_wrap_s(GL_REPEAT);
        ao_tex.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
        ao_tex.set_mag_filter(GL_LINEAR);
    }    
    ao_tex.unbind();

    auto projection = glm::perspective(glm::radians(scene_camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    pbr_shader.set_mat4("projection"s, projection);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<GLuint> indices;

    constexpr auto SEGMENTS = 64U;
    constexpr auto PI = 3.14159265358f;
    for (auto y = 0U; y < SEGMENTS; ++y)
    {
        for (auto x = 0u; x <= SEGMENTS; ++x)
        {
            auto xs = (float)x / (float)SEGMENTS;
            auto ys = (float)y / (float)SEGMENTS;
            auto xpos = std::cos(xs*2.0f*PI)*std::sin(ys*PI);
            auto ypos = std::cos(ys*PI);
            auto zpos = std::sin(xs*2.0f*PI) * std::sin(ys*PI);

            positions.push_back(glm::vec3{ xpos,ypos,zpos });
            uv.push_back(glm::vec2{ xs,ys });
            normals.push_back(glm::vec3{ xpos,ypos,zpos });
        }
    }

    auto oddRow = false;
    for (auto y = 0; y < SEGMENTS; ++y)
    {
        if (!oddRow)
        {
            for (auto x = 0; x <= SEGMENTS; ++x)
            {
                indices.push_back(y * (SEGMENTS + 1) + x);
                indices.push_back((y + 1)*(SEGMENTS + 1) + x);
            }
        }
        else
        {
            for (int x = SEGMENTS; x >= 0; --x)
            {
                indices.push_back((y + 1)*(SEGMENTS + 1) + x);
                indices.push_back(y*(SEGMENTS + 1) + x);
            }
        }

        oddRow = !oddRow;
    }

    index_count = indices.size();

    std::vector<float> data;
    for (auto i = 0u; i < positions.size(); ++i)
    {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        if (uv.size() > 0)
        {
            data.push_back(uv[i].x);
            data.push_back(uv[i].y);
        }
        if (normals.size() > 0)
        {
            data.push_back(normals[i].x);
            data.push_back(normals[i].y);
            data.push_back(normals[i].z);
        }
    }

    sphere_vao.bind();
    auto vbo = freeze::make_vertex_buffer();
    vbo.bind();
    vbo.copy_data(data.data(), data.size() * sizeof(float), GL_STATIC_DRAW);
    auto vex = freeze::make_vertex();
    vex.set(0, 3, GL_FLOAT, 8 * sizeof(float), 0);
    vex.set(1, 2, GL_FLOAT, 8 * sizeof(float), 3 * sizeof(float));
    vex.set(2, 3, GL_FLOAT, 8 * sizeof(float), 5 * sizeof(float));
    auto ebo = freeze::make_element_buffer();
    ebo.bind();
    ebo.copy_data(indices.data(), indices.size() * sizeof(GLuint), GL_STATIC_DRAW);
    vbo.unbind();
    sphere_vao.unbind();
}

void pbr_renderer::draw()
{
    pbr_shader.use();
    auto view = scene_camera.get_view_matrix();
    pbr_shader.set_mat4("view"s, view);
    pbr_shader.set_vec3("camPos"s, scene_camera.Position);
    albedo_tex.active();
    albedo_tex.bind();
    normal_tex.active(1);
    normal_tex.bind();
    metallic_tex.active(2);
    metallic_tex.bind();
    roughness_tex.active(3);
    roughness_tex.bind();
    ao_tex.active(4);
    ao_tex.bind();

    sphere_vao.bind();
    glm::mat4 model{ 1.0f };
    for (auto row = 0; row < nrRows; ++row)
    {
        for (auto col = 0; col < nrColumns; ++col)
        {
            model = glm::mat4{ 1.0f };
            model = glm::translate(model, glm::vec3{
                (float)(col - (nrColumns / 2))*spacing,
                (float)(row - (nrRows / 2))*spacing,
                0.0f });
            pbr_shader.set_mat4("model"s, model);
            glDrawElements(GL_TRIANGLE_STRIP, index_count, GL_UNSIGNED_INT,0);
        }
    }

    for (auto i = 0u; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
    {
        auto newPos = lightPositions[i] + glm::vec3{ sin(glfwGetTime()*5.0f)*5.0f,0.0f,0.0f };
        pbr_shader.set_vec3("lightPositions["s + std::to_string(i) + "]"s, newPos);
        pbr_shader.set_vec3("lightColors["s + std::to_string(i) + "]"s, lightColors[i]);
        model = glm::mat4{ 1.0f };
        model = glm::translate(model, newPos);
        model = glm::scale(model, glm::vec3{ 0.5f });
        pbr_shader.set_mat4("model"s, model);
        glDrawElements(GL_TRIANGLE_STRIP, index_count, GL_UNSIGNED_INT, 0);
    }

    sphere_vao.unbind();
}

void pbr_renderer::process_event(window_pointer window)
{
}
