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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// Global variables
float forceFactor = 0.0f;
bool forceEnabled = false;
bool gravityEnabled = false;
bool spawnEnabled = false;

float xVelocity = 3.5f;
float yVelocity = 0.0f;
float xAcceleration = 0.0f;
float yAcceleration = -0.1f;

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

	// var for Physics
	float xPos = 0.0f;
	float yPos = 0.0f;
	float zPos = 0.0f;
	float xVel = 0.0f;
	float yVel = 0.0f;
	//float gravity = 9.8f*0.05f; //Dampening gravity because too strong
	float gravity = 0.05f;

	//Camera vec vars
	//Perspective Vecs
	glm::vec3 eye = glm::vec3(-7.0f, 11.5f, 33.0f);
	glm::vec3 center = glm::vec3(1.5f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	bool ortho = false;
	float test = 1.0f;

	std::vector<particle> particles;

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	//depth testing
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_ALWAYS); // set the depth test function

	//face culling
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK); // set which face to cull
	//glFrontFace(GL_CCW); // set the front face orientation

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
		/*
		if (ortho == false) {
			view = glm::lookAt(
				glm::vec3(0.0f, 10.0f, -20.0f),
				center,
				glm::vec3(0.0f, 1.0f, 0.0f)
			);
		}
		else {
			view = glm::lookAt(
				eye,
				center,
				//glm::vec3(trans[3][0], trans[3][1], trans[3][2]),
				up
			);
		}
		*/
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

		//-----draw Sun-----
		glBindVertexArray(planet.vaoId);
		glUseProgram(shaderProgram);

		if(particles.size() > 0) {
			for(int i = 0; i < particles.size(); i++) {
				//particles[i].setPosition(xPos, yPos, zPos);
				particles[i].update();
				particles[i].draw(planet);
				//std::cout << "Should be doing smth" << std::endl;
			}
		}

		//place lighting
		//glUniform3f(lightPosLoc, trans[0][0], trans[0][1], trans[0][2]);

		glUniform3f(ambientColorLoc, 1.0f, 1.0f, 1.0f);

		//glUseProgram(shaderProgram);

		// Update Loop
		// Loops via deltaTime and the Semi-fixed timestep design
		//currentTime = glfwGetTime();
		//deltaTime = currentTime - prevTime;
		float newTime = glfwGetTime();
		float frameTime = newTime - currentTime;
		currentTime = newTime;
		while(frameTime >0.0f) {
			float deltaTime = std::min(frameTime, dT);
			frameTime -= deltaTime;

			// Desperate debug values
			//std::cout << "xPos: " << xPos << std::endl;
			//std::cout << "yPos: " << yPos << std::endl;
			//std::cout << "forceFactor: " << forceFactor << std::endl;
			if(spawnEnabled==true) {
				particle totesNew(&trans, &normalTransformLoc, &modelTransformLoc, planet);
				totesNew.setPosition(0.0f,0.0f,0.0f);
				totesNew.setParticleParams(xVelocity, yVelocity, xAcceleration, yAcceleration);
				particles.push_back(totesNew);
				std::cout << "Pushed back!" << std::endl;
				spawnEnabled = false;
			}
		}
		

		//--- stop drawing here ---
#pragma endregion

		glfwSwapBuffers(window);
		//listen for glfw input events
		glfwPollEvents();
		
		/* Camera Controls
		*/
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
	}
	return 0;
}
void resetValues() {
	xVelocity = 0.0f;
	yVelocity = 0.0f;
	xAcceleration = 0.0f;
	yAcceleration = 0.0f;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	// 1 - Pistol
	if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		xVelocity = 3.5f;
		yVelocity = 0.0f;
		xAcceleration = 0.0f;
		yAcceleration = -0.1f;
		std::cout << "Pistol set" << std::endl;
	}
	// 2 - Artillery
	if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		xVelocity = 4.0f;
		yVelocity = 3.0f;
		xAcceleration = 0.0f;
		yAcceleration = -2.0f;
		std::cout << "Artillery set" << std::endl;
	}
	// 3 - Fireball
	if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		xVelocity = 1.0f;
		yVelocity = 0.0f;
		xAcceleration = 0.0f;
		yAcceleration = 0.06f;
	}
	// 4 - Laser
	if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
		xVelocity = 7.0f;
		yVelocity = 0.0f;
		xAcceleration = 0.0f;
		yAcceleration = 0.0f;
	}
	// 5 - Fireworks
	if(glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
		xVelocity = 0.0f;
		yVelocity = 1.0f;
		xAcceleration = 0.0f;
		yAcceleration = 0.3f;
	}
}



void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
	// onClick
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		spawnEnabled = true;	
	}
}
