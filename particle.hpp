#pragma once
#ifndef PARTICLE_H
#define PARTICLE_H

#include "GL/glew.h"
#include <glm/glm.hpp>

class particle {
	public:
		particle(glm::mat4* trans, GLuint* normalTransformLoc, GLuint* modelTransformLoc, ObjData object);
	private:
};

#endif /* PARTICLE_H */
