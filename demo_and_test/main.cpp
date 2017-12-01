//
//main.cpp

//#include "point_shadow_renderer.h"
//#include "dir_shadow_renderer.h"
#include "pbr_renderer.h"

//int main()
//{
//    auto freegl = freeze::make<glfw::glfw>(4, 3);
//
//    auto window = freeze::make<glfw::window>(SCR_WIDTH, SCR_HEIGHT);
//    window.make_context_current();
//    window.set_frame_buffer_size_callback(RENDERER::framebuffer_size_callback);
//    window.set_cursor_pos_callback(RENDERER::mouse_callback);
//    window.set_scroll_callback(RENDERER::scroll_callback);
//
//    freegl.load_loader();
//    freegl.swap_interval(1);
//        
//    auto render = std::make_shared<RENDERER>();
//    render->init();
//
//    while (!window.should_close())
//    {
//        render->process_event(window.get());
//
//        auto color = freeze::make_color(0.1f, 0.1f, 0.1f, 1.0f);
//        color.clear();
//        freeze::depth::clear();
//
//        render->draw();
//        
//        window.swap_buffers();
//        freegl.poll_events();
//    }
//
//	return 0;
//}

using Camera = freeze::camera;
using Shader = freeze::program;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
void renderSphere();
void renderCube();
void renderQuad();

// settings
//const unsigned int SCR_WIDTH = 1280;
//const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int quadVAO = 0;
unsigned int quadVBO;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														 // glfw window creation
														 // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	// set depth function to less than AND equal for skybox depth trick.
	glDepthFunc(GL_LEQUAL);
	// enable seamless cubemap sampling for lower mip levels in the pre-filter map.
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);


	// build and compile shaders
	// -------------------------
	auto shader_dir = "resources/shaders/"s;

	Shader pbrShader;
	pbrShader.compile_file(shader_dir+"pbr.vs", shader_dir + "pbr.fs");

	Shader equirectangularToCubemapShader;
	equirectangularToCubemapShader.compile_file(shader_dir + "cubemap.vs", shader_dir + "equirectangular_to_cubemap.fs");

	Shader irradianceShader;
	irradianceShader.compile_file(shader_dir + "cubemap.vs", shader_dir + "irradiance_convolution.fs");

	Shader prefilterShader;
	prefilterShader.compile_file(shader_dir + "cubemap.vs", shader_dir + "prefilter.fs");

	Shader brdfShader;
	brdfShader.compile_file(shader_dir + "brdf.vs", shader_dir + "brdf.fs");

	Shader backgroundShader;
	backgroundShader.compile_file(shader_dir + "background.vs", shader_dir + "background.fs");

	pbrShader.use();
	pbrShader.set_int("irradianceMap", 0);
	pbrShader.set_int("prefilterMap", 1);
	pbrShader.set_int("brdfLUT", 2);
	pbrShader.set_int("albedoMap", 3);
	pbrShader.set_int("normalMap", 4);
	pbrShader.set_int("metallicMap", 5);
	pbrShader.set_int("roughnessMap", 6);
	pbrShader.set_int("aoMap", 7);

	backgroundShader.use();
	backgroundShader.set_int("environmentMap", 0);

	// load PBR material textures
	// --------------------------
	// 生锈的铁球
	unsigned int ironAlbedoMap = loadTexture("resources/textures/pbr/rusted_iron/albedo.png");
	unsigned int ironNormalMap = loadTexture("resources/textures/pbr/rusted_iron/normal.png");
	unsigned int ironMetallicMap = loadTexture("resources/textures/pbr/rusted_iron/metallic.png");
	unsigned int ironRoughnessMap = loadTexture("resources/textures/pbr/rusted_iron/roughness.png");
	unsigned int ironAOMap = loadTexture("resources/textures/pbr/rusted_iron/ao.png");

	// 金色的球
	unsigned int goldAlbedoMap = loadTexture("resources/textures/pbr/gold/albedo.png");
	unsigned int goldNormalMap = loadTexture("resources/textures/pbr/gold/normal.png");
	unsigned int goldMetallicMap = loadTexture("resources/textures/pbr/gold/metallic.png");
	unsigned int goldRoughnessMap = loadTexture("resources/textures/pbr/gold/roughness.png");
	unsigned int goldAOMap = loadTexture("resources/textures/pbr/gold/ao.png");

	// 草球
	unsigned int grassAlbedoMap = loadTexture("resources/textures/pbr/grass/albedo.png");
	unsigned int grassNormalMap = loadTexture("resources/textures/pbr/grass/normal.png");
	unsigned int grassMetallicMap = loadTexture("resources/textures/pbr/grass/metallic.png");
	unsigned int grassRoughnessMap = loadTexture("resources/textures/pbr/grass/roughness.png");
	unsigned int grassAOMap = loadTexture("resources/textures/pbr/grass/ao.png");

	// 染上红漆的塑料球
	unsigned int plasticAlbedoMap = loadTexture("resources/textures/pbr/plastic/albedo.png");
	unsigned int plasticNormalMap = loadTexture("resources/textures/pbr/plastic/normal.png");
	unsigned int plasticMetallicMap = loadTexture("resources/textures/pbr/plastic/metallic.png");
	unsigned int plasticRoughnessMap = loadTexture("resources/textures/pbr/plastic/roughness.png");
	unsigned int plasticAOMap = loadTexture("resources/textures/pbr/plastic/ao.png");

	// 砖混球
	unsigned int wallAlbedoMap = loadTexture("resources/textures/pbr/wall/albedo.png");
	unsigned int wallNormalMap = loadTexture("resources/textures/pbr/wall/normal.png");
	unsigned int wallMetallicMap = loadTexture("resources/textures/pbr/wall/metallic.png");
	unsigned int wallRoughnessMap = loadTexture("resources/textures/pbr/wall/roughness.png");
	unsigned int wallAOMap = loadTexture("resources/textures/pbr/wall/ao.png");

	// 表示4个点光源
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

	// pbr: setup framebuffer
	// ----------------------
	unsigned int captureFBO;
	unsigned int captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	// pbr: load the HDR environment map
	// ---------------------------------
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float *data = stbi_loadf("resources/textures/hdr/newport_loft.hdr", &width, &height, &nrComponents, 0);
	unsigned int hdrTexture;
	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load HDR image." << std::endl;
	}

	// pbr: 立方体贴图 setup cubemap to render to and attach to framebuffer
	// ---------------------------------------------------------
	unsigned int envCubemap;
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
	// ----------------------------------------------------------------------------------------------
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	// pbr: convert HDR equirectangular environment map to cubemap equivalent
	// ----------------------------------------------------------------------
	equirectangularToCubemapShader.use();
	equirectangularToCubemapShader.set_int("equirectangularMap", 0);
	equirectangularToCubemapShader.set_mat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);

	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		equirectangularToCubemapShader.set_mat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
	// --------------------------------------------------------------------------------
	unsigned int irradianceMap;
	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
	// -----------------------------------------------------------------------------
	irradianceShader.use();
	irradianceShader.set_int("environmentMap", 0);
	irradianceShader.set_mat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		irradianceShader.set_mat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
	// --------------------------------------------------------------------------------
	unsigned int prefilterMap;
	glGenTextures(1, &prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minifcation filter to mip_linear 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
	// ----------------------------------------------------------------------------------------------------
	prefilterShader.use();
	prefilterShader.set_int("environmentMap", 0);
	prefilterShader.set_mat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = (int)(128 * std::pow(0.5, mip));
		unsigned int mipHeight = (int)(128 * std::pow(0.5, mip));
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader.set_float("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterShader.set_mat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderCube();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// pbr: generate a 2D LUT from the BRDF equations used.
	// ----------------------------------------------------
	unsigned int brdfLUTTexture;
	glGenTextures(1, &brdfLUTTexture);

	// pre-allocate enough memory for the LUT texture.
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	//---------------------------------------------------------------
	float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};
	// setup plane VAO
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

	//glGenBuffers(1, &quadVBO);
	//glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	auto vbo = freeze::make_vertex_buffer();
	freeze::quickly_set_vbo(vbo,quadVertices, sizeof(quadVertices), 3, 2);
	//---------------------------------------------------------------

	glViewport(0, 0, 512, 512);
	brdfShader.use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// initialize static shader uniforms before rendering
	// --------------------------------------------------
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	pbrShader.use();
	pbrShader.set_mat4("projection", projection);
	backgroundShader.use();
	backgroundShader.set_mat4("projection", projection);

	// then before rendering, configure the viewport to the original framebuffer's screen dimensions
	int scrWidth, scrHeight;
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render scene, supplying the convoluted irradiance map to the final shader.
		// ------------------------------------------------------------------------------------------
		pbrShader.use();
		glm::mat4 model;
		glm::mat4 view = camera.get_view_matrix();
		pbrShader.set_mat4("view", view);
		pbrShader.set_vec3("camPos", camera.Position);

		// bind pre-computed IBL data
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

		// rusted iron
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, ironAlbedoMap);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, ironNormalMap);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, ironMetallicMap);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, ironRoughnessMap);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, ironAOMap);

		model = glm::mat4{ 1.0f };
		model = glm::translate(model, glm::vec3(-5.0, 0.0, 2.0));
		pbrShader.set_mat4("model", model);
		renderSphere();

		// gold
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, goldAlbedoMap);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, goldNormalMap);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, goldMetallicMap);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, goldRoughnessMap);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, goldAOMap);

		model = glm::mat4{ 1.0f };
		model = glm::translate(model, glm::vec3(-3.0, 0.0, 2.0));
		pbrShader.set_mat4("model", model);
		renderSphere();

		// grass
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, grassAlbedoMap);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, grassNormalMap);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, grassMetallicMap);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, grassRoughnessMap);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, grassAOMap);

		model = glm::mat4{ 1.0f };
		model = glm::translate(model, glm::vec3(-1.0, 0.0, 2.0));
		pbrShader.set_mat4("model", model);
		renderSphere();

		// plastic
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, plasticAlbedoMap);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, plasticNormalMap);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, plasticMetallicMap);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, plasticRoughnessMap);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, plasticAOMap);

		model = glm::mat4{ 1.0f };
		model = glm::translate(model, glm::vec3(1.0, 0.0, 2.0));
		pbrShader.set_mat4("model", model);
		renderSphere();

		// wall
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, wallAlbedoMap);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, wallNormalMap);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, wallMetallicMap);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, wallRoughnessMap);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, wallAOMap);

		model = glm::mat4{ 1.0f };
		model = glm::translate(model, glm::vec3(3.0, 0.0, 2.0));
		pbrShader.set_mat4("model", model);
		renderSphere();

		// render light source (simply re-render sphere at light positions)
		// this looks a bit off as we use the same shader, but it'll make their positions obvious and 
		// keeps the codeprint small.
		for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
		{
			glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
			//newPos = lightPositions[i];
			pbrShader.set_vec3("lightPositions[" + std::to_string(i) + "]", newPos);
			pbrShader.set_vec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

			model = glm::mat4{ 1.0f };
			model = glm::translate(model, newPos);
			model = glm::scale(model, glm::vec3(0.5f));
			pbrShader.set_mat4("model", model);
			renderSphere();
		}

		// render skybox (render as last to prevent overdraw)
		backgroundShader.use();

		backgroundShader.set_mat4("view", view);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); // display irradiance map
		//glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap); // display prefilter map
		renderCube();

		// render BRDF map to screen
		//brdfShader.use();
		//renderQuad();


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.process_keyboard(freeze::camera_movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.process_keyboard(freeze::camera_movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.process_keyboard(freeze::camera_movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.process_keyboard(freeze::camera_movement::RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = float(xpos - lastX);
	float yoffset = float(lastY - ypos); // reversed since y-coordinates go from bottom to top

	lastX = (float)xpos;
	lastY = (float)ypos;

	camera.process_mouse_movement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.process_mouse_scroll((float)yoffset);
}

// renders (and builds at first invocation) a sphere
// -------------------------------------------------
unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
	if (sphereVAO == 0)
	{
		glGenVertexArrays(1, &sphereVAO);

		unsigned int vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359f;
		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y       * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y       * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		indexCount = indices.size();

		std::vector<float> data;
		for (int i = 0; i < (int)positions.size(); ++i)
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
		glBindVertexArray(sphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		GLsizei stride = (3 + 2 + 3) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
	}

	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
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
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
//unsigned int quadVAO = 0;
//unsigned int quadVBO;
void renderQuad()
{
	//if (quadVAO == 0)
	//{
	//	float quadVertices[] = {
	//		// positions        // texture Coords
	//		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	//		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	//		1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
	//		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	//	};
	//	// setup plane VAO
	//	glGenVertexArrays(1, &quadVAO);
	//	glBindVertexArray(quadVAO);

	//	//glGenBuffers(1, &quadVBO);
	//	//glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	//	//glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	//	//glEnableVertexAttribArray(0);
	//	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	//	//glEnableVertexAttribArray(1);
	//	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	//	auto vbo = freeze::make_vertex_buffer();
	//	vbo.bind();
	//	vbo.copy_data(quadVertices, sizeof(quadVertices));
	//	auto vertex = freeze::make_vertex();
	//	vertex.set(0, 3, GL_FLOAT, 5 * sizeof(float), 0);
	//	vertex.set(1, 2, GL_FLOAT, 5 * sizeof(float), 3 * sizeof(float));
	//	vbo.unbind();
	//	quadVBO = vbo.ref();
	//}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}