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

void SpringParticle::updateForce(particle* particle, float duration) {
	glm::vec3 force;

	force = particle->getPosition();
	force -= other->getPosition();

	float magnitude = force.length();

	magnitude = abs(magnitude - restLength);
	magnitude *= springConstant;

	force = glm::normalize(force);
	force *= magnitude;
	particle->addForce(force);
}

void AnchoredSpring::updateForce(particle* particle, float duration) {
	glm::vec3 force;

	force = particle->getPosition();
	force -= *anchor;

	float magnitude = force.length();

	magnitude = abs(magnitude - restLength);
	magnitude *= springConstant;

	force = glm::normalize(force);
	force *= magnitude;
	particle->addForce(force);
}

void ElasticBungee::updateForce(particle* particle, float duration) {
	glm::vec3 force;

	force = particle->getPosition();
	force -= other->getPosition();

	float magnitude = force.length();

	magnitude = abs(magnitude - restLength);
	if (magnitude <= restLength) return;

	magnitude = springConstant * (restLength - magnitude);

	force = glm::normalize(force);
	force *= magnitude;
	particle->addForce(force);
}
