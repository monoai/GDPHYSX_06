#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"
#include "obj_mesh.h"
#include "shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "skybox.h"

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

	ObjData planet;
	LoadObjFile(&planet, "planets/Earth.obj");
	GLfloat earthOffsets[] = { 0.0f, 0.0f, 0.0f };
	LoadObjToMemory(
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

	// set bg color to green
	glClearColor(0.4f, 0.4f, 0.0f, 0.0f);

	// var for rotations
	float rotFactor = 0.0f;
	float xFactor = 0.0f;
	float xSpeed = 1.0f;
	float currentTime = glfwGetTime();
	float prevTime = 0.0f;
	float deltaTime = 0.0f;

	//Camera vec vars
	//Perspective Vecs
	glm::vec3 eye = glm::vec3(0.0f, 0.0f, 6.0f);
	glm::vec3 center = glm::vec3(trans[3][0], trans[3][1], trans[3][2]);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	bool ortho = false;
	float test = 1.0f;

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

		float zoomFactor = 20.0f;

		// Perspective Projection
		if (ortho == false) {
			projection = glm::perspective(glm::radians(90.0f), ratio, 0.1f, 60.0f);
		}
		else {
			projection = glm::ortho(-ratio - zoomFactor, ratio + zoomFactor, -1.0f - zoomFactor, 1.0f + zoomFactor, -20.1f, 60.0f);
		}

		// Set projection matrix in shader
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

#pragma endregion

#pragma region View
		glm::mat4 view;


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
		glUniform3f(cameraPosLoc, eye.x, eye.y, eye.z);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
#pragma endregion

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//toggle to render wit GL_FILL or GL_LINE
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#pragma region Draw

		DrawSkybox(skybox, skyboxShaderProgram, view, projection);

		//-----draw Sun-----
		glBindVertexArray(planet.vaoId);
		glUseProgram(shaderProgram);

		// transforms
		trans = glm::mat4(1.0f); // identity
		trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, 0.0f)); // matrix * translate_matrix
		trans = glm::rotate(trans, glm::radians(rotFactor), glm::vec3(0.0f, 1.0f, 0.0f)); // matrix * rotation_matrix
		trans = glm::scale(trans, glm::vec3(1.5f, 1.5f, 1.5f));

		//send to shader
		normalTrans = glm::transpose(glm::inverse(trans));
		glUniformMatrix4fv(normalTransformLoc, 1, GL_FALSE, glm::value_ptr(normalTrans));
		glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		glActiveTexture(GL_TEXTURE0);
		GLuint sunTexture = planet.textures[planet.materials[0].diffuse_texname];
		glBindTexture(GL_TEXTURE_2D, sunTexture);

		//drawbackpack
		glDrawElements(GL_TRIANGLES, planet.numFaces, GL_UNSIGNED_INT, (void*)0);

		//unbindtexture after rendering
		glBindTexture(GL_TEXTURE_2D, 0);

		//Re-use normal shaders
		glUseProgram(shaderProgram);

		//place lighting
		glUniform3f(lightPosLoc, trans[0][0], trans[0][1], trans[0][2]);

		glUniform3f(ambientColorLoc, 0.2f, 0.2f, 0.2f);

		//-----draw Earth-----

		glBindVertexArray(planet.vaoId);

		// transforms
		//trans = glm::mat4(1.0f); // identity
		trans = glm::translate(trans, glm::vec3(-9.0f, 0.0f, 0.0f)); // matrix * translate_matrix
		trans = glm::rotate(trans, glm::radians(rotFactor), glm::vec3(0.0f, 1.0f, 0.0f)); // matrix * rotation_matrix
		trans = glm::scale(trans, glm::vec3(0.6f, 0.6f, 0.6f));

		//send to shader
		normalTrans = glm::transpose(glm::inverse(trans));
		glUniformMatrix4fv(normalTransformLoc, 1, GL_FALSE, glm::value_ptr(normalTrans));
		glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		glActiveTexture(GL_TEXTURE0);
		GLuint earthTexture = planet.textures[planet.materials[1].diffuse_texname];
		glBindTexture(GL_TEXTURE_2D, earthTexture);

		//drawbackpack
		glDrawElements(GL_TRIANGLES, planet.numFaces, GL_UNSIGNED_INT, (void*)0);

		//unbindtexture after rendering
		glBindTexture(GL_TEXTURE_2D, 0);

		//-----draw Moon-----
		glBindVertexArray(planet.vaoId);

		// transforms
		//trans = glm::mat4(1.0f); // identity
		trans = glm::translate(trans, glm::vec3(-5.0f, 0.0f, 0.0f)); // matrix * translate_matrix
		//trans = glm::rotate(trans, glm::radians(rotFactor), glm::vec3(0.0f, 1.0f, 0.0f)); // matrix * rotation_matrix
		trans = glm::scale(trans, glm::vec3(0.3f, 0.3f, 0.3f));
		//send to shader
		normalTrans = glm::transpose(glm::inverse(trans));
		glUniformMatrix4fv(normalTransformLoc, 1, GL_FALSE, glm::value_ptr(normalTrans));
		glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		glActiveTexture(GL_TEXTURE0);
		GLuint moonTexture = planet.textures[planet.materials[2].diffuse_texname];
		glBindTexture(GL_TEXTURE_2D, moonTexture);

		//drawbackpack
		glDrawElements(GL_TRIANGLES, planet.numFaces, GL_UNSIGNED_INT, (void*)0);

		//unbindtexture after rendering
		glBindTexture(GL_TEXTURE_2D, 0);

		// incerement rotation by deltaTime
		currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;
		xFactor += deltaTime * xSpeed;
		if (xFactor > 1.0f) {
			xSpeed = -1.0f;
		}
		else if (xFactor < -1.0f) {
			xSpeed = 1.0f;
		}
		rotFactor += (deltaTime * 0.2f) * 360.0f;
		if (rotFactor > 360.0f) {
			rotFactor -= 360.0f;
		}
		prevTime = currentTime;

		glUseProgram(shaderProgram);


		//--- stop drawing here ---
#pragma endregion

		glfwSwapBuffers(window);
		//listen for glfw input events
		glfwPollEvents();

		//Perspective
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			ortho = false;
			eye = glm::vec3(0.0f, 0.0f, 6.0f);
			center = glm::vec3(trans[1][0], trans[1][1], trans[1][2]);
			up = glm::vec3(0.0f, 1.0f, 0.0f);
		}
		//Orthographic
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			ortho = true;
			eye = glm::vec3(0.0f, -10.0f, 0.0f);
			center = glm::vec3(trans[0][0], trans[0][1] + 60.0f, trans[0][2]);
			up = glm::vec3(0.0f, 1.0f, 0.0f);
		}
	}
	return 0;
}
