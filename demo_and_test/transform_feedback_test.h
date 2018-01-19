#pragma once

#include "renderer_base.h"

#ifdef RENDERER
#undef RENDERER
#endif

#define RENDERER transform_feedback_test

#ifdef SCR_WIDTH
#undef SCR_WIDTH
#endif

#ifdef SCR_HEIGHT
#undef SCR_HEIGHT
#endif

#define SCR_WIDTH 800
#define SCR_HEIGHT 600


#define COUNTOF_WIDTH  40
#define COUNTOF_HEIGHT 30
#define PER_WIDTH ((float)((float)SCR_WIDTH/(float)COUNTOF_WIDTH))
#define PER_HEIGHT ((float)((float)SCR_HEIGHT/(float)COUNTOF_HEIGHT))
#define COUNTOF_MUL    (COUNTOF_WIDTH * COUNTOF_HEIGHT)
//#define NormalOf(L,X) ((float)((float)(2.0f*X)/((float)L)-1.0f))
//#define ONE_OF_WIDTH  NormalOf(COUNTOF_WIDTH,1)
//#define ONE_OF_HEIGHT NormalOf(COUNTOF_HEIGHT,1)

using high_clock = std::chrono::high_resolution_clock;

class transform_feedback_test
    : public renderer_base<transform_feedback_test>
    , public std::enable_shared_from_this<transform_feedback_test>
    , public renderer
{
public:
    transform_feedback_test();
    ~transform_feedback_test();

public:
    std::shared_ptr<freeze::freegl> gl;
    std::shared_ptr<freeze::window> win;
public:
    void do_init();
    virtual void draw() override;
    void process_event(window_pointer window);

private:
    float last_x;
    float last_y;
    bool first_mouse;
    float delta_time;
    std::chrono::time_point<high_clock> last_time;
    freeze::camera                      scene_camera;

private:
    freeze::program                     image_shader;
    std::vector<freeze::texture2d>      vec_image_tex;
    std::vector<freeze::tfo>            vec_tfo;
    freeze::vao                         vao;
    glm::mat4                           mat_model[COUNTOF_MUL];
};

