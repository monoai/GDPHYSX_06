#include "AnchoredSpring.hpp"

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