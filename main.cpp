#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdlib.h>
#include <algorithm>
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// Global variables
bool spawnEnabled = false;

// Enum Declaration
enum springType{ NONE, BASIC, ANCHOR, ELASTIC};
springType spring;

particle::particleName particleType = particle::particleName::PISTOL;

int main() {
	//stbi_set_flip_vertically_on_load(true);
#pragma region Initialization
	//initialize glfw
	if (glfwInit() != GLFW_TRUE) {
		fprintf(stderr, "Failed to initialized! \n");
		return -1;
	}

	// set opengl version to 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create window 
	GLFWwindow* window;
	window = glfwCreateWindow(1024, 768, "GDPHYSX_Particle", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to load window! \n");
		return -1;
	}
	glfwMakeContextCurrent(window);

	//initialize glew
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
#pragma endregion


#pragma region Mesh Loading
	obj_mesh mesh; // Initialize class

	ObjData planet;
	mesh.LoadObjFile(&planet, "planets/Earth.obj");
	GLfloat earthOffsets[] = { 0.0f, 0.0f, 0.0f };
	mesh.LoadObjToMemory(
		&planet,
		1.0f,
		earthOffsets
	);

	// Load Skybox Model
	std::vector<std::string> faces{
		"right.png",
		"left.png",
		"top.png",
		"bottom.png",
		"front.png",
		"back.png"
	};
	SkyboxData skybox = LoadSkybox("Assets/skybox", faces);
#pragma endregion

#pragma region Shader Loading

	//Skybox Shaders
	GLuint skyboxShaderProgram = LoadShaders("Shaders/skybox_vertex.shader", "Shaders/skybox_fragment.shader");
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Normal Shaders
	GLuint shaderProgram = LoadShaders("Shaders/phong_vertex.shader", "Shaders/phong_normal_fragment.shader");
	glUseProgram(shaderProgram);

	GLuint simpleShader = LoadShaders("Shaders/vertex.shader", "Shaders/fragment.shader");

	GLuint colorLoc = glGetUniformLocation(shaderProgram, "u_color");
	glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);


	// initialize MVP
	GLuint modelTransformLoc = glGetUniformLocation(shaderProgram, "u_model");
	GLuint viewLoc = glGetUniformLocation(shaderProgram, "u_view");
	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "u_projection");

	// initialize normal transformation
	GLuint normalTransformLoc = glGetUniformLocation(shaderProgram, "u_normal");
	GLuint cameraPosLoc = glGetUniformLocation(shaderProgram, "u_camera_pos");
	GLuint ambientColorLoc = glGetUniformLocation(shaderProgram, "u_ambient_color");
	//glUniform3f(ambientColorLoc, 1.0f, 1.0f, 1.0f);

	glm::mat4 trans = glm::mat4(1.0f); // identity
	glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans));

	// normal stuff
	glm::mat4 normalTrans = glm::transpose(glm::inverse(trans));

	// define projection matrix
	glm::mat4 projection = glm::mat4(1.0f);
	//glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//setup shading
	GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "u_light_pos");

#pragma endregion

#pragma region Prerender Preparations

	// set bg color to green
	glClearColor(0.4f, 0.4f, 0.0f, 0.0f);//float gravity = 9.8f*0.05f; //Dampening gravity because too strong

	// var for Time
	float currentTime = glfwGetTime();
	float prevTime = 0.0f;
	float dT = 1.0f / 60.0f;


	//Camera vec vars
	//Perspective Vecs
	glm::vec3 eye = glm::vec3(-7.0f, 11.5f, 33.0f);
	glm::vec3 center = glm::vec3(0.5f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	bool ortho = false;

	//std::vector<particle> particles;
	particleForcePool particlepool;

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	//depth testing
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_ALWAYS); // set the depth test function

	//face culling
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK); // set which face to cull
	//glFrontFace(GL_CCW); // set the front face orientation

#pragma endregion

	while (!glfwWindowShouldClose(window)) {

#pragma region Viewport
		float ratio;
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);
#pragma endregion

#pragma region Projection
		// Orthopgraphic projection but make units same as pixels. origin is lower left of window
		// projection = glm::ortho(0.0f, (GLfloat)width, 0.0f, (GLfloat)height, 0.1f, 10.0f); // when using this scale objects really high at pixel unity size

		// Orthographic with stretching
		//projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 10.0f);

		// Orthographic with corection for stretching, resize window to see difference with previous example
		//projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.1f, 10.0f);

		/*
		*/
		float zoomFactor = 20.0f;

		// Perspective Projection
		if (ortho == false) {
			projection = glm::perspective(glm::radians(90.0f), ratio, 0.1f, 500.0f);
		}
		else {
			projection = glm::ortho(-ratio - zoomFactor, ratio + zoomFactor, -1.0f - zoomFactor, 1.0f + zoomFactor, -20.1f, 120.0f);
		}

		// Set projection matrix in shader
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

#pragma endregion

#pragma region View
		glm::mat4 view;

		view = glm::lookAt(eye, eye + center, up);

		glUniform3f(cameraPosLoc, eye.x, eye.y, eye.z);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		//std::cout << "Eye\t" << glm::to_string(eye) << std::endl;
		//std::cout << "Center\t" << glm::to_string(center) << std::endl;
		//std::cout << "Up\t" << glm::to_string(up) << std::endl;


#pragma endregion

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//toggle to render wit GL_FILL or GL_LINE
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#pragma region Draw

		DrawSkybox(skybox, skyboxShaderProgram, view, projection);

		//-----draw particle-----
		glBindVertexArray(planet.vaoId);
		glUseProgram(shaderProgram);
		glUniform3f(ambientColorLoc, 1.0f, 1.0f, 1.0f);

		// Update Loop
		// Loops via deltaTime and the Semi-fixed timestep design
		float newTime = glfwGetTime();
		float frameTime = newTime - currentTime;
		currentTime = newTime;
		while(frameTime >0.0f) {
			float deltaTime = std::min(frameTime, dT);
			frameTime -= deltaTime;
			
			/*
			if(particles.size() > 0) {
				for(int i = 0; i < particles.size(); i++) {
					if(particles[i].inUse == true) {
						//particles[i].setPosition(xPos, yPos, zPos);
						particles[i].update(deltaTime);
						particles[i].draw(planet);
						//std::cout << "Should be doing smth" << std::endl;
					}
				}
			}
			*/
			
			if(particlepool.getSize() > 0) {
				particlepool.updateForces(deltaTime);
				particlepool.update(deltaTime);
				particlepool.draw(planet);
			}
			

			if(spawnEnabled==true) {
				particle* fixedPoint = new particle(&normalTransformLoc, &modelTransformLoc, planet);
				fixedPoint->setPosition(glm::vec3(1.0f,1.0f,0.0f));

				particle* totesNew = new particle(&normalTransformLoc, &modelTransformLoc, planet);
				totesNew->setParticleParams(particleType);
				totesNew->setPosition(glm::vec3(6.5,0.5f,0.0f));
				totesNew->inUse = true;

				/*	
				glm::vec3 acceleration = totesNew->getAcceleration();	
				particleGravity* gpart = new particleGravity(acceleration);
				particlepool.add(totesNew, gpart);
				*/
				
				switch(spring)
				{
				case BASIC: {
					particleSpring* springParticle = new particleSpring(fixedPoint,2.0f,3.0f);
					particlepool.add(totesNew, springParticle);
					//particleSpring* springParticleb = new particleSpring(totesNew, 1.0f,2.0f);
					//particlepool.add(fixedPoint, springParticle);
					}
					break;
				case ANCHOR: {
					glm::vec3 fixedPos = fixedPoint->getPosition();
					particleAnchoredSpring* anchoredSpring = new particleAnchoredSpring(fixedPos, 2.0f, 3.0f);
					particlepool.add(totesNew, anchoredSpring);
					}
					break;
				case ELASTIC: {
					particleElasticBungee* elasticBungee = new particleElasticBungee(fixedPoint, 2.0f, 5.0f);
					particlepool.add(totesNew, elasticBungee);
					//particleElasticBungee* elasticBungeeb = new particleElasticBungee(totesNew, 2.0f,5.0f);
					//particlepool.add(fixedPoint, elasticBungeeb);
					}
				case NONE: {
					// Do nothing
					}
					break;
				}

				//particles.push_back(totesNew);
				//std::cout << "Pushed back!" << std::endl;
				spawnEnabled = false;
			}
		}
		//--- stop drawing here ---
#pragma endregion

		glfwSwapBuffers(window);
		//listen for glfw input events
		glfwPollEvents();
		
		/* 
		Camera Controls
		float camSpeed = 0.05f;
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			eye += camSpeed * center;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			eye -= camSpeed * center;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			eye -= glm::normalize(glm::cross(center, up)) * camSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			eye += glm::normalize(glm::cross(center, up)) * camSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
			eye += camSpeed * up;
		}
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
			eye -= camSpeed * up;
		}
		*/
	}
	return 0;
}
/* [resetValues FUNCTION]
 * Reverts all global variables of a particle to 0.0f 
 */
void resetValues() {
	particleType = particle::UNKNOWN;
}

/* [User Inputs FUNCTIONS]
 * These functions contains the input functionalities of the program
 */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	// 1 - Pistol
	if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		particleType = particle::PISTOL;
	}
	// 2 - Artillery
	if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		particleType = particle::ARTILLERY;
	}
	// 3 - Fireball
	if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		particleType = particle::FIREBALL;
	}
	// 4 - Laser
	if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
		particleType = particle::LASER;
	}
	// 5 - Fireworks
	if(glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
		std::cout << "BASIC SPRING SET" << std::endl;
		spring = BASIC;
	}	
	if(glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
		std::cout << "ANCHOR SPRING SET" << std::endl;
		spring = ANCHOR;
	}
	if(glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
		std::cout << "ELASTIC SPRING SET" << std::endl;
		spring = ELASTIC;
	}
	if(glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {
		std::cout << "SPRING REMOVED" << std::endl;
		spring = NONE;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
	// onClick
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		spawnEnabled = true;	
	}
}
