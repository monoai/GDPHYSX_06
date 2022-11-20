#include "particlelink.hpp"

float particleLink::currentLength() const {
	glm::vec3 relativePos = _particle[0]->getPosition() - _particle[1]->getPosition();
	return glm::length(relativePos);
}

unsigned particleCable::addContact(std::shared_ptr<particleContact> contact, unsigned limit) const {
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

unsigned particleRod::addContact(std::shared_ptr<particleContact> contact, unsigned limit) const {
	float _currentLength = currentLength();
	if (_currentLength == length) return 0;

	contact->_particle[0] = _particle[0];
	contact->_particle[1] = _particle[1];

	glm::vec3 _normal = _particle[1]->getPosition() - _particle[0]->getPosition();
	_normal = glm::normalize(_normal);

	if (_currentLength > length) {
		contact->contactNormal = _normal;
		contact->penetration = _currentLength - length;
	}
	else {
		contact->contactNormal = _normal * -1.0f;
		contact->penetration = length - _currentLength;
	}

	contact->restitution = 0.5f;

	return 1;
}
