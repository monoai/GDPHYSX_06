#include "particle.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obj_mesh.h"

particle::particle(glm::mat4* transMat, GLuint* normalTransformLoc, GLuint* modelTransformLoc, ObjData object) : trans(*transMat), normalTrans(*normalTransformLoc), modelTrans(*modelTransformLoc) {
}

void particle::update(float time) {
	this->xPos += (this->xVelocity*this->damping) + time;
	this->yPos += (this->yVelocity * this->damping + this->yAcceleration )+ time;
	
	if (this->yVelocity > 0 && this->yAcceleration < 0){
		this->yVelocity += (this->yAcceleration * damping) + time;
	}
	//std::cout << this->yPos << " = " << this->yVelocity << " * " << this->damping << " + " << this->yAcceleration << std::endl;
	//std::cout <<"xPosition " << this->xPos << std::endl;
	std::cout <<"yPosition " << this->yPos << std::endl;
	//std::cout << "Time " << time << std::endl;

}

void particle::draw(ObjData obj) {
	this->trans = glm::mat4(1.0f);
	// [DEBUG]
	//std::cout << "currxPos set: " << this->xPos << std::endl;
	//std::cout << "curryPos set: " << yPos << std::endl;
	this->trans = glm::translate(this->trans, glm::vec3(xPos,yPos,zPos));
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

void particle::setParticleParams(float xVelocity, float yVelocity, float xAcceleration, float yAcceleration, float mass, float damping){
	this->xVelocity = xVelocity;
	this->yVelocity = yVelocity;

	this->xAcceleration = xAcceleration;
	this->yAcceleration = yAcceleration;

	this->mass = mass;
	this->damping = damping;

}

void particle::setPosition(float x, float y, float z) {
	this->xPos = x;
	this->yPos = y;
	this->zPos = z;
	// [DEBUG]
	//std::cout << "xPos set: " << xPos << std::endl;
	//std::cout << "yPos set: " << yPos << std::endl;
}

