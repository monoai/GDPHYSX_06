#include "particlefgen.hpp"

#include "glm/glm.hpp"

particleGravity::particleGravity(glm::vec3& gravity) : gravity(gravity) {

}

void particleGravity::updateForce(std::shared_ptr<particle> particle, float dT) {
	if(particle->getInverseMass() <= 0.0f) {
		return;
	} 

	particle->addForce(this->gravity * particle->getMass());
}

particleDrag::particleDrag(float k1, float k2) : k1(k1), k2(k2) {

}

void particleDrag::updateForce(std::shared_ptr<particle> particle, float dT) {
	glm::vec3 force = glm::vec3(0.0f);
	force = particle->getVelocity();

	float dragCoeff = glm::length(force);
	dragCoeff = k1 * dragCoeff + k2 * dragCoeff * dragCoeff;

	glm::normalize(force);
	force *= -dragCoeff;
	particle->addForce(force);
}

particleSpring::particleSpring(std::shared_ptr<particle> other, float springConstant, float restLength) : other(other), springConstant(springConstant), restLength(restLength) {

}

void particleSpring::updateForce(std::shared_ptr<particle> particle, float duration) {
	glm::vec3 force = glm::vec3(0.0f);

	force = particle->getPosition();
	force -= this->other->getPosition();

	float magnitude = glm::length(force);

	magnitude = std::abs(magnitude - restLength);
	magnitude *= this->springConstant;

	force = glm::normalize(force);
	force *= -magnitude;
	particle->addForce(force);
}

particleAnchoredSpring::particleAnchoredSpring(glm::vec3 anchor, float springConstant, float restLength) : anchor(anchor), springConstant(springConstant), restLength(restLength) {

}

void particleAnchoredSpring::updateForce(std::shared_ptr<particle> particle, float duration) {
	glm::vec3 force = glm::vec3(0.0f);

	force = particle->getPosition();
	force -= this->anchor;

	float magnitude = glm::length(force);

	magnitude = std::abs(magnitude - restLength);
	magnitude *= this->springConstant;

	force = glm::normalize(force);
	force *= -magnitude;
	particle->addForce(force);
}

particleElasticBungee::particleElasticBungee(std::shared_ptr<particle> other, float springConstant, float restLength) : other(other), springConstant(springConstant), restLength(restLength) {

}

void particleElasticBungee::updateForce(std::shared_ptr<particle> particle, float duration) {
	glm::vec3 force = glm::vec3(0.0f);

	force = particle->getPosition();
	force -= this->other->getPosition();

	float magnitude = glm::length(force);
	if (magnitude <= this->restLength) return;

	magnitude = this->springConstant * (this->restLength - magnitude);

	force = glm::normalize(force);
	force *= magnitude;
	particle->addForce(force);
}

particlePlanetaryGravity::particlePlanetaryGravity(std::shared_ptr<particle> main, std::shared_ptr<particle> other) : main(main) {
	this->main->changePlanet(other);
}

void particlePlanetaryGravity::updateForce(std::shared_ptr<particle> particle, float duration) {
	if(this->main->planetExists() != false && glm::distance(this->main->getPosition(), this->main->getPlanet()->getPosition()) <= this->main->distLimit) {
		std::cout << "PGRAVITY FORCE CALCULATING" << std::endl;
		glm::vec3 force = glm::vec3(0.0f);
		glm::vec3 distance = this->main->getPosition() - this->main->getPlanet()->getPosition();
		float distanceMag = glm::length(distance);
		float distanceSqr = glm::dot(distance, distance);

		force += -9.81*(((this->main->getMass() * this->main->getPlanet()->getPlanetaryMass())/std::abs(distanceSqr)));
		force *= glm::normalize(distance);

		particle->addForce(force);
	}
}
