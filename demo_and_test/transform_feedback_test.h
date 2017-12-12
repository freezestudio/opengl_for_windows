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

class transform_feedback_test
    : public renderer_base<transform_feedback_test>
    , public std::enable_shared_from_this<transform_feedback_test>
    , public renderer
{
public:
    transform_feedback_test();
    ~transform_feedback_test();
public:
    void do_init();
    virtual void draw() override;
    void update();
    void draw_particle();
    void process_event(window_pointer window);
};

