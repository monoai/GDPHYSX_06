#pragma once
#ifndef RIGIDBODY_CONTACT_H
#define RIGIDBODY_CONTACT_H

#include "rigidbody.hpp"
#include <memory>

#include <glm/glm.hpp>
#include <glm/matrix.hpp>

class rigidBodyContact {
	friend class rigidBodyContactResolver;

	public:
		std::shared_ptr<rigidBody> _rbody[2];

		float restitution;
		glm::vec3 contactNormal;
		float penetration = 0.0f;
		//void resolve(float dT); //Temporarily on public while code is still being sorted
	private:
		float calculateSeparatingVel() const;
		void resolve(float dT);
		void resolveVel(float dT);
		void resolveInterpenetration(float dT);
};

class rigidBodyContactResolver {
	public:
		rigidBodyContactResolver(unsigned iterations);
		void setIterations(unsigned iterations);
		void resolveContacts(std::vector<std::shared_ptr<rigidBodyContact>> contactArray, float dT);
	private:
		unsigned iterations;
		unsigned iterationsUsed;
};

class rigidBodyContactGen {
	public:
		virtual std::shared_ptr<rigidBodyContact> addContact() const = 0;
};

#endif
