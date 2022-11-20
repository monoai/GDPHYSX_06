#include "particlecontact.hpp"

#include <glm/gtx/string_cast.hpp>
#include <cmath>

void particleContact::resolve(float dT) {
	resolveVel(dT);
	resolveInterpenetration(dT);
}

float particleContact::calculateSeparatingVel() const {
	glm::vec3 relativeVel = _particle[0]->getVelocity();
	if(_particle[1]) relativeVel -= _particle[1]->getVelocity();
	return glm::dot(relativeVel, contactNormal);
}

void particleContact::resolveVel(float dT) {
	float separatingVel = calculateSeparatingVel();

	if(separatingVel > 0.0f) {
		//std::cout << "[DEBUG]: SeparatingVel not > 0" << std::endl;
		return;
	}

	float newSepVel = -separatingVel * restitution;

	glm::vec3 accCausedVel = _particle[0]->getAcceleration();
	if(_particle[1]) {
		accCausedVel -= _particle[1]->getAcceleration();
	}
	glm::vec3 accCausedSepVel = accCausedVel * contactNormal * dT;

	//std::cout << "[DEBUG] - AccCausedSepVel: " << glm::to_string(accCausedSepVel) << std::endl;

	
	if(glm::all(glm::lessThan(accCausedSepVel, glm::vec3(0.0f)))) {
		std::cout << "[DEBUG]: accCausedSepVel not < 0" << std::endl;
		newSepVel += restitution * glm::length(accCausedSepVel);

		if(newSepVel < 0.0f) {
			std::cout << "[DEBUG]: newSepVel not < 0" << std::endl;
			newSepVel = 0.0f;
		}
	}

	float deltaVel = newSepVel - separatingVel;

	float totalInverseMass = _particle[0]->getInverseMass();
	if(_particle[1]) {
		totalInverseMass += _particle[1]->getInverseMass();
	}

	if(totalInverseMass <= 0) {
		return;
	}

	float impulse = deltaVel / totalInverseMass;

	glm::vec3 impulsePerIMass = contactNormal * impulse;

	//std::cout << "[DEBUG] - Particle0Before: " << glm::to_string(_particle[0]->getVelocity()) << std::endl;
	//std::cout << "[DEBUG] - Particle1Before: " << glm::to_string(_particle[1]->getVelocity()) << std::endl;

	if(!std::isnan(impulsePerIMass.x) && !std::isnan(impulsePerIMass.y) && !std::isnan(impulsePerIMass.z)) {
		_particle[0]->setVelocity(_particle[0]->getVelocity() + (impulsePerIMass * _particle[0]->getInverseMass()));
		//std::cout << "[DEBUG] - Particle0After1: " << glm::to_string(_particle[0]->getVelocity()) << std::endl;
		//std::cout << "[DEBUG] - Particle1After1: " << glm::to_string(_particle[1]->getVelocity()) << std::endl;
		if(_particle[1]) {
				//std::cout << "[DEBUG] - Particle0After1: " << glm::to_string(_particle[0]->getVelocity()) << std::endl;
				//std::cout << "[DEBUG] - Particle1After1: " << glm::to_string(_particle[1]->getVelocity()) << std::endl;
			_particle[1]->setVelocity(_particle[1]->getVelocity() + (impulsePerIMass * -_particle[1]->getInverseMass()));
		}
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

	glm::vec3 movePerIMass = contactNormal * (penetration / totalInverseMass);

	//std::cout << "[DEBUG] - Particle0Before: " << glm::to_string(_particle[0]->getPosition()) << std::endl;
	//std::cout << "[DEBUG] - Particle1Before: " << glm::to_string(_particle[1]->getPosition()) << std::endl;

	_particle[0]->setPosition(_particle[0]->getPosition() + (movePerIMass * _particle[0]->getInverseMass()));

	//std::cout << "[DEBUG] - Particle0After1: " << glm::to_string(_particle[0]->getPosition()) << std::endl;
	//std::cout << "[DEBUG] - Particle1After1: " << glm::to_string(_particle[1]->getPosition()) << std::endl;

	if(_particle[1]) {
		_particle[1]->setPosition(_particle[1]->getPosition() + (movePerIMass * -_particle[1]->getInverseMass()));
		//std::cout << "[DEBUG] - Particle0After2: " << glm::to_string(_particle[0]->getPosition()) << std::endl;
		//std::cout << "[DEBUG] - Particle1After2: " << glm::to_string(_particle[1]->getPosition()) << std::endl;
	}

	penetration = 0;
}

particleContactResolver::particleContactResolver(unsigned iterations) : iterations(iterations) {

}

void particleContactResolver::setIterations(unsigned iterations) {
	particleContactResolver::iterations = iterations;
}

void particleContactResolver::resolveContacts(std::vector<std::shared_ptr<particleContact>> contactPool, float dT) {
	iterationsUsed = 0;

	while(iterationsUsed < iterations) {
		float min = contactPool[0]->calculateSeparatingVel();
		unsigned minIndex = 0;
		for(unsigned i = 0; i < contactPool.size(); i++) {
			if(contactPool[i]->calculateSeparatingVel() < min){
				min = contactPool[i]->calculateSeparatingVel();
				minIndex = i;
				//std::cout << "[DEBUG] - Something got calculated" << std::endl;
			}
		}
		
		/*
		if(minIndex == contactPool.size()) {
			std::cout << "[DEBUG] - algoDone" << std::endl;
			break;
		}
		*/

		//std::cout << "[DEBUG] - IterationsUsed: " << iterationsUsed << std::endl;
		contactPool[minIndex]->resolve(dT);

		iterationsUsed++;
	}
}
