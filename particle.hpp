#pragma once
#ifndef PARTICLE_H
#define PARTICLE_H

#include <GL/glew.h>
#include <glm/glm.hpp>
//#include "obj_mesh.h"

struct ObjData;

class particle {
	public:
		particle(glm::mat4* trans, GLuint* normalTransformLoc, GLuint* modelTransformLoc, ObjData object);
	private:
};

#endif
