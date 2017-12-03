#include "ibl_renderer.h"


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

    auto current_frame = glfw::detail::get_time();

    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    freeze::depth::enable();
    freeze::depth::test_less_equal();
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

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
    auto cv = freeze::make_vertex();
    cv.set(0, 3, GL_FLOAT, 8 * sizeof(float), 0);
    cv.set(1, 3, GL_FLOAT, 8 * sizeof(float), 3 * sizeof(float));
    cv.set(2, 2, GL_FLOAT, 8 * sizeof(float), 6 * sizeof(float));
    cvbo.unbind();
    cube_vao.unbind();

    quad_vao.bind();
    auto qvbo = freeze::make_vertex_buffer();
    qvbo.bind();
    qvbo.copy_data(quad_vertices, sizeof(quad_vertices));
    auto qv = freeze::make_vertex();
    qv.set(0, 3, GL_FLOAT, 5 * sizeof(float),0);
    qv.set(1, 2, GL_FLOAT, 5 * sizeof(float), 3 * sizeof(float));
    qvbo.unbind();
    quad_vao.unbind();
}

void ibl_renderer::set_shader()
{
    pbr_shader.compile_file("resources/shaders/pbr.vs"s, "resources/shaders/pbr.fs");
    etc_shader.compile_file("resources/shaders/cubemap.vs"s, "resources/shaders/equirectangular_to_cubemap.fs"s);
    irr_shader.compile_file("resources/shaders/cubemap.vs"s, "resources/shaders/irradiance_convolution.fs"s);
    pft_shader.compile_file("resources/shaders/cubemap.vs"s, "resources/shaders/prefilter.fs"s);
    brdf_shader.compile_file("resources/shaders/brdf.vs"s, "resources/shaders/brdf.fs"s);
    bg_shader.compile_file("resources/shaders/background.vs"s,"resources/shaders/background.fs"s);

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
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    pbr_fbo.bind();
    auto rbo = freeze::make_render_buffer();
    rbo.bind();
    rbo.storage(512, 512, GL_DEPTH_COMPONENT24);
    pbr_fbo.attachement_render_buffer(rbo.ref(), GL_DEPTH_ATTACHMENT);

    //pbr: load the HDR environment map
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf("resources/textures/hdr/newport_loft.hdr",
        &width,&height,&nrComponents,0);
    if (data)
    {
        hdr_tex.bind();
        hdr_tex.set_image(GL_RGB16F, width, height, GL_RGB, GL_FLOAT, data);
        hdr_tex.set_wrap_s(GL_CLAMP_TO_EDGE);
        hdr_tex.set_wrap_t(GL_CLAMP_TO_EDGE);
        hdr_tex.set_min_filter(GL_LINEAR);
        hdr_tex.set_mag_filter(GL_LINEAR);
        stbi_image_free(data);
    }

    //pbr: setup cubemap to render to and attach to framebuffer
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

    //pbr: convert HDR equirectangular environment map to cubemap equivalent
    etc_shader.use();
    etc_shader.set_int("equirectangularMap"s, 0);
    etc_shader.set_mat4("projection"s, projection);
    hdr_tex.active(0);
    hdr_tex.bind();

    glViewport(0, 0, 512, 512);
    pbr_fbo.bind();
    for (auto i = 0; i < 6; ++i)
    {
        etc_shader.set_mat4("view"s, views[i]);
        pbr_fbo.attachement_color(cubemap_tex, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cube_vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        cube_vao.unbind();
    }
    pbr_fbo.unbind();

    cubemap_tex.bind();
    cubemap_tex.mipmap();

    //pbr: create an irradiance cubemap,and re-scale capture FBO to irradiance scal
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

    pbr_fbo.bind();
    rbo.bind();
    rbo.storage(32, 32, GL_DEPTH_COMPONENT24);

    irr_shader.use();
    irr_shader.set_int("environmentMap"s, 0);
    irr_shader.set_mat4("projection"s, projection);
    cubemap_tex.active();
    cubemap_tex.bind();
    glViewport(0, 0, 32, 32);
    pbr_fbo.bind();
    for (auto i = 0; i < 6; ++i)
    {
        irr_shader.set_mat4("view"s, views[i]);
        pbr_fbo.attachement_color(irr_tex, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cube_vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        cube_vao.unbind();
    }
    pbr_fbo.unbind();

    //pbr: create a pre-filter cubemap,and re-scale capture FBO to pre-filter scale
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

    pft_shader.use();
    pft_shader.set_int("environmentMap"s, 0);
    pft_shader.set_mat4("projection"s, projection);
    cubemap_tex.active();
    cubemap_tex.bind();
    pbr_fbo.bind();
    unsigned maxMipLevels = 5;
    for (auto mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = 128 * std::pow(0.5, mip);
        unsigned int mipHeight = 128 * std::pow(0.5, mip);
        rbo.bind();
        rbo.storage(mipWidth, mipHeight,GL_DEPTH_COMPONENT24);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        pft_shader.set_float("roughness"s, roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            pft_shader.set_mat4("view", views[i]);
            pbr_fbo.attachement_color(pft_tex, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            cube_vao.bind();
            glDrawArrays(GL_TRIANGLES, 0, 36);
            cube_vao.unbind();
        }
    }
    pbr_fbo.unbind();

    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    brdf_tex.bind();
    brdf_tex.set_image(GL_RG16F, 512, 512, GL_RG, GL_FLOAT, nullptr);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    brdf_tex.set_wrap_s(GL_CLAMP_TO_EDGE);
    brdf_tex.set_wrap_t(GL_CLAMP_TO_EDGE);
    brdf_tex.set_min_filter(GL_LINEAR);
    brdf_tex.set_mag_filter(GL_LINEAR);
    

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    pbr_fbo.bind();
    rbo.bind();
    rbo.storage(512, 512, GL_DEPTH_COMPONENT24);
    pbr_fbo.attachement_color(brdf_tex);

    glViewport(0, 0, 512, 512);
    brdf_shader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    quad_vao.bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    quad_vao.unbind();

    pbr_fbo.unbind();

    auto proj = glm::perspective(glm::radians(scene_camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    pbr_shader.use();
    pbr_shader.set_mat4("projection"s, proj);
    bg_shader.use();
    bg_shader.set_mat4("projection"s, proj);
}

void ibl_renderer::set_model()
{
    ufo_model.load("resources/objects/ufo/ufo.obj"s);
    ufo_model.setup(pbr_shader);
}

void ibl_renderer::draw()
{
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
        //newPos = lightPositions[i];
        pbr_shader.set_vec3("lightPositions[" + std::to_string(i) + "]", newPos);
        pbr_shader.set_vec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
    }

    ufo_model.draw(pbr_shader);

    bg_shader.use();
    bg_shader.set_mat4("view"s, view);
    cubemap_tex.active();
    cubemap_tex.bind();
    cube_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    cube_vao.unbind();
}

void ibl_renderer::process_event(window_pointer window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * delta_time;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        scene_camera.process_keyboard(freeze::camera_movement::FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        scene_camera.process_keyboard(freeze::camera_movement::BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        scene_camera.process_keyboard(freeze::camera_movement::LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        scene_camera.process_keyboard(freeze::camera_movement::RIGHT, delta_time);
}

void ibl_renderer::do_mouse_callback(double xpos, double ypos)
{
    if (first_mouse)
    {
        last_x = xpos;
        last_y = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - last_x;
    float yoffset = last_y - ypos; // reversed since y-coordinates go from bottom to top

    last_x = xpos;
    last_y = ypos;

    scene_camera.process_mouse_movement(xoffset, yoffset);
}

void ibl_renderer::do_scroll_callback(double xoffset, double yoffset)
{
    scene_camera.process_mouse_scroll(yoffset);
}
