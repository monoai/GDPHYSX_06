#include "ElasticBungee.hpp"

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