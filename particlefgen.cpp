#include "particlefgen.hpp"

#include "glm/glm.hpp"

particleGravity::particleGravity(glm::vec3& gravity) : gravity(gravity)
{
}

void particleGravity::updateForce(particle* particle, float dT) {
	if(particle->getInverseMass() <= 0.0f) {
		return;
	} 

	particle->addForce(this->gravity * particle->getMass());
}

particleDrag::particleDrag(float k1, float k2) : k1(k1), k2(k2) {

}

void particleDrag::updateForce(particle *particle, float dT) {
	glm::vec3 force = glm::vec3(0.0f);
	force = particle->getVelocity();

	float dragCoeff = glm::length(force);
	dragCoeff = k1 * dragCoeff + k2 * dragCoeff * dragCoeff;

	glm::normalize(force);
	force *= -dragCoeff;
	particle->addForce(force);
}

particleSpring::particleSpring(particle* other, float springConstant, float restLength) : other(other), springConstant(springConstant), restLength(restLength) {

}

void particleSpring::updateForce(particle* particle, float duration) {
	glm::vec3 force = glm::vec3(0.0f);

	force = particle->getPosition();
	force -= this->other->getPosition();

	float magnitude = force.length();

	magnitude = std::abs(magnitude - restLength);
	magnitude *= this->springConstant;

	force = glm::normalize(force);
	force *= magnitude;
	particle->addForce(force);
}

particleAnchoredSpring::particleAnchoredSpring(glm::vec3* anchor, float springConstant, float restLength) {

}

void particleAnchoredSpring::updateForce(particle* particle, float duration) {
	glm::vec3 force = glm::vec3(0.0f);

	force = particle->getPosition();
	force -= *anchor;

	float magnitude = force.length();

	magnitude = std::abs(magnitude - restLength);
	magnitude *= springConstant;

	force = glm::normalize(force);
	force *= magnitude;
	particle->addForce(force);
}

particleElasticBungee::particleElasticBungee(particle* other, float springConstant, float restLength) {

}

void particleElasticBungee::updateForce(particle* particle, float duration) {
	glm::vec3 force = glm::vec3(0.0f);

	force = particle->getPosition();
	force -= other->getPosition();

	float magnitude = force.length();

	magnitude = std::abs(magnitude - restLength);
	if (magnitude <= restLength) return;

	magnitude = springConstant * (restLength - magnitude);

	force = glm::normalize(force);
	force *= magnitude;
	particle->addForce(force);
}
