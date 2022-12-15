#pragma once
#ifndef RIGIDBODY_FORCE_GEN_H
#define RIGIDBODY_FORCE_GEN_H

#include "rigidbody.hpp"

#include <memory>

class rigidForceGen {
	public:
		virtual void updateForce(std::shared_ptr<rigidBody> rbody, float dT) = 0;
};

class rigidGravity : public rigidForceGen {
	public:
		rigidGravity(glm::vec3& gravity);
		virtual void updateForce(std::shared_ptr<rigidBody> rbody, float dT);

		glm::vec3 gravity = glm::vec3(0.0f);
};

class rigidDrag : public rigidForceGen {
	public:
		rigidDrag(float k1, float k2);
		virtual void updateForce(std::shared_ptr<rigidBody> rbody, float dT);

		float k1;
		float k2;
};

class rigidSpring : public rigidForceGen
{
	public:
		rigidSpring(std::shared_ptr<rigidBody> other, float springConstant, float restLength);
		virtual void updateForce(std::shared_ptr<rigidBody> rbody, float duration);

		std::shared_ptr<rigidBody> other;
		float springConstant;
		float restLength;
};

class rigidAnchoredSpring : public rigidForceGen
{
	public:
		rigidAnchoredSpring(glm::vec3 anchor, float springConstant, float restLength);
		virtual void updateForce(std::shared_ptr<rigidBody> rbody, float duration);

		glm::vec3 anchor;
		float springConstant;
		float restLength;
};

class rigidElasticBungee : public rigidForceGen
{
	public:
		rigidElasticBungee(std::shared_ptr<rigidBody> other, float springConstant, float restLength);
		virtual void updateForce(std::shared_ptr<rigidBody> rbody, float duration);

		std::shared_ptr<rigidBody> other;
		float springConstant;
		float restLength;
};

#endif
