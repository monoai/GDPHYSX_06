#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include "particlefgen.hpp"
class ElasticBungee : public particleForceGen
{
	particle* other;

	float springConstant;

	float restLength;

public:

	ElasticBungee(particle *other, float springConstant, float restLength);

	virtual void updateForce(particle* particle, float duration);

};
