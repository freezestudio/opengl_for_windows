#include "ibl_renderer.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// lights
// ------
static glm::vec3 lightPositions[] = {
    glm::vec3(-10.0f,  10.0f, 10.0f),
    glm::vec3(10.0f,  10.0f, 10.0f),
    glm::vec3(-10.0f, -10.0f, 10.0f),
    glm::vec3(10.0f, -10.0f, 10.0f),
};

static glm::vec3 lightColors[] = {
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f)
};

/////////////////////////////////////////////////////////////
static float cube_vertices[] = {
    // back face
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
     1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
    // front face
    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
     1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
    -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
    // left face
    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
    -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
    -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
    // right face
     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
     1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
     1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
    // bottom face
    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
     1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
     1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
     1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
    -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
    // top face
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
     1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
     1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
     1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
    -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
};

static float quad_vertices[] = {
    // positions        // texture Coords
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
};

static float particle_vertices[] = {
    // positions        // uv       // colors
    -0.05f,  0.05f, 0.0f, 0.0f, 1.0f, /* 1.0f,0.0f,0.0f, */
    -0.05f, -0.05f, 0.0f, 0.0f, 0.0f, /* 1.0f,0.0f,0.0f, */
     0.05f,  0.05f, 0.0f, 1.0f, 1.0f, /* 1.0f,0.0f,0.0f, */
     0.05f, -0.05f, 0.0f, 1.0f, 0.0f, /* 1.0f,0.0f,0.0f, */
};

static std::vector<freeze::particle> particles;
static int last_particle = 0;

/////////////////////////////////////////////////////////////

ibl_renderer::ibl_renderer()
    : scene_camera{ 0.0f,0.0f,3.0f }
    , last_x(SCR_WIDTH / 2.0f)
    , last_y(SCR_HEIGHT / 2.0f)
    , first_mouse(true)
    , delta_time(0.0f)
    , last_frame(0.0f)
{
}


ibl_renderer::~ibl_renderer()
{
}

void ibl_renderer::do_init()
{
    ms_instance = shared_from_this();

    freeze::depth::enable();
    freeze::depth::test_less_equal();
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glEnable(GL_MULTISAMPLE);

    set_particle();

    set_vertices();
    set_shader();
    set_texture();
    set_model();
}

void ibl_renderer::set_vertices()
{
    cube_vao.bind();
    auto cvbo = freeze::make_vertex_buffer();
    cvbo.bind();
    cvbo.copy_data(cube_vertices, sizeof(cube_vertices));
    freeze::vertex::set_enable(0, 3, 8, 0);
    freeze::vertex::set_enable(1, 3, 8, 3);
    freeze::vertex::set_enable(2, 2, 8, 6);
    cvbo.unbind();
    cube_vao.unbind();

    quad_vao.bind();
    auto qvbo = freeze::make_vertex_buffer();
    qvbo.bind();
    qvbo.copy_data(quad_vertices, sizeof(quad_vertices));
    freeze::vertex::set_enable(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    freeze::vertex::set_enable(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 3 * sizeof(float));
    qvbo.unbind();
    quad_vao.unbind();
}

void ibl_renderer::set_shader()
{
    pbr_shader.compile_file_and_link(
        "resources/shaders/pbr.vs"s,
        "resources/shaders/pbr.fs");

    etc_shader.compile_file_and_link(
        "resources/shaders/cubemap.vs"s, 
        "resources/shaders/equirectangular_to_cubemap.fs"s);

    irr_shader.compile_file_and_link(
        "resources/shaders/cubemap.vs"s, 
        "resources/shaders/irradiance_convolution.fs"s);

    pft_shader.compile_file_and_link(
        "resources/shaders/cubemap.vs"s, 
        "resources/shaders/prefilter.fs"s);

    brdf_shader.compile_file_and_link(
        "resources/shaders/brdf.vs"s, 
        "resources/shaders/brdf.fs"s);

    bg_shader.compile_file_and_link(
        "resources/shaders/background.vs"s, 
        "resources/shaders/background.fs"s);

    pick_shader.compile_file_and_link(
        "resources/shaders/pick.vs"s, 
        "resources/shaders/pick.fs"s);

    pbr_shader.use();
    pbr_shader.set_int("irradianceMap"s, 3);
    pbr_shader.set_int("prefilterMap"s, 4);
    pbr_shader.set_int("brdfLUT"s, 5);

    bg_shader.use();
    bg_shader.set_int("environmentMap"s, 0);
}

void ibl_renderer::set_texture()
{
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 views[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3(1.0f,  0.0f,  0.0f), 
            glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3(-1.0f,  0.0f,  0.0f), 
            glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3(0.0f,  1.0f,  0.0f), 
            glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3(0.0f, -1.0f,  0.0f), 
            glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3(0.0f,  0.0f,  1.0f), 
            glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3(0.0f,  0.0f, -1.0f), 
            glm::vec3(0.0f, -1.0f,  0.0f))
    };

    //初始化渲染缓冲区
    pbr_fbo.bind();
    auto rbo = freeze::make_render_buffer();
    rbo.bind();
    rbo.storage(512, 512, GL_DEPTH_COMPONENT24);
    rbo.unbind();
    pbr_fbo.attachement_render_buffer(rbo.ref(), GL_DEPTH_ATTACHMENT);
    pbr_fbo.unbind();

    //从外部加载hdr纹理
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf("resources/textures/hdr/newport_loft.hdr",
        &width, &height, &nrComponents, 0);
    if (data)
    {
        hdr_tex.bind();
        hdr_tex.set_image(GL_RGB16F, width, height, GL_RGB, GL_FLOAT, data);
        hdr_tex.set_wrap_s(GL_CLAMP_TO_EDGE);
        hdr_tex.set_wrap_t(GL_CLAMP_TO_EDGE);
        hdr_tex.set_min_filter(GL_LINEAR);
        hdr_tex.set_mag_filter(GL_LINEAR);
        hdr_tex.unbind();
        stbi_image_free(data);
    }

    //cubemap纹理,空
    cubemap_tex.bind();
    for (unsigned i = 0; i < 6; ++i)
    {
        cubemap_tex.set_image(i, GL_RGB16F, 512, 512, GL_RGB, GL_FLOAT, nullptr);
    }
    cubemap_tex.set_wrap_r(GL_CLAMP_TO_EDGE);
    cubemap_tex.set_wrap_s(GL_CLAMP_TO_EDGE);
    cubemap_tex.set_wrap_t(GL_CLAMP_TO_EDGE);
    cubemap_tex.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
    cubemap_tex.set_mag_filter(GL_LINEAR);
    cubemap_tex.unbind();

    //渲染到纹理 hdr --> cubemap
    etc_shader.use();
    etc_shader.set_int("equirectangularMap"s, 0);
    etc_shader.set_mat4("projection"s, projection);
    hdr_tex.active(0);
    hdr_tex.bind();

    glViewport(0, 0, 512, 512);

    pbr_fbo.bind();
    {
        for (auto i = 0; i < 6; ++i)
        {
            etc_shader.set_mat4("view"s, views[i]);
            pbr_fbo.attachement_color(cubemap_tex, 0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            cube_vao.bind();
            glDrawArrays(GL_TRIANGLES, 0, 36);
            cube_vao.unbind();
        }
    }
    pbr_fbo.unbind();

    cubemap_tex.bind();
    cubemap_tex.mipmap();
    cubemap_tex.unbind();

    //diffuse

    //辐照纹理
    irr_tex.bind();
    for (auto i = 0; i < 6; ++i)
    {
        irr_tex.set_image(i, GL_RGB16F, 32, 32, GL_RGB, GL_FLOAT, nullptr);
    }
    irr_tex.set_wrap_r(GL_CLAMP_TO_EDGE);
    irr_tex.set_wrap_s(GL_CLAMP_TO_EDGE);
    irr_tex.set_wrap_t(GL_CLAMP_TO_EDGE);
    irr_tex.set_min_filter(GL_LINEAR);
    irr_tex.set_mag_filter(GL_LINEAR);
    irr_tex.unbind();

    pbr_fbo.bind();
    rbo.bind();
    rbo.storage(32, 32, GL_DEPTH_COMPONENT24);
    rbo.unbind();
    pbr_fbo.unbind();

    //渲染到纹理 cubemap --> irradiance
    irr_shader.use();
    irr_shader.set_int("environmentMap"s, 0);
    irr_shader.set_mat4("projection"s, projection);
    cubemap_tex.active();
    cubemap_tex.bind();
    glViewport(0, 0, 32, 32);

    pbr_fbo.bind();
    {
        for (auto i = 0; i < 6; ++i)
        {
            irr_shader.set_mat4("view"s, views[i]);
            pbr_fbo.attachement_color(irr_tex, 0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            cube_vao.bind();
            glDrawArrays(GL_TRIANGLES, 0, 36);
            cube_vao.unbind();
        }
    }
    pbr_fbo.unbind();

    //specular

    //多级渐远预过滤纹理
    pft_tex.bind();
    for (auto i = 0; i < 6; ++i)
    {
        pft_tex.set_image(i, GL_RGB16F, 128, 128, GL_RGB, GL_FLOAT, nullptr);
    }
    pft_tex.set_wrap_r(GL_CLAMP_TO_EDGE);
    pft_tex.set_wrap_s(GL_CLAMP_TO_EDGE);
    pft_tex.set_wrap_t(GL_CLAMP_TO_EDGE);
    pft_tex.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
    pft_tex.set_mag_filter(GL_LINEAR);
    pft_tex.mipmap();
    pft_tex.unbind();

    pft_shader.use();
    pft_shader.set_int("environmentMap"s, 0);
    pft_shader.set_mat4("projection"s, projection);
    cubemap_tex.active();
    cubemap_tex.bind();

    //渲染到纹理 cubemap --> pre-filter
    pbr_fbo.bind();
    {
        unsigned maxMipLevels = 5;
        for (auto mip = 0u; mip < maxMipLevels; ++mip)
        {
            int mipWidth = 128 * std::pow(0.5, mip);
            int mipHeight = 128 * std::pow(0.5, mip);

            rbo.bind();
            rbo.storage(mipWidth, mipHeight, GL_DEPTH_COMPONENT24);
            rbo.unbind();

            glViewport(0, 0, mipWidth, mipHeight);

            float roughness = (float)mip / (float)(maxMipLevels - 1);
            pft_shader.set_float("roughness"s, roughness);

            for (unsigned int i = 0; i < 6; ++i)
            {
                pft_shader.set_mat4("view", views[i]);
                pbr_fbo.attachement_color(pft_tex, 0, 
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                cube_vao.bind();
                glDrawArrays(GL_TRIANGLES, 0, 36);
                cube_vao.unbind();
            }
        }
    }
    pbr_fbo.unbind();

    // pbr: generate a 2D LUT from the BRDF equations used.
    // brdf纹理，根据brdf参数方程生成二维查找纹理
    brdf_tex.bind();
    brdf_tex.set_image(GL_RG16F, 512, 512, GL_RG, GL_FLOAT, nullptr);
    brdf_tex.set_wrap_s(GL_CLAMP_TO_EDGE);
    brdf_tex.set_wrap_t(GL_CLAMP_TO_EDGE);
    brdf_tex.set_min_filter(GL_LINEAR);
    brdf_tex.set_mag_filter(GL_LINEAR);
    brdf_tex.unbind();


    pbr_fbo.bind();
    {
        rbo.bind();
        rbo.storage(512, 512, GL_DEPTH_COMPONENT24);
        pbr_fbo.attachement_color(brdf_tex);

        glViewport(0, 0, 512, 512);
        brdf_shader.use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        quad_vao.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        quad_vao.unbind();
    }
    pbr_fbo.unbind();

    //------------------------------------------------------------------------------
    auto proj = glm::perspective(glm::radians(scene_camera.Zoom), 
        (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    pbr_shader.use();
    pbr_shader.set_mat4("projection"s, proj);
    bg_shader.use();
    bg_shader.set_mat4("projection"s, proj);
    pick_shader.use();
    pick_shader.set_mat4("projection"s, proj);
    pick_shader.set_float("id"s, 123.0f);

    particle_shader.use();
    particle_shader.set_mat4("projection"s, proj);
}

void ibl_renderer::set_model()
{
    ufo_model.load("resources/objects/ufo/ufo.obj"s);
    ufo_model.setup(pbr_shader);

    pick_fbo.bind();
    auto rbo = freeze::make_render_buffer();
    rbo.bind();
    rbo.storage(SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_COMPONENT24);
    rbo.unbind();

    auto tex = freeze::make_texture2d();
    tex.bind();
    tex.set_image(GL_R16F, SCR_WIDTH, SCR_HEIGHT, GL_RED, GL_FLOAT, nullptr);
    tex.unbind();
    pick_fbo.attachement_color(tex);

    pick_fbo.attachement_render_buffer(rbo, GL_DEPTH_ATTACHMENT);
    pick_fbo.unbind();
}

void ibl_renderer::set_particle()
{
    float trans[200];
    int index = 0;
    for (auto y = -10; y < 10; y += 2)
    {
        for (auto x = -10; x < 10; x += 2)
        {
            float pos_x = (float)x * 0.1f + 0.1f;
            float pos_y = (float)y * 0.1f + 0.1f;
            trans[index++] = pos_x;
            trans[index++] = pos_y;
        }
    }

    particle_vao.bind();

    auto pv = freeze::make_vertex();
    auto pvbo = freeze::make_vertex_buffer();
    pvbo.bind();
    pvbo.copy_data(particle_vertices, sizeof(particle_vertices));
    pv.set(0, 3, 5, 0);
    pv.set(1, 2, 5, 3);
    pvbo.unbind();

    auto pvbo2 = freeze::make_vertex_buffer();
    pvbo2.bind();
    pvbo2.copy_data(trans, sizeof(trans));
    pv.set(2, 2, 2, 0);
    pv.divisor(2, 1);
    pvbo2.unbind();

    particle_vao.unbind();

    particle_shader.compile_file_and_link(
        "resources/shaders/particle.vs"s, 
        "resources/shaders/particle.fs"s);
    particle_shader.use();
    particle_shader.set_int("sprite"s, 0);

    particle_tex.bind();
    auto data = freeze::load_image_from_file("resources/textures/particle.png"s);
    particle_tex.set_image(data);
    particle_tex.mipmap();
    particle_tex.set_wrap_s(GL_REPEAT);
    particle_tex.set_wrap_t(GL_REPEAT);
    particle_tex.set_min_filter(GL_LINEAR);
    particle_tex.set_mag_filter(GL_LINEAR);
    particle_tex.unbind();
}

void ibl_renderer::draw()
{
    auto current_frame = freeze::window::get_time();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    auto view = scene_camera.get_view_matrix();
    auto model = glm::mat4{ 1.0f };
    model = glm::scale(glm::mat4{ 1.0f }, glm::vec3{ 0.03f });
    model = glm::rotate(model, glm::radians(-20.0f), glm::vec3{ 0.0f,1.0f,0.0f });
    model = glm::translate(model, glm::vec3{ 0.0f,-25.0f,0.0f });

    pbr_shader.use();
    pbr_shader.set_mat4("view"s, view);
    pbr_shader.set_mat4("model"s, model);
    pbr_shader.set_vec3("camPos"s, scene_camera.Position);

    irr_tex.active(3);
    irr_tex.bind();
    pft_tex.active(4);
    pft_tex.bind();
    brdf_tex.active(5);
    brdf_tex.bind();

    for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
    {
        glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
        pbr_shader.set_vec3("lightPositions[" + std::to_string(i) + "]", newPos);
        pbr_shader.set_vec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
    }

    picking();
    ufo_model.draw(pbr_shader);

    bg_shader.use();
    bg_shader.set_mat4("view"s, view);
    cubemap_tex.active();
    cubemap_tex.bind();
    cube_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    cube_vao.unbind();

    particle_shader.use();
    particle_shader.set_mat4("view"s, view);
    particle_shader.set_mat4("model"s, glm::mat4{ 1.0f });

    particle_tex.active();
    particle_tex.bind();

    particle_vao.bind();
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 100);
    particle_vao.unbind();

}

void ibl_renderer::picking()
{
    auto view = scene_camera.get_view_matrix();
    auto model = glm::mat4{ 1.0f };
    model = glm::translate(model, glm::vec3{ 0.0f,-0.8f,0.0f });
    model = glm::rotate(model, glm::radians(-30.0f), glm::vec3{ 0.0f,1.0f,0.0f });
    model = glm::scale(model, glm::vec3{ 0.03f });

    pick_shader.use();
    pick_shader.set_mat4("view"s, view);
    pick_shader.set_mat4("model"s, model);

    pick_fbo.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ufo_model.draw(pick_shader);
    pick_fbo.unbind();
}

int ibl_renderer::hit_test(int x, int y)
{
    pick_fbo.bind();
    pick_fbo.read_buffer(GL_COLOR_ATTACHMENT0);
    float v;
    pick_fbo.read_pixels(x, y, 1, 1, GL_RED, GL_FLOAT, (GLvoid*)&v);
    pick_fbo.read_buffer(GL_NONE);
    pick_fbo.unbind();
    return (int)v;
}

void ibl_renderer::on_hit(int hit)
{
}

void ibl_renderer::process_event(window_pointer window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 0.25f * delta_time;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        scene_camera.process_keyboard(freeze::camera_movement::FORWARD, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        scene_camera.process_keyboard(freeze::camera_movement::BACKWARD, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        scene_camera.process_keyboard(freeze::camera_movement::LEFT, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        scene_camera.process_keyboard(freeze::camera_movement::RIGHT, cameraSpeed);
}

void ibl_renderer::do_mouse_callback(freeze::window::pointer window, double xpos, double ypos)
{
    if (first_mouse)
    {
        last_x = xpos;
        last_y = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - last_x;
    float yoffset = last_y - ypos;

    last_x = xpos;
    last_y = ypos;

    scene_camera.process_mouse_movement(xoffset, yoffset);
}

void ibl_renderer::do_scroll_callback(freeze::window::pointer window, 
    double xoffset, double yoffset)
{
    scene_camera.process_mouse_scroll(yoffset);
}

void ibl_renderer::do_button_callback(freeze::window::pointer window,
    int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        auto hit = hit_test((int)xpos, SCR_HEIGHT - (int)ypos - 1);
        on_hit(hit);
    }
}
