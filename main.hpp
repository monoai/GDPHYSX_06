#pragma once
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <memory>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "glm/glm.hpp"
#include "obj_mesh.h"
#include "shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "skybox.h"
#include "glm/gtx/string_cast.hpp"
#include "particle.hpp"
#include "particlefgen.hpp"
#include "particlefpool.hpp"
#include "particleworld.hpp"
#include "particlecontact.hpp"
#include "particlelink.hpp"

//Shaders
GLuint skyboxShaderProgram;
GLuint shaderProgram;
GLuint colorLoc;

GLuint modelTransformLoc;
GLuint viewLoc;
GLuint projectionLoc;

GLuint normalTransformLoc;
GLuint cameraPosLoc;
GLuint ambientColorLoc;

//ObjMesh Related
obj_mesh mesh;
ObjData planet;
SkyboxData skybox;

// Global variables
bool debugI = false;
bool debugO = false;
bool debugP = false;

// Enum Declaration
enum springType { NONE, BASIC, ANCHOR, ELASTIC };
springType spring;
particle::particleName particleType = particle::particleName::PISTOL;

// var for Time
double currentTime = glfwGetTime();
double accumulator = 0.0;

double t = 0.0f;
double dT = 0.01f;

//Camera vec vars
//Perspective Vecs
glm::vec3 eye = glm::vec3(-7.0f, 11.5f, 33.0f);
glm::vec3 center = glm::vec3(0.5f, 0.0f, -1.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

// World variables
// particleForcePool particlepool;
particleWorld world;

// Custom unique variables
std::shared_ptr<particle> planet1;
std::shared_ptr<particle> planet2;
float distLimit = 50.0f;
int unqCount;
