#include "particle_renderer.h"

struct Particle
{
	float x,y,z;
	float vx,vy,vz;
	float age;
	float type;
};

#define NUMBERS 1000

#define PARTICLE_TYPE_LAUNCHER 0.0f
#define PARTICLE_TYPE_SHELL    1.0f
#define PARTICLE_TYPE_SHELL_2  2.0f

particle_renderer::particle_renderer()
	: scene_camera{0.0f,0.0f,3.0f}
	, last_x(SCR_WIDTH / 2.0f)
	, last_y(SCR_HEIGHT / 2.0f)
	, first_mouse(true)
	, delta_time(0.0f)
	, last_frame(0.0f)
	, ftimes{0.0f}
{

}


particle_renderer::~particle_renderer()
{
	
}

void particle_renderer::do_init()
{
	ms_instance = shared_from_this();

	Particle Particles[NUMBERS];
	memset(Particles, 0, sizeof(Particles));

	Particles[0].z = 1.0f;
	Particles[0].vy = 0.0001f;
	Particles[0].age = 0.0f;
	Particles[0].type = PARTICLE_TYPE_LAUNCHER;
	
    vao.bind();
	for (auto i = 0; i < 2; ++i)
	{
        vbo[i].bind();
        vbo[i].copy_data(Particles, sizeof(Particles), GL_DYNAMIC_DRAW);

        tfb[i].bind();
        tfb[i].bind_base(vbo[i], 0);
        //tfb[i].unbind();

        //vbo[i].unbind();
	}
    //vao.unbind();

	billboard_shader.compile_file_and_link(
        "resources/shaders/billboard.vs"s,
		"resources/shaders/billboard.fs"s,
		"resources/shaders/billboard.gs"s);
	//uniform mat4 gVP;
	//uniform vec3 gCameraPos;
	//uniform float gBillboardSize;
	//uniform sampler2D gColorMap; 
	billboard_shader.use();
	billboard_shader.set_int("gColorMap"s, 0);
	billboard_shader.set_float("gBillboardSize"s, 0.1f);

	auto data = freeze::load_image_from_file("resources/textures/fireworks_red.jpg"s);
	billboard_tex.bind();
	billboard_tex.set_image(data);
	//billboard_tex.set_wrap_s(GL_REPEAT);
	//billboard_tex.set_wrap_t(GL_REPEAT);
	billboard_tex.set_min_filter(GL_LINEAR);
	billboard_tex.set_mag_filter(GL_LINEAR);
	billboard_tex.unbind();

	partical_shader.compile_file(
        "resources/shaders/particle.vs"s,
		"resources/shaders/particle.fs"s,
		"resources/shaders/particle.gs"s);

	const GLchar* Varyings[4];
	Varyings[0] = "Type1";
	Varyings[1] = "Position1";
	Varyings[2] = "Velocity1";
	Varyings[3] = "Age1";

	glTransformFeedbackVaryings(partical_shader, 4, Varyings, GL_INTERLEAVED_ATTRIBS);

    partical_shader.link();

	//uniform float gDeltaTimeMillis;
	//uniform float gTime;
	//uniform float gLauncherLifetime;
	//uniform float gShellLifetime;
	//uniform float gSecondaryShellLifetime;
	//uniform sampler1D gRandomTexture;

	partical_shader.use();
	partical_shader.set_int("gRandomTexture"s, 3);
	partical_shader.set_float("gLauncherLifetime"s, 100.0f);
	partical_shader.set_float("gShellLifetime"s, 10000.0f);
	partical_shader.set_float("gSecondaryShellLifetime"s, 2500.0f);

	glm::vec3 pData[NUMBERS];
	for (auto i = 0; i < NUMBERS; ++i)
	{
		pData[i].x = ((float)std::rand() / RAND_MAX);
		pData[i].y = ((float)std::rand() / RAND_MAX);
		pData[i].z = ((float)std::rand() / RAND_MAX);
	}
	random_tex.bind();
	random_tex.set_image(GL_RGB, NUMBERS, GL_RGB, GL_FLOAT, pData);
	random_tex.set_wrap_s(GL_REPEAT);
	random_tex.set_min_filter(GL_LINEAR);
	random_tex.set_mag_filter(GL_LINEAR);
	random_tex.unbind();

}

void particle_renderer::update()
{
	partical_shader.use();
	partical_shader.set_float("gDeltaTimeMillis"s, delta_time);
	partical_shader.set_float("gTime"s, ftimes);

	random_tex.active(3);
	random_tex.bind();

    vao.bind();
	vbo[current].bind();

    freeze::vertex::set_enable(0, 3, sizeof(Particle) / sizeof(float), 0);
    freeze::vertex::set_enable(1, 3, sizeof(Particle) / sizeof(float), 3);
    freeze::vertex::set_enable(2, 1, sizeof(Particle) / sizeof(float), 6);
    freeze::vertex::set_enable(3, 1, sizeof(Particle) / sizeof(float), 7);

    tfb[next].bind();

	glEnable(GL_RASTERIZER_DISCARD);
	freeze::transform_feedback::begin(GL_POINTS);    
	if (first_render)
    {
		glDrawArrays(GL_POINTS, 0, 1);
        first_render = false;
	}
	else
    {
		tfb[current].draw(GL_POINTS);
	}
    freeze::transform_feedback::end();
	glDisable(GL_RASTERIZER_DISCARD);

}

void particle_renderer::draw_particle()
{
	auto projection = glm::perspective(glm::radians(scene_camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	auto view = scene_camera.get_view_matrix();

	billboard_shader.use();
	//billboard_shader.set_mat4("projection"s, projection);
	//billboard_shader.set_mat4("view"s, view);
	billboard_shader.set_mat4("gVP"s, projection * view);
	billboard_shader.set_vec3("gCameraPos"s, scene_camera.Position);

	billboard_tex.active();
	billboard_tex.bind();

    vao.bind();
	vbo[next].bind();
    freeze::vertex::set_enable(0, 3, 0, 0);

    tfb[next].draw(GL_POINTS);
}

void particle_renderer::draw()
{
	auto current_frame = freeze::window::get_time();
	delta_time = current_frame - last_frame;
	last_frame = current_frame;
	ftimes += delta_time;

	update();
	draw_particle();

    std::swap(current, next);
}

void particle_renderer::process_event(window_pointer window)
{
}
