#include "rigidbodycontact.hpp"

#include <glm/gtx/string_cast.hpp>
#include <cmath>

void rigidBodyContact::resolve(float dT) {
	resolveVel(dT);
	resolveInterpenetration(dT);
}

float rigidBodyContact::calculateSeparatingVel() const {
	glm::vec3 relativeVel = _rbody[0]->getVelocity();
	if(_rbody[1]) relativeVel -= _rbody[1]->getVelocity();
	return glm::dot(relativeVel, contactNormal);
}

void rigidBodyContact::resolveVel(float dT) {
	float separatingVel = calculateSeparatingVel();

	if(separatingVel > 0.0f) {
		//std::cout << "[DEBUG]: SeparatingVel not > 0" << std::endl;
		return;
	}

	float newSepVel = -separatingVel * restitution;

	glm::vec3 accCausedVel = _rbody[0]->getAcceleration();
	if(_rbody[1]) {
		accCausedVel -= _rbody[1]->getAcceleration();
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

	float totalInverseMass = _rbody[0]->getInverseMass();
	if(_rbody[1]) {
		totalInverseMass += _rbody[1]->getInverseMass();
	}

	if(totalInverseMass <= 0) {
		return;
	}

	float impulse = deltaVel / totalInverseMass;

	glm::vec3 impulsePerIMass = contactNormal * impulse;

	//std::cout << "[DEBUG] - RBody0Before: " << glm::to_string(_rbody[0]->getVelocity()) << std::endl;
	//std::cout << "[DEBUG] - RBody1Before: " << glm::to_string(_rbody[1]->getVelocity()) << std::endl;

	if(!std::isnan(impulsePerIMass.x) && !std::isnan(impulsePerIMass.y) && !std::isnan(impulsePerIMass.z)) {
		_rbody[0]->setVelocity(_rbody[0]->getVelocity() + (impulsePerIMass * _rbody[0]->getInverseMass()));
		//std::cout << "[DEBUG] - RBody0After1: " << glm::to_string(_rbody[0]->getVelocity()) << std::endl;
		//std::cout << "[DEBUG] - RBody1After1: " << glm::to_string(_rbody[1]->getVelocity()) << std::endl;
		if(_rbody[1]) {
				//std::cout << "[DEBUG] - RBody0After1: " << glm::to_string(_rbody[0]->getVelocity()) << std::endl;
				//std::cout << "[DEBUG] - RBody1After1: " << glm::to_string(_rbody[1]->getVelocity()) << std::endl;
			_rbody[1]->setVelocity(_rbody[1]->getVelocity() + (impulsePerIMass * -_rbody[1]->getInverseMass()));
		}
	}
}

void rigidBodyContact::resolveInterpenetration(float dT) {
	if(penetration <= 0) {
		return;
	}

	float totalInverseMass = _rbody[0]->getInverseMass();
	if(_rbody[1]) {
		totalInverseMass += _rbody[1]->getInverseMass();
	}

	if(totalInverseMass <= 0) {
		return;
	}

	glm::vec3 movePerIMass = contactNormal * (penetration / totalInverseMass);

	//std::cout << "[DEBUG] - RBody0Before: " << glm::to_string(_rbody[0]->getPosition()) << std::endl;
	//std::cout << "[DEBUG] - RBody1Before: " << glm::to_string(_rbody[1]->getPosition()) << std::endl;

	_rbody[0]->setPosition(_rbody[0]->getPosition() + (movePerIMass * _rbody[0]->getInverseMass()));

	//std::cout << "[DEBUG] - RBody0After1: " << glm::to_string(_rbody[0]->getPosition()) << std::endl;
	//std::cout << "[DEBUG] - RBody1After1: " << glm::to_string(_rbody[1]->getPosition()) << std::endl;

	if(_rbody[1]) {
		_rbody[1]->setPosition(_rbody[1]->getPosition() + (movePerIMass * -_rbody[1]->getInverseMass()));
		//std::cout << "[DEBUG] - RBody0After2: " << glm::to_string(_rbody[0]->getPosition()) << std::endl;
		//std::cout << "[DEBUG] - RBody1After2: " << glm::to_string(_rbody[1]->getPosition()) << std::endl;
	}

	penetration = 0;
}

rigidBodyContactResolver::rigidBodyContactResolver(unsigned iterations) : iterations(iterations) {

}

void rigidBodyContactResolver::setIterations(unsigned iterations) {
	rigidBodyContactResolver::iterations = iterations;
}

void rigidBodyContactResolver::resolveContacts(std::vector<std::shared_ptr<rigidBodyContact>> contactPool, float dT) {
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
