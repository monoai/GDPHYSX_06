#include "SpringParticle.hpp"

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