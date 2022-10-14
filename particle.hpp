#pragma once
#ifndef PARTICLE_H
#define PARTICLE_H

#include <GL/glew.h>
#include <glm/glm.hpp>
//#include "obj_mesh.h"

struct ObjData;

class particle {
	public:
		particle(glm::mat4* transMat, GLuint* normalTransformLoc, GLuint* modelTransformLoc, ObjData object);
		~particle();
		void draw(ObjData obj);
		void setPosition(float x, float y, float z);
		void update(float dT);
		void setParticleParams(float xVelocity, float yVelocity, float xAcceleration, float yAcceleration, float mass, float damping);
		bool inUse;
	private:
		// Transformation variables
		glm::mat4& trans;
		GLuint& normalTrans;
		GLuint& modelTrans;

		// Position variables
		float xPos = 0.0f;
		float yPos = 0.0f;
		float zPos = 0.0f;

		// Time variables
		float deltaTime = 0.0f;
		float life = 0.0f;
		
		// Physics variables
		float xVelocity = 0.0f;
		float yVelocity = 0.0f;
		float xAcceleration = 0.0f;
		float yAcceleration = 0.0f;

		float mass = 0.0f;
		float damping = 0.0f;
};

#endif
