#include "rigidbody.hpp"

#include <glm/gtx/string_cast.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>

rigidBody::rigidBody(GLuint* normalTransformLoc, GLuint* modelTransformLoc, ObjData object) : normalTrans(*normalTransformLoc), modelTrans(*modelTransformLoc) {
	this->obj = object;
	//std::cout << "init trans: " << glm::to_string(this->trans) << std::endl;
}

// Might not need to use these 2 (3?) functions.
void rigidBody::calculateDerivedData() {
	calculateTransMat(this->trans, this->position, this->orientation);
}

void rigidBody::calculateTransMat(glm::mat4 trans, glm::vec3 pos, glm::quat quat) {
	trans = glm::mat4(1.0f);
	trans = glm::mat4_cast(quat);
	trans = glm::translate(trans, pos);

	// [DEBUG]
	std::cout << "quat: " << glm::to_string(this->orientation) << std::endl;
	std::cout << "mat4: " << glm::to_string(trans) << std::endl;
}

void rigidBody::transformInertiaTensor(glm::mat3 iiTensorWorld, glm::mat3 iiTensorBody, glm::mat4 rotMat) {
	// TODO: Whatever this function was supposed to be.
}

void rigidBody::draw() {
	glBindVertexArray(obj.vaoId);
	// [DEBUG]
	//std::cout << "draw trans: " << glm::to_string(this->trans) << std::endl;
	this->trans = glm::mat4(1.0f); // Resets transMatrix to identity just in case.
	// [DEBUG]
	//std::cout << "currPos: " << glm::to_string(this->positionVector) << std::endl;
	this->trans = glm::translate(this->trans, position);
	// [DEBUG]
	//std::cout << "draw trans after translate: " << glm::to_string(this->trans) << std::endl
	glm::mat4 rotMat = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
	this->trans = this->trans * rotMat;
	//this->trans = glm::rotate(this->trans, glm::radians(1.0f), rotation);
	// [DEBUG]
	//std::cout << "draw trans after rotate: " << glm::to_string(this->trans) << std::endl;
	this->trans = glm::scale(this->trans, glm::vec3(1.0f,1.0f,1.0f));
	
	glm::mat4 normalTransLoc = glm::transpose(glm::inverse(this->trans));
	glUniformMatrix4fv(this->normalTrans, 1, GL_FALSE, glm::value_ptr(normalTransLoc));
	glUniformMatrix4fv(this->modelTrans, 1, GL_FALSE, glm::value_ptr(this->trans));
	// Applying textures to the object if there are any detected.
	glActiveTexture(GL_TEXTURE0);
	GLuint objTexture = obj.textures[obj.materials[0].diffuse_texname];
	glBindTexture(GL_TEXTURE_2D, objTexture);

	glDrawElements(GL_TRIANGLES, obj.numFaces, GL_UNSIGNED_INT, (void*)0);

	// unbindtexture after rendering
	glBindTexture(GL_TEXTURE_2D, 0);
}


void rigidBody::update(float dT) {
	if(!isUpdate) return;

	lastFrameAcceleration = this->acceleration;
	lastFrameAcceleration += forceAccum * inverseMass;

	glm::vec3 angularAcceleration = inverseInertiaTensorWorld * torqueAccum;

	this->velocity += lastFrameAcceleration * dT;

	this->rotation += angularAcceleration * dT;

	// Impose drag
	this->velocity *= pow(damping, dT);
	this->rotation *= pow(angularDamping, dT);

	this->position += velocity * dT;

	// Find a way to convert a rotation vector multiplied with dT into a quaternion
	this->orientation = this->orientation * (this->rotation * dT);

	std::cout << "rotate: " << glm::to_string(this->rotation) << std::endl;

	// calculateDerivedData();
	
	clearAccums();
}

void rigidBody::clearAccums() {
	this->forceAccum = glm::vec3(0.0f);
	this->torqueAccum = glm::vec3(0.0f);
}

void rigidBody::addForce(glm::vec3 force) {
	this->forceAccum += force;
}

void rigidBody::addForceAtBodyPoint(glm::vec3 force, glm::vec3 point) {
	//glm::vec3 targetPoint = getPointInWorldSpace(point);
	//addForceAtPoint(force, targetPoint);
}

void rigidBody::addForceAtPoint(glm::vec3 force, glm::vec3 point) {
	glm::vec3 targetPoint = point;
	targetPoint -= this->position;

	forceAccum += force;
	torqueAccum += glm::cross(targetPoint, force);

	isUpdate = true;
}

void rigidBody::setInertiaTensor(glm::mat3 inertiaTensor) {
	glm::inverse(inertiaTensor);
	// check inverse inertia tensor?
}

/*
glm::vec3 rigidBody::getPointInLocalSpace(glm::vec3 point) {
	glm::vec3 result = glm::inverse(this->trans) * point;
	return result;
}

glm::vec3 rigidBody::getPointInWorldSpace(glm::vec3 point) {
	glm::vec3 result = this->trans * point;
	return result;
}
*/

glm::vec3 rigidBody::getLastFrameAcceleration() {
	return this->lastFrameAcceleration;
}

/* [getPosition FUNCTION]
 * Returns the position vector 
 */
glm::vec3 rigidBody::getPosition() {
	return this->position;
}

/* [setPosition FUNCTION]
 * This is the function that overrides the current position
 * of a rigidBody whenever needed. Normally this is only used
 * for debugging purposes but it could also be used for
 * resetting the position or other use cases that a precise
 * position is needed
 */
void rigidBody::setPosition(glm::vec3 vector) {

	this->position = vector;
}

/* [getInverseMass FUNCTION]
 * Returns the inverseMass
 */
float rigidBody::getInverseMass() {
	return this->inverseMass;
}

/* [setMass FUNCTION]
 * Sets the mass by first calculating the inverse of the mass
 * then assigns it to the inverseMass variable.
 */
void rigidBody::setMass(float mass) {
	this->inverseMass = (1.0f)/mass;
}

/* [getPosition FUNCTION]
 * Returns the mass which is actually the calculated 
 * version of the inverseMass.
 */
float rigidBody::getMass() {
	return (1.0f)/this->inverseMass;
}

/*
 * [setVelocity FUNCTION]
 * Sets the velocity via a glm::vec3 parameter
 */
void rigidBody::setVelocity(glm::vec3 newVel) {
	this->velocity = newVel;	
}

/* [getVelocity FUNCTION]
 * Returns the velocity vector.
 */
glm::vec3 rigidBody::getVelocity() {
	return this->velocity;
}

/* [getAcceleration FUNCTION]
 * Returns the acceleration vector. 
 */
glm::vec3 rigidBody::getAcceleration() {
	return this->acceleration;
}

void rigidBody::setType(int _type) {
	this->rbodyType = _type;
}

int rigidBody::getType() {
	return this->rbodyType;
}
