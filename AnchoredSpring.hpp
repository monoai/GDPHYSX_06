#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include "particlefgen.hpp"
class AnchoredSpring : public particleForceGen
{
	glm::vec3* anchor;

	float springConstant;

	float restLength;

public:

	AnchoredSpring(glm::vec3 *anchor, float springConstant, float restLength);

	virtual void updateForce(particle *particle, float duration);

};

