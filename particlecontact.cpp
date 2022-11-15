#include "particlecontact.hpp"

void particleContact::resolve(float dT) {
	resolveVel(dT);
	resolveInterpenetration(dT);
}

glm::vec3 particleContact::calculateSeparatingVel() const {
	glm::vec3 relativeVel = particle[0]->getVelocity();
	if(particle[1]) relativeVel -= particle[1]->getVelocity();
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

	glm::vec3 accCausedVel = particle[0]->getAcceleration();
	if(particle[1]) {
		accCausedVel -= particle[1]->getAcceleration();
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

	float totalInverseMass = particle[0]->getInverseMass();
	if(particle[1]) {
		totalInverseMass += particle[1]->getInverseMass();
	}

	if(totalInverseMass <= 0) {
		return;
	}

	glm::vec3 impulse = deltaVel / totalInverseMass;

	glm::vec3 impulsePerIMass = contactNormal * impulse;

	particle[0]->setVelocity(particle[0]->getVelocity() + impulsePerIMass * particle[0]->getInverseMass());
	if(particle[1]) {
		particle[1]->setVelocity(particle[1]->getVelocity() + impulsePerIMass * -particle[1]->getInverseMass());
	}
}

void particleContact::resolveInterpenetration(float dT) {
	if(penetration <= 0) {
		return;
	}

	float totalInverseMass = particle[0]->getInverseMass();
	if(particle[1]) {
		totalInverseMass += particle[1]->getInverseMass();
	}

	if(totalInverseMass <= 0) {
		return;
	}

	glm::vec3 movePerIMass = contactNormal * (-penetration / totalInverseMass);

	particle[0]->setPosition(particle[0]->getPosition() + movePerIMass * particle[0]->getInverseMass());

	if(particle[1]) {
		particle[1]->setPosition(particle[1]->getPosition() + movePerIMass * particle[1]->getInverseMass());
	}
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
