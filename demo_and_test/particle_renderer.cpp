#include "particle_renderer.h"

struct Particle
{
	float x,y,z;
	float vx,vy,vz;
	float age;
};

#define NUMBERS 1000

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

    Particles[0].x = 0.0f;
    Particles[0].y = 0.0f;
	Particles[0].z = 0.0f;
    Particles[0].vx = 0.01f;
	Particles[0].vy = 0.01f;
	Particles[0].age = 0.0f;
	
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

    const GLchar* Varyings[] = {
        "GS.Position",
        "GS.Velocity",
        "GS.Age",
    };
    partical_shader.set_varyings(Varyings, 3);
    partical_shader.link();

    //uniform float gDeltaTime;
    //uniform float gTime;
    //uniform sampler1D gRandomTexture;

	partical_shader.use();
	partical_shader.set_int("gRandomTexture"s, 3);

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
	partical_shader.set_float("gDeltaTime"s, delta_time);
	partical_shader.set_float("gTime"s, ftimes);

	random_tex.active(3);
	random_tex.bind();

	vbo[current].bind();
    freeze::vertex::set_enable(0, 3, sizeof(Particle) / sizeof(float), 0);
    freeze::vertex::set_enable(1, 3, sizeof(Particle) / sizeof(float), 3);
    freeze::vertex::set_enable(2, 1, sizeof(Particle) / sizeof(float), 6);
    
    tfb[next].bind();

	glEnable(GL_RASTERIZER_DISCARD);
	freeze::tfo::begin(GL_POINTS);    
	if (first_render)
    {
		glDrawArrays(GL_POINTS, 0, 1);
        first_render = false;
	}
	else
    {
		tfb[current].draw(GL_POINTS);
	}
    freeze::tfo::end();
	glDisable(GL_RASTERIZER_DISCARD);

}

void particle_renderer::draw_particle()
{
	auto projection = glm::perspective(glm::radians(scene_camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	auto view = scene_camera.get_view_matrix();

	billboard_shader.use();
	billboard_shader.set_mat4("gVP"s, projection * view);
	billboard_shader.set_vec3("gCameraPos"s, scene_camera.Position);

	billboard_tex.active();
	billboard_tex.bind();

	vbo[next].bind();
    freeze::vertex::set_enable(0, 3, sizeof(Particle)/sizeof(float), 0);

    tfb[next].bind();
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
