#include "particlefgen.hpp"

#include "glm/glm.hpp"

void particleGravity::updateForce(particle* particle, float dT) {
	if(particle->getInverseMass() <= 0.0f) {
		return;
	} 

	particle->addForce(this->gravity * particle->getMass());
}

void particleDrag::updateForce(particle *particle, float dT) {
	glm::vec3 force;
	force = particle->getVelocity();

	float dragCoeff = glm::length(force);
	dragCoeff = k1 * dragCoeff + k2 * dragCoeff * dragCoeff;

	glm::normalize(force);
	force *= -dragCoeff;
	particle->addForce(force);
}
