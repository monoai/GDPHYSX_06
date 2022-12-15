#include "rigidbodyfgen.hpp"

#include "glm/glm.hpp"

rigidGravity::rigidGravity(glm::vec3& gravity) : gravity(gravity) {

}

void rigidGravity::updateForce(std::shared_ptr<rigidBody> rbody, float dT) {
	if(rbody->getInverseMass() <= 0.0f) {
		return;
	} 

	rbody->addForce(this->gravity * rbody->getMass());
}

rigidDrag::rigidDrag(float k1, float k2) : k1(k1), k2(k2) {

}

void rigidDrag::updateForce(std::shared_ptr<rigidBody> rbody, float dT) {
	glm::vec3 force = glm::vec3(0.0f);
	force = rbody->getVelocity();

	float dragCoeff = glm::length(force);
	dragCoeff = k1 * dragCoeff + k2 * dragCoeff * dragCoeff;

	glm::normalize(force);
	force *= -dragCoeff;
	rbody->addForce(force);
}

rigidSpring::rigidSpring(std::shared_ptr<rigidBody> rbody, float springConstant, float restLength) : other(other), springConstant(springConstant), restLength(restLength) {

}

void rigidSpring::updateForce(std::shared_ptr<rigidBody> rbody, float duration) {
	glm::vec3 force = glm::vec3(0.0f);

	force = rbody->getPosition();
	force -= this->other->getPosition();

	float magnitude = glm::length(force);

	magnitude = std::abs(magnitude - restLength);
	magnitude *= this->springConstant;

	force = glm::normalize(force);
	force *= -magnitude;
	rbody->addForce(force);
}

rigidAnchoredSpring::rigidAnchoredSpring(glm::vec3 anchor, float springConstant, float restLength) : anchor(anchor), springConstant(springConstant), restLength(restLength) {

}

void rigidAnchoredSpring::updateForce(std::shared_ptr<rigidBody> rbody, float duration) {
	glm::vec3 force = glm::vec3(0.0f);

	force = rbody->getPosition();
	force -= this->anchor;

	float magnitude = glm::length(force);

	magnitude = std::abs(magnitude - restLength);
	magnitude *= this->springConstant;

	force = glm::normalize(force);
	force *= -magnitude;
	rbody->addForce(force);
}

rigidElasticBungee::rigidElasticBungee(std::shared_ptr<rigidBody> other, float springConstant, float restLength) : other(other), springConstant(springConstant), restLength(restLength) {

}

void rigidElasticBungee::updateForce(std::shared_ptr<rigidBody> rbody, float duration) {
	glm::vec3 force = glm::vec3(0.0f);

	force = rbody->getPosition();
	force -= this->other->getPosition();

	float magnitude = glm::length(force);
	if (magnitude <= this->restLength) return;

	magnitude = this->springConstant * (this->restLength - magnitude);

	force = glm::normalize(force);
	force *= magnitude;
	rbody->addForce(force);
}
