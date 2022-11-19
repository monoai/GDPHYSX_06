#include "particleLink.hpp"

float particleLink::currentLength() const {
	glm::vec3 relativePos = _particle[0]->getPosition() - _particle[1]->getPosition();
	return glm::length(relativePos);
}

unsigned particleCable::fillContact(particleContact* contact, unsigned limit) const {
	float length = currentLength();
	if (length < maxLength) {
		return 0;
	}

	contact->_particle[0] = _particle[0];
	contact->_particle[1] = _particle[1];

	glm::vec3 normal = _particle[1]->getPosition() - _particle[0]->getPosition();
	contact->contactNormal = glm::normalize(normal);

	contact->penetration = length - maxLength;
	contact->restitution = restitution;

	return 1;
}

unsigned particleRod::fillContact(particleContact* contact, unsigned limit) const {
	float _currentLength = currentLength();
	if (_currentLength == length) return 0;

	contact->_particle[0] = _particle[0];
	contact->_particle[1] = _particle[1];

	glm::vec3 normal = _particle[1]->getPosition() - _particle[0]->getPosition();
	normal = glm::normalize(normal);

	if (_currentLength > length) {
		contact->contactNormal = normal;
		contact->penetration = _currentLength - length;
	}
	else {
		//contact->contactNormal = normal * -1;
		contact->penetration = length - _currentLength - length;
	}

	contact->restitution = 0;

	return 1;
}