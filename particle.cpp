#include "particle.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

#include "obj_mesh.h"


particle::particle(glm::mat4* transMat, GLuint* normalTransformLoc, GLuint* modelTransformLoc, ObjData object) : trans(*transMat), normalTrans(*normalTransformLoc), modelTrans(*modelTransformLoc), inUse(false) {
}

//particle::particle(const particle &other) : trans(other.trans), normalTrans(other.normalTrans), modelTrans(other.modelTrans), inUse(other.inUse) {
//}

/*
particle::~particle() {
	std::cout << "Particle destroyed" << std::endl;
}
*/

/* [UPDATE FUNCTION]
 * The function where all the calculations happens
 * deltaTime usually updated through here to ensure
 * it only starts when the object is initiated. 
 */
void particle::update(float dT) {
	
	// Updates the position
	this->positionVector += this->velocityVector * dT;

	// Calculates the acceleration
	glm::vec3 newAccel = this->accelerationVector;
	newAccel += this->forceAccumVec * this->inverseMass; 
	
	// Updates the velocity
	this->velocityVector += newAccel * dT;

	// Adds drag
	this->velocityVector *= pow(this->damping,dT);

	// Clears accumulated force
	clearForceAccum();


	//[Debug]
	//std::cout << this->yPos << " = " << this->yVelocity << " * " << this->damping << " + " << this->yAcceleration << std::endl;
	//std::cout <<"xPosition " << this->xPos << std::endl;
	//std::cout <<"yPosition " << this->yPos << std::endl;
	//std::cout << "Time " << dT << std::endl;
	life += dT;
	//[Debug]
	//std::cout << "Life: " << life << std::endl;
	if(life >= 2.5f) {
		inUse = false;
		//this->~particle();
	}
}

/* [DRAW FUNCTION]
 * This is where all the OpenGL draw calls and transformations
 * are happening. All the translations, rotations, scales that
 * can be done on a model are already handled by this function.
 */
void particle::draw(ObjData obj) {
	this->trans = glm::mat4(1.0f);
	// [DEBUG]
	//std::cout << "currxPos set: " << this->xPos << std::endl;
	//std::cout << "curryPos set: " << yPos << std::endl;
	this->trans = glm::translate(this->trans, positionVector);
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
		this->velocityVector = glm::vec3(35.0f, 0.0f, 0.0f);
		this->accelerationVector = glm::vec3(0.0f, -1.0f, 0.0f);

		this->setMass(2.0f);
		this->damping = 0.99f;
		std::cout << "Pistol set" << std::endl;
		break;
	case ARTILLERY:
		this->velocityVector = glm::vec3(40.0f, 30.0f, 0.0f);
		this->accelerationVector = glm::vec3(0.0f, -20.0f, 0.0f);

		this->setMass(200.0f);
		this->damping = 0.99f;
		std::cout << "Artillery set" << std::endl;
		break;
	case FIREBALL:
		this->velocityVector = glm::vec3(10.0f, 0.0f, 0.0f);
		this->accelerationVector = glm::vec3(0.0f, 0.6f, 0.0f);

		this->setMass(1.0f);
		this->damping = 0.99f;
		std::cout << "Fireball set" << std::endl;
		break;
	case LASER:

		this->velocityVector = glm::vec3(100.0f, 0.0f, 0.0f);
		this->accelerationVector = glm::vec3(0.0f, 0.0f, 0.0f);

		this->setMass(0.1f);
		this->damping = 0.99f;
		std::cout << "Laser set" << std::endl;
		break;
	case FIREWORK:
		this->velocityVector = glm::vec3(0.0f, 0.0f, 0.0f);
		this->accelerationVector = glm::vec3(0.0f, 0.3f, 0.0f);

		std::cout << "Firework set" << std::endl;
		break;
	
	default:

		this->velocityVector = glm::vec3(0.0f);
		this->accelerationVector = glm::vec3(0.0f);

		this->setMass(0.0f);
		this->damping = 0.0f;
		std::cout << "Unknown particle. Setting to 0" << std::endl;
		break;
	}

}

void particle::addForce(glm::vec3 force) {
	this->forceAccumVec += force;
}

void particle::clearForceAccum() {
	this->forceAccumVec = glm::vec3(0.0f);
}

/* [setPosition FUNCTION]
 * This is the function that overrides the current position
 * of a particle whenever needed. Normally this is only used
 * for debugging purposes but it could also be used for
 * resetting the position or other use cases that a precise
 * position is needed
 */
void particle::setPosition(glm::vec3 vector) {

	this->positionVector = vector;
	// [DEBUG]
	//std::cout << "xPos set: " << xPos << std::endl;
	//std::cout << "yPos set: " << yPos << std::endl;
}

float particle::getInverseMass() {
	return this->inverseMass;
}

void particle::setMass(float mass) {
	this->inverseMass = (1.0f)/mass;
}

float particle::getMass() {
	return (1.0f)/this->inverseMass;
}

glm::vec3 particle::getVelocity() {
	return this->velocityVector;
}

glm::vec3 particle::getPosition() {
	return this->positionVector;
}

glm::vec3 particle::getAcceleration() {
	return this->accelerationVector;
}
