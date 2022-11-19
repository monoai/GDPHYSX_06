#pragma once
#ifndef PARTICLE_CONTACT_H
#define PARTICLE_CONTACT_H

#include "particle.hpp"
#include <memory>

#include <glm/glm.hpp>
#include <glm/matrix.hpp>

class particleContact {
	friend class particleContactResolver;

	public:
		std::shared_ptr<particle> _particle[2];

		float restitution;
		glm::vec3 contactNormal;
		float penetration;
		void resolve(float dT); //Temporarily on public while code is still being sorted
	private:
		glm::vec3 calculateSeparatingVel() const;
		//void resolve(float dT);
		void resolveVel(float dT);
		void resolveInterpenetration(float dT);
};

class particleContactResolver {
	public:
		particleContactResolver(unsigned iterations);
		void setIterations(unsigned iterations);
		void resolveContacts(std::vector<std::shared_ptr<particleContact>> contactArray, unsigned numContacts, float dT);
	private:
		unsigned iterations;
		unsigned iterationsUsed;
};

class particleContactGen {
	public:
		virtual unsigned addContact(std::shared_ptr<particleContact> contact, unsigned limit) const = 0;
};

#endif
