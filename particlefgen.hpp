#pragma once
#ifndef PARTICLE_FORCE_GEN_H
#define PARTICLE_FORCE_GEN_H

#include "particle.hpp"

class particleForceGen {
	public:
		virtual void updateForce(particle *particle, float dT) = 0;
};

class particleGravity : public particleForceGen {
	public:
		particleGravity(glm::vec3 &gravity);
		virtual void updateForce(particle* particle, float dT);

		glm::vec3 gravity = glm::vec3(0.0f);
};

class particleDrag : public particleForceGen {
	public:
		particleDrag(float k1, float k2);
		virtual void updateForce(particle* particle, float dT);

		float k1;
		float k2;
};

class particleSpring : public particleForceGen
{
	public:
		particleSpring(particle* other, float springConstant, float restLength);
		virtual void updateForce(particle* particle, float duration);

		particle* other;
		float springConstant;
		float restLength;
};

class particleAnchoredSpring : public particleForceGen
{
	public:
		particleAnchoredSpring(glm::vec3* anchor, float springConstant, float restLength);
		virtual void updateForce(particle* particle, float duration);

		glm::vec3* anchor;
		float springConstant;
		float restLength;
};

class particleElasticBungee : public particleForceGen
{
	public:
		particleElasticBungee(particle* other, float springConstant, float restLength);
		virtual void updateForce(particle* particle, float duration);

		particle* other;
		float springConstant;
		float restLength;
};

#endif
