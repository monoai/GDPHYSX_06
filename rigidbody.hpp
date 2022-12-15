#pragma once
#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/quaternion.hpp>

#include "obj_mesh.h"

class rigidBody {
	public:
		bool inUse = false;
		rigidBody(GLuint* normalTransformLoc, GLuint* modelTransformLoc, ObjData object);

		void calculateDerivedData();
		void calculateTransMat(glm::mat4 trans, glm::vec3 pos, glm::quat quat);
		void transformInertiaTensor(glm::mat3 iiTensorWorld, glm::mat3 iiTensorBody, glm::mat4 rotMat);
		void draw();
		void update(float dT);
		void clearAccums();
		void addForce(glm::vec3 force); // Done for formality
		void addForceAtBodyPoint(glm::vec3 force, glm::vec3 point);
		void addForceAtPoint(glm::vec3 force, glm::vec3 point);

		void setInertiaTensor(glm::mat3 inertiaTensor);
		glm::vec3 getPointInLocalSpace(glm::vec3 point);
		glm::vec3 getPointInWorldSpace(glm::vec3 point);
		glm::vec3 getLastFrameAcceleration();
		void setPosition(glm::vec3 vector);
		glm::vec3 getPosition();

		float getInverseMass();
		void setMass(float mass);
		float getMass();
		void setVelocity(glm::vec3 newVel);
		glm::vec3 getVelocity();
		glm::vec3 getAcceleration();

	private:
		// Transformation variables
		glm::mat4 trans = glm::mat4(1.0f);
		GLuint& normalTrans;
		GLuint& modelTrans;

		// Position variables
		glm::vec3 position = glm::vec3(0.0f);

		// Life variables
		float life = 0.0f;
		bool isUpdate;
		
		// Physics variables
		glm::vec3 velocity = glm::vec3(0.0f);
		glm::vec3 acceleration = glm::vec3(0.0f);
		glm::vec3 lastFrameAcceleration = glm::vec3(0.0f);
		glm::vec3 forceAccum = glm::vec3(0.0f);
		glm::vec3 torqueAccum = glm::vec3(0.0f);

		// Particle properties
		ObjData obj;
		float inverseMass = 0.0f;
		float mass = 0.0f;
		float damping = 0.99f;
		float angularDamping = 0.99f;
		float angularVelocity;
		glm::vec3 rotation = glm::vec3(0.0f);
		glm::quat orientation;

		glm::mat3 inverseInertiaTensor;
		glm::mat3 inverseInertiaTensorWorld;
};

#endif
