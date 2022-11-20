#include "particlelink.hpp"

float particleLink::currentLength() const {
	glm::vec3 relativePos = _particle[0]->getPosition() - _particle[1]->getPosition();
	return glm::length(relativePos);
}

std::shared_ptr<particleContact> particleCable::addContact() const {
	float length = currentLength();
	if (length < maxLength) {
		return nullptr;
	}

	std::shared_ptr<particleContact> contact(new particleContact());

	contact->_particle[0] = _particle[0];
	contact->_particle[1] = _particle[1];

	glm::vec3 normal = _particle[1]->getPosition() - _particle[0]->getPosition();
	contact->contactNormal = glm::normalize(normal);

	contact->penetration = length - maxLength;
	contact->restitution = restitution;

	return contact;
}

std::shared_ptr<particleContact> particleRod::addContact() const {
	float _currentLength = currentLength();
	//std::cout << "[DEBUG] - currLength: " << _currentLength << std::endl;
	//std::cout << "[DEBUG] - length: " << length << std::endl;
	if (_currentLength == length) return nullptr;

	std::shared_ptr<particleContact> contact(new particleContact());

	contact->_particle[0] = _particle[0];
	contact->_particle[1] = _particle[1];

	glm::vec3 _normal = _particle[1]->getPosition() - _particle[0]->getPosition();
	_normal = glm::normalize(_normal);

	if (_currentLength > length) {
		contact->contactNormal = _normal;
		contact->penetration = _currentLength - length;
		//std::cout << "[DEBUG] - currLength > length" << std::endl;
	}
	else {
		contact->contactNormal = _normal * -1.0f;
		contact->penetration = length - _currentLength;
		//std::cout << "[DEBUG] - currLength < length" << std::endl;
	}

	contact->restitution = 0;

	return contact;
}
