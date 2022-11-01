#pragma once
#ifndef PARTICLE_H
#define PARTICLE_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>

#include "obj_mesh.h"

class particle {
	public:
		bool inUse;
		enum particleName {
			UNKNOWN, PISTOL, ARTILLERY, FIREBALL, LASER, FIREWORK
		};

		particle(GLuint* normalTransformLoc, GLuint* modelTransformLoc, ObjData object);

		void draw();
		void update(float dT);
		void clearForceAccum();
		void addForce(glm::vec3 force);
		void setParticleParams(particleName name);

		void setPosition(glm::vec3 vector);
		glm::vec3 getPosition();

		float getInverseMass();
		void setMass(float mass);
		float getMass();
		glm::vec3 getVelocity();
		glm::vec3 getAcceleration();
	private:
		// Transformation variables
		glm::mat4 trans;
		GLuint& normalTrans;
		GLuint& modelTrans;

		// Position variables
		glm::vec3 positionVector = glm::vec3(0.0f);

		// Life variables
		float life = 0.0f;
		
		// Physics variables
		glm::vec3 velocityVector = glm::vec3(0.0f);
		glm::vec3 accelerationVector = glm::vec3(0.0f);
		glm::vec3 forceAccumVec = glm::vec3(0.0f);

		// Particle properties
		ObjData obj;
		float inverseMass = 0.0f;
		float mass = 0.0f;
		float damping = 0.0f;
};

#endif
