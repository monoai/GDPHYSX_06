#pragma once
#ifndef PARTICLE_FORCE_GEN_H
#define PARTICLE_FORCE_GEN_H

#include "particle.hpp"

class particleForceGen {
	public:
		void updateForce(particle *particle, float dT);
};

class particleGravity : public particleForceGen {
	public:
		particleGravity(glm::vec3 &gravity);
		void updateForce(particle* particle, float dT);
		glm::vec3 gravity = glm::vec3(0.0f);
};

class particleDrag : public particleForceGen {
	public:
		particleDrag(float k1, float k2);
		void updateForce(particle* particle, float dT);

		float k1;
		float k2;
};

#endif
