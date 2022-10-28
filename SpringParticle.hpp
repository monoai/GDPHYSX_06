#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include "particlefgen.hpp"
class SpringParticle : public particleForceGen
{

	particle *other;

	float springConstant;

	float restLength;

public:

		SpringParticle(particle* other, float springConstant, float restLength);

		virtual void updateForce(particle* particle, float duration);

};

