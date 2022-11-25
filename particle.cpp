#include "particle.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <cmath>

particle::particle(GLuint* normalTransformLoc, GLuint* modelTransformLoc, ObjData object) : normalTrans(*normalTransformLoc), modelTrans(*modelTransformLoc) {
	this->trans = glm::mat4(1.0f);
	this->obj = object;
	//std::cout << "init trans: " << glm::to_string(this->trans) << std::endl;
}

/* [UPDATE FUNCTION]
 * The function where all the calculations happens
 * deltaTime usually updated through here to ensure
 * it only starts when the object is initiated. 
 */
void particle::update(float dT) {
	// Updates the position
	this->position += this->velocity * dT;

	// Calculates the acceleration and the force.
	glm::vec3 newAccel = this->acceleration;
	newAccel += this->forceAccum * this->inverseMass; 
	
	// Updates the velocity
	this->velocity += newAccel * dT;

	// Adds drag
	this->velocity *= pow(this->damping,dT);

	// Clears accumulated force
	clearForceAccum();

	life += dT;
	//[DEBUG]
	//std::cout << "Life: " << life << std::endl;
	//std::cout << "Time " << dT << std::endl;
	if(life >= 2.5f) {
		inUse = false;
	}
}

/* [DRAW FUNCTION]
 * This is where all the OpenGL draw calls and transformations
 * are happening. All the translations, rotations, scales that
 * can be done on a model are already handled by this function.
 */
void particle::draw() {
	glBindVertexArray(obj.vaoId);
	// [DEBUG]
	//std::cout << "draw trans: " << glm::to_string(this->trans) << std::endl;
	this->trans = glm::mat4(1.0f);
	// [DEBUG]
	//std::cout << "currPos: " << glm::to_string(this->positionVector) << std::endl;
	this->trans = glm::translate(this->trans, position);
	//this->trans = glm::rotate(this->trans, glm::radians(0.0f), glm::vec3(0.0f,0.0f,0.0f));
	this->trans = glm::scale(this->trans, glm::vec3(1.0f,1.0f,1.0f));
	
	glm::mat4 normalTransLoc = glm::transpose(glm::inverse(this->trans));
	glUniformMatrix4fv(this->normalTrans, 1, GL_FALSE, glm::value_ptr(normalTransLoc));
	normalTransLoc = glm::transpose(glm::inverse(this->trans));
	glUniformMatrix4fv(this->normalTrans, 1, GL_FALSE, glm::value_ptr(normalTransLoc));
	//glUniformMatrix4fv(*modelTransformLoc, 1, GL_FALSE, glm::value_ptr(this->trans));
	glUniformMatrix4fv(this->modelTrans, 1, GL_FALSE, glm::value_ptr(this->trans));
	// Applying textures to the object if there are any detected.
	glActiveTexture(GL_TEXTURE0);
	GLuint objTexture = obj.textures[obj.materials[0].diffuse_texname];
	glBindTexture(GL_TEXTURE_2D, objTexture);

	glDrawElements(GL_TRIANGLES, obj.numFaces, GL_UNSIGNED_INT, (void*)0);

	// unbindtexture after rendering
	glBindTexture(GL_TEXTURE_2D, 0);
}

/* [setParticleParams FUNCTION]
 * This is the function that sets the properties/parameters
 * of a particle to prepare their proper behavior based on the
 * selected bullet type. Normally the values are applied
 * outside of the class
 */
void particle::setParticleParams(particleName name) {

	switch (name)
	{
	case PISTOL:
		this->velocity = glm::vec3(35.0f, 0.0f, 0.0f);
		this->acceleration = glm::vec3(0.0f, -1.0f, 0.0f);

		this->setMass(2.0f);
		this->damping = 0.99f;
		std::cout << "Pistol set" << std::endl;
		break;
	case ARTILLERY:
		this->velocity = glm::vec3(40.0f, 30.0f, 0.0f);
		this->acceleration = glm::vec3(0.0f, -20.0f, 0.0f);

		this->setMass(200.0f);
		this->damping = 0.99f;
		std::cout << "Artillery set" << std::endl;
		break;
	case FIREBALL:
		this->velocity = glm::vec3(10.0f, 0.0f, 0.0f);
		this->acceleration = glm::vec3(0.0f, 0.6f, 0.0f);

		this->setMass(1.0f);
		this->damping = 0.99f;
		std::cout << "Fireball set" << std::endl;
		break;
	case LASER:

		this->velocity = glm::vec3(100.0f, 0.0f, 0.0f);
		this->acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

		this->setMass(0.1f);
		this->damping = 0.99f;
		std::cout << "Laser set" << std::endl;
		break;
	case FIREWORK:
		this->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		this->acceleration = glm::vec3(0.0f, 0.3f, 0.0f);

		std::cout << "Firework set" << std::endl;
		break;
	
	default:

		this->velocity = glm::vec3(0.0f);
		this->acceleration = glm::vec3(0.0f);

		this->setMass(0.0f);
		this->damping = 0.0f;
		std::cout << "Unknown particle. Setting to 0" << std::endl;
		break;
	}

}

/* [addForce FUNCTION]
 * The function where the accumulated force
 * that was calculated elsewhere will be added
 * in one final vector. Usually used outside
 * of the function rather than here, but could also
 * be used if an initial force is necessary.
 */
void particle::addForce(glm::vec3 force) {
	this->forceAccum += force;
}

/* [clearForceAccum FUNCTION]
 * Clears the accumulated force to ensure
 * that the calculated force wouldn't be too strong 
 * for other iterations
 */
void particle::clearForceAccum() {
	this->forceAccum = glm::vec3(0.0f);
}

/* [getPosition FUNCTION]
 * Returns the position vector 
 */
glm::vec3 particle::getPosition() {
	return this->position;
}

/* [setPosition FUNCTION]
 * This is the function that overrides the current position
 * of a particle whenever needed. Normally this is only used
 * for debugging purposes but it could also be used for
 * resetting the position or other use cases that a precise
 * position is needed
 */
void particle::setPosition(glm::vec3 vector) {

	this->position = vector;
}

/* [getInverseMass FUNCTION]
 * Returns the inverseMass
 */
float particle::getInverseMass() {
	return this->inverseMass;
}

/* [setMass FUNCTION]
 * Sets the mass by first calculating the inverse of the mass
 * then assigns it to the inverseMass variable.
 */
void particle::setMass(float mass) {
	this->inverseMass = (1.0f)/mass;
}

void particle::setInverseMass(float mass) {
	this->inverseMass = mass;
}

float particle::getPlanetaryMass() {
	return (1.0f)/this->planetaryMass;
}

/* [getPosition FUNCTION]
 * Returns the mass which is actually the calculated 
 * version of the inverseMass.
 */
float particle::getMass() {
	return (1.0f)/this->inverseMass;
}

/*
 * [setVelocity FUNCTION]
 * Sets the velocity via a glm::vec3 parameter
 */
void particle::setVelocity(glm::vec3 newVel) {
	this->velocity = newVel;	
}

/* [getVelocity FUNCTION]
 * Returns the velocity vector.
 */
glm::vec3 particle::getVelocity() {
	return this->velocity;
}

/* [getAcceleration FUNCTION]
 * Returns the acceleration vector. 
 */
glm::vec3 particle::getAcceleration() {
	return this->acceleration;
}

void particle::changePlanet(std::shared_ptr<particle> other) {
	this->other = other;
}

void particle::changeLimit(float amt) {
	this->distLimit += amt;
}

bool particle::planetExists() {
	if(this->other != NULL) {
		return true;
	}
	return false;
}

std::shared_ptr<particle> particle::getPlanet() {
	return this->other;
}
