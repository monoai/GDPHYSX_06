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

class SpringParticle : public particleForceGen
{

	particle* other;

	float springConstant;

	float restLength;

public:

	SpringParticle(particle* other, float springConstant, float restLength);

	virtual void updateForce(particle* particle, float duration);

};

class AnchoredSpring : public particleForceGen
{
	glm::vec3* anchor;

	float springConstant;

	float restLength;

public:

	AnchoredSpring(glm::vec3* anchor, float springConstant, float restLength);

	virtual void updateForce(particle* particle, float duration);

};

class ElasticBungee : public particleForceGen
{
	particle* other;

	float springConstant;

	float restLength;

public:

	ElasticBungee(particle* other, float springConstant, float restLength);

	virtual void updateForce(particle* particle, float duration);

};

#endif
