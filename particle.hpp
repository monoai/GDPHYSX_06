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
		void draw(ObjData obj);
		void setPosition(float x, float y, float z);
		void update();
	private:
		glm::mat4& trans;
		GLuint& normalTrans;
		GLuint& modelTrans;
		float xPos = 0.0f;
		float yPos = 0.0f;
		float zPos = 0.0f;

};

#endif
