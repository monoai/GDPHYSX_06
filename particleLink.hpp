#pragma once
#include "particle.hpp"
#include "particlecontact.hpp"

class particleLink
{
public:
	std::shared_ptr<particle> _particle[2];
	virtual unsigned fillContact(particleContact contact, unsigned limit) const = 0;
protected:
	float currentLength() const;
};

class particleCable : particleLink{
	float maxLength;
	float restitution;

	virtual unsigned fillContact(particleContact *contact, unsigned limit) const = 0;

};

class particleRod : particleLink {
public:
	float length;
	float currentLength() const;

	virtual unsigned fillContact(particleContact* contact, unsigned limit) const = 0;
};