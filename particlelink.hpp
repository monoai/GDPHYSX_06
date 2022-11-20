#pragma once
#ifndef PARTICLE_LINK_H
#define PARTICLE_LINK_H
#include "particle.hpp"
#include "particlecontact.hpp"

class particleLink : public particleContactGen
{
	public:
		std::shared_ptr<particle> _particle[2];
		virtual std::shared_ptr<particleContact> addContact() const = 0;
	protected:
		float currentLength() const;
};

class particleCable : public particleLink {
	public:
		float maxLength;
		float restitution;

		virtual std::shared_ptr<particleContact> addContact() const;

};

class particleRod : public particleLink {
	public:
		float length;

		virtual std::shared_ptr<particleContact> addContact() const;
};

#endif
