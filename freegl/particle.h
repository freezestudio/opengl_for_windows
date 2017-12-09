//
// Created by freezestudio on 2017/12/9.
//

#ifndef FREEGL_PARTICLE_H
#define FREEGL_PARTICLE_H
/////////////////////////////////////////////////////////////////////////

namespace freeze
{
	struct particle
	{
		float x, y, z;    //Position
		float vx, vy, vz; //Velocity
		float r, g, b, a; //Color
		float hp;         //Health Point
	};
}


/////////////////////////////////////////////////////////////////////////
#endif //FREEGL_PARTICLE_H