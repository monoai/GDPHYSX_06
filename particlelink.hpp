#pragma once
#ifndef PARTICLE_LINK_H
#define PARTICLE_LINK_H
#include "particle.hpp"
#include "particlecontact.hpp"

class particleLink : public particleContactGen
{
	public:
		std::shared_ptr<particle> _particle[2];
		virtual unsigned addContact(std::shared_ptr<particleContact> contact, unsigned limit) const = 0;
	protected:
		float currentLength() const;
};

class particleCable : public particleLink {
	public:
		float maxLength;
		float restitution;

		virtual unsigned addContact(std::shared_ptr<particleContact> contact, unsigned limit) const;

};

class particleRod : public particleLink {
	public:
		float length;

		virtual unsigned addContact(std::shared_ptr<particleContact> contact, unsigned limit) const;
};

#endif
