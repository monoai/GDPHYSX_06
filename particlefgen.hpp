#pragma once
#ifndef PARTICLE_FORCE_GEN_H
#define PARTICLE_FORCE_GEN_H

#include "particle.hpp"

#include <memory>

class particleForceGen {
	public:
		virtual void updateForce(std::shared_ptr<particle> particle, float dT) = 0;
};

class particleGravity : public particleForceGen {
	public:
		particleGravity(glm::vec3& gravity);
		virtual void updateForce(std::shared_ptr<particle> particle, float dT);

		glm::vec3 gravity = glm::vec3(0.0f);
};

class particleDrag : public particleForceGen {
	public:
		particleDrag(float k1, float k2);
		virtual void updateForce(std::shared_ptr<particle> particle, float dT);

		float k1;
		float k2;
};

class particleSpring : public particleForceGen
{
	public:
		particleSpring(std::shared_ptr<particle> other, float springConstant, float restLength);
		virtual void updateForce(std::shared_ptr<particle> particle, float duration);

		std::shared_ptr<particle> other;
		float springConstant;
		float restLength;
};

class particleAnchoredSpring : public particleForceGen
{
	public:
		particleAnchoredSpring(glm::vec3 anchor, float springConstant, float restLength);
		virtual void updateForce(std::shared_ptr<particle> particle, float duration);

		glm::vec3 anchor;
		float springConstant;
		float restLength;
};

class particleElasticBungee : public particleForceGen
{
	public:
		particleElasticBungee(std::shared_ptr<particle> other, float springConstant, float restLength);
		virtual void updateForce(std::shared_ptr<particle> particle, float duration);

		std::shared_ptr<particle> other;
		float springConstant;
		float restLength;
};

#endif
