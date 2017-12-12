//
// Created by freezestudio on 2017/10/26.
//

#ifndef FREEGL_COLOR_H
#define FREEGL_COLOR_H

namespace freeze
{
    struct color
    {
		static void clear(float r, float g, float b, float a = 1.0f)
		{
			glClearColor(r, g, b, a);
			assert_error();
			glClear(GL_COLOR_BUFFER_BIT);
			assert_error();
		}
    };
}

#endif //FREEGL_COLOR_H
