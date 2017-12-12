#include "particle_renderer.h"

struct Particle
{
	float type;
	float x,y,z;
	float vx,vy,vz;
	float age;
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
	if (m_transformFeedback[0] != 0) {
		glDeleteTransformFeedbacks(2, m_transformFeedback);
	}

	if (m_particleBuffer[0] != 0) {
		glDeleteBuffers(2, m_particleBuffer);
	}
}

void particle_renderer::do_init()
{
	ms_instance = shared_from_this();

	Particle Particles[NUMBERS];
	memset(Particles, 0, sizeof(Particles));

	Particles[0].type = PARTICLE_TYPE_LAUNCHER;
	Particles[0].z = 1.0f;
	Particles[0].vy = 0.0001f;
	Particles[0].age = 0.0f;
	
	glGenVertexArrays(2, vao);
	glGenTransformFeedbacks(2, m_transformFeedback);
	glGenBuffers(2, m_particleBuffer);

	for (auto i = 0; i < 2; ++i)
	{
		glBindVertexArray(vao[i]);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[i]);
		glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particles), Particles, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBuffer[i]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, m_particleBuffer[i]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, m_particleBuffer[i]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 3, m_particleBuffer[i]);
	}

	billboard_shader.compile_file("resources/shaders/billboard.vs"s,
		"resources/shaders/billboard.fs"s,
		"resources/shaders/billboard.gs"s);
	//uniform mat4 gVP;
	//uniform vec3 gCameraPos;
	//uniform float gBillboardSize;
	//uniform sampler2D gColorMap; 
	billboard_shader.use();
	billboard_shader.set_int("gColorMap"s, 0);
	billboard_shader.set_float("gBillboardSize"s, 0.01f);

	auto data = freeze::load_image_from_file("resources/textures/fireworks_red.jpg"s);
	billboard_tex.bind();
	billboard_tex.set_image(data);
	billboard_tex.set_wrap_s(GL_REPEAT);
	billboard_tex.set_wrap_t(GL_REPEAT);
	billboard_tex.set_min_filter(GL_NEAREST);
	billboard_tex.set_mag_filter(GL_NEAREST);
	billboard_tex.unbind();

	partical_shader.compile_file("resources/shaders/particle.vs"s,
		"resources/shaders/particle.fs"s,
		"resources/shaders/particle.gs"s);

	const GLchar* Varyings[4];
	Varyings[0] = "Type1";
	Varyings[1] = "Position1";
	Varyings[2] = "Velocity1";
	Varyings[3] = "Age1";

	glTransformFeedbackVaryings(partical_shader, 4, Varyings, GL_INTERLEAVED_ATTRIBS);

	//uniform float gDeltaTimeMillis;
	//uniform float gTime;
	//uniform sampler1D gRandomTexture;
	//uniform float gLauncherLifetime;
	//uniform float gShellLifetime;
	//uniform float gSecondaryShellLifetime;

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

	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(vao[m_currVB]);
	glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currVB]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[m_currTFB]);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);                          // type
	assert_error();
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4);         // position
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)16);        // velocity
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)28);          // lifetime

	glBeginTransformFeedback(GL_POINTS);
	assert_error();
	if (m_isFirst) {
		glDrawArrays(GL_POINTS, 0, 1);

		m_isFirst = false;
	}
	else {
		glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currVB]);
	}

	glEndTransformFeedback();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);

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

	glDisable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(vao[m_currTFB]);
	glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currTFB]);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4);  // position

	glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currTFB]);

	glDisableVertexAttribArray(0);
}

void particle_renderer::draw()
{
	auto current_frame = freeze::window::get_time();
	delta_time = current_frame - last_frame;
	last_frame = current_frame;
	ftimes += delta_time;

	update();
	draw_particle();

	m_currVB = m_currTFB;
	m_currTFB = (m_currTFB + 1) & 0x1;
}

void particle_renderer::process_event(window_pointer window)
{
}
