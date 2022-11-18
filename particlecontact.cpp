#include "particlecontact.hpp"

void particleContact::resolve(float dT) {
	resolveVel(dT);
	resolveInterpenetration(dT);
}

glm::vec3 particleContact::calculateSeparatingVel() const {
	glm::vec3 relativeVel = _particle[0]->getVelocity();
	if(_particle[1]) relativeVel -= _particle[1]->getVelocity();
	return relativeVel * contactNormal;
}

void particleContact::resolveVel(float dT) {
	glm::vec3 separatingVel = calculateSeparatingVel();

	// Maybe try this instead?
	// if(glm::length2(separatingVel) > 0.0f)
	if(glm::all(glm::greaterThan(separatingVel, glm::vec3(0.0f)))) {
		std::cout << "[DEBUG]: SeparatingVel not > 0" << std::endl;
		return;
	}

	glm::vec3 newSepVel = -separatingVel * restitution;

	glm::vec3 accCausedVel = _particle[0]->getAcceleration();
	if(_particle[1]) {
		accCausedVel -= _particle[1]->getAcceleration();
	}
	glm::vec3 accCausedSepVel = accCausedVel * contactNormal * dT;

	if(glm::all(glm::lessThan(accCausedSepVel, glm::vec3(0.0f)))) {
		std::cout << "[DEBUG]: accCausedSepVel not < 0" << std::endl;
		newSepVel += restitution * accCausedSepVel;

		if(glm::all(glm::lessThan(newSepVel, glm::vec3(0.0f)))) {
			std::cout << "[DEBUG]: newSepVel not < 0" << std::endl;
			newSepVel = glm::vec3(0.0f);
		}
	}

	glm::vec3 deltaVel = newSepVel - separatingVel;

	float totalInverseMass = _particle[0]->getInverseMass();
	if(_particle[1]) {
		totalInverseMass += _particle[1]->getInverseMass();
	}

	if(totalInverseMass <= 0) {
		return;
	}

	glm::vec3 impulse = deltaVel / totalInverseMass;

	glm::vec3 impulsePerIMass = contactNormal * impulse;

	_particle[0]->setVelocity(_particle[0]->getVelocity() + impulsePerIMass * _particle[0]->getInverseMass());
	if(_particle[1]) {
		_particle[1]->setVelocity(_particle[1]->getVelocity() + impulsePerIMass * -_particle[1]->getInverseMass());
	}
}

void particleContact::resolveInterpenetration(float dT) {
	if(penetration <= 0) {
		return;
	}

	float totalInverseMass = _particle[0]->getInverseMass();
	if(_particle[1]) {
		totalInverseMass += _particle[1]->getInverseMass();
	}

	if(totalInverseMass <= 0) {
		return;
	}

	glm::vec3 movePerIMass = contactNormal * (-penetration / totalInverseMass);

	_particle[0]->setPosition(_particle[0]->getPosition() + movePerIMass * _particle[0]->getInverseMass());

	if(_particle[1]) {
		_particle[1]->setPosition(_particle[1]->getPosition() + movePerIMass * _particle[1]->getInverseMass());
	}
}

particleContactResolver::particleContactResolver(unsigned iterations) : iterations(iterations) {

}

void particleContactResolver::setIterations(unsigned iterations) {
	particleContactResolver::iterations = iterations;
}

void particleContactResolver::resolveContacts(particleContact *contactArray, unsigned int numContacts, float dT) {
	iterationsUsed = 0;
	while(iterationsUsed < iterations) {
		glm::vec3 max = glm::vec3(0.0f);
		unsigned maxIndex = numContacts;
		for(unsigned i = 0; i < numContacts; i++) {
			glm::vec3 sepVel = contactArray[i].calculateSeparatingVel();
			if(glm::all(glm::lessThan(sepVel, max))){
				max = sepVel;
				maxIndex = i;
			}
		}

		contactArray[maxIndex].resolve(dT);
		iterationsUsed++;
	}
}
