#pragma once
#ifndef PARTICLE_H
#define PARTICLE_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>

#include "obj_mesh.h"
#include <memory>

class particle {
	public:
		bool inUse = false;
		bool dontDelete = false;
		float radius = 1.0f;
		float restitution = 1.0f;
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
		float getPlanetaryMass();
		void setInverseMass(float mass);
		void setVelocity(glm::vec3 newVel);
		glm::vec3 getVelocity();
		glm::vec3 getAcceleration();

		// Planet variables
		void changePlanet(std::shared_ptr<particle> other);
		void changeLimit(float amt);
		bool planetExists();
		std::shared_ptr<particle> getPlanet();

		// Planet properties
		float distLimit = 50.0f;
		float planetaryMass = 1.0f;
	private:
		// Transformation variables
		glm::mat4 trans;
		GLuint& normalTrans;
		GLuint& modelTrans;

		// Position variables
		glm::vec3 position = glm::vec3(0.0f);

		// Life variables
		float life = 0.0f;
		
		// Physics variables
		glm::vec3 velocity = glm::vec3(0.0f);
		glm::vec3 acceleration = glm::vec3(0.0f);
		glm::vec3 forceAccum = glm::vec3(0.0f);

		// Particle properties
		ObjData obj;
		float inverseMass = 0.0f;
		float mass = 0.0f;
		float damping = 0.99f;

		// Planet properties
		std::shared_ptr<particle> other = NULL;
};

#endif
