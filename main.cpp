#include "main.hpp"

#include <glm/gtx/string_cast.hpp>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

int main() {
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

	//Load Planet Model
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
	skybox = LoadSkybox("Assets/skybox", faces);
#pragma endregion

#pragma region Shader Loading

	//Skybox Shaders
	skyboxShaderProgram = LoadShaders("Shaders/skybox_vertex.shader", "Shaders/skybox_fragment.shader");
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Normal Shaders
	shaderProgram = LoadShaders("Shaders/phong_vertex.shader", "Shaders/phong_normal_fragment.shader");
	glUseProgram(shaderProgram);

	colorLoc = glGetUniformLocation(shaderProgram, "u_color");
	glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);

	// initialize MVP
	modelTransformLoc = glGetUniformLocation(shaderProgram, "u_model");
	viewLoc = glGetUniformLocation(shaderProgram, "u_view");
	projectionLoc = glGetUniformLocation(shaderProgram, "u_projection");

	// initialize normal transformation
	normalTransformLoc = glGetUniformLocation(shaderProgram, "u_normal");
	cameraPosLoc = glGetUniformLocation(shaderProgram, "u_camera_pos");
	ambientColorLoc = glGetUniformLocation(shaderProgram, "u_ambient_color");

	glm::mat4 trans = glm::mat4(1.0f); // identity
	glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans));

	// define projection matrix
	glm::mat4 projection = glm::mat4(1.0f);

	//setup shading
	//GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "u_light_pos");

#pragma endregion

#pragma region Prerender Preparations

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

#pragma region Prerender Spawnings
	// This region's purpose is mostly/usually debugging
	// Could also be used to declare objects that should only spawn once
	
	// Contact test
	/*
	std::shared_ptr<particle> testParticle(new particle(&normalTransformLoc, &modelTransformLoc, planet));
	testParticle->setPosition(glm::vec3(50.0,0.0f,0.0f));
	testParticle->setMass(1.5f);

	world.getParticlePool().push_back(testParticle);
	*/
	/*
	particleContact contact = particleContact();
	contact._particle[0] = testParticle;
	contact._particle[1] = NULL;
	*/

	// Making a box out of particles
	float dist = 50.0f;
	std::vector<std::shared_ptr<particle>> boxParticlePool;
	std::shared_ptr<particle> boxParticle(new particle(&normalTransformLoc, &modelTransformLoc, planet));

	// The part where i set position and push new copies into a vector
	boxParticle->setPosition(glm::vec3(dist+3.5f,-3.5f,3.5f)); //0-A
	boxParticlePool.push_back(boxParticle);
	boxParticle.reset(new particle(&normalTransformLoc, &modelTransformLoc, planet));
	boxParticle->setPosition(glm::vec3(dist+3.5f,-3.5f,-3.5f)); //1-B
	boxParticlePool.push_back(boxParticle);
	boxParticle.reset(new particle(&normalTransformLoc, &modelTransformLoc, planet));
	boxParticle->setPosition(glm::vec3(dist+3.5f,3.5f,3.5f)); //2-C
	boxParticlePool.push_back(boxParticle);
	boxParticle.reset(new particle(&normalTransformLoc, &modelTransformLoc, planet));
	boxParticle->setPosition(glm::vec3(dist+3.5f,3.5f,-3.5f)); //3-D
	boxParticlePool.push_back(boxParticle);
	boxParticle.reset(new particle(&normalTransformLoc, &modelTransformLoc, planet));
	boxParticle->setPosition(glm::vec3(dist+-3.5f,-3.5f,3.5f)); //4-E
	boxParticlePool.push_back(boxParticle);
	boxParticle.reset(new particle(&normalTransformLoc, &modelTransformLoc, planet));
	boxParticle->setPosition(glm::vec3(dist+-3.5f,-3.5f,-3.5f)); //5-F
	boxParticlePool.push_back(boxParticle);
	boxParticle.reset(new particle(&normalTransformLoc, &modelTransformLoc, planet));
	boxParticle->setPosition(glm::vec3(dist+-3.5f,3.5f,3.5f)); //6-G
	boxParticlePool.push_back(boxParticle);
	boxParticle.reset(new particle(&normalTransformLoc, &modelTransformLoc, planet));
	boxParticle->setPosition(glm::vec3(dist+-3.5f,3.5f,-3.5f)); //7-H
	boxParticlePool.push_back(boxParticle);
	boxParticle.reset(new particle(&normalTransformLoc, &modelTransformLoc, planet));

	// Initializing its values
	for(long unsigned int i=0; i < boxParticlePool.size(); i++) {
		boxParticlePool[i]->dontDelete = true;
		boxParticlePool[i]->radius = 5.0f;
		boxParticlePool[i]->setMass(1.0f);
		boxParticlePool[i]->setVelocity(glm::vec3(0.0f));
		boxParticlePool[i]->clearForceAccum();
	}

	// Initializing rods
	std::vector<std::shared_ptr<particleRod>> rodsPool;
	std::shared_ptr<particleRod> rods(new particleRod());

	// Vertical Sides
	rods->_particle[0] = boxParticlePool[0];
	rods->_particle[1] = boxParticlePool[1];
	rods->length = glm::distance(boxParticlePool[0]->getPosition(), boxParticlePool[1]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());	
	rods->_particle[0] = boxParticlePool[2];
	rods->_particle[1] = boxParticlePool[3];
	rods->length = glm::distance(boxParticlePool[2]->getPosition(), boxParticlePool[3]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());
	rods->_particle[0] = boxParticlePool[4];
	rods->_particle[1] = boxParticlePool[5];
	rods->length = glm::distance(boxParticlePool[4]->getPosition(), boxParticlePool[5]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());
	rods->_particle[0] = boxParticlePool[6];
	rods->_particle[1] = boxParticlePool[7];
	rods->length = glm::distance(boxParticlePool[6]->getPosition(), boxParticlePool[7]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());

	// Horizontal Sides
	// Top
	rods->_particle[0] = boxParticlePool[0];
	rods->_particle[1] = boxParticlePool[2];
	rods->length = glm::distance(boxParticlePool[0]->getPosition(), boxParticlePool[2]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());
	rods->_particle[0] = boxParticlePool[0];
	rods->_particle[1] = boxParticlePool[4];
	rods->length = glm::distance(boxParticlePool[0]->getPosition(), boxParticlePool[4]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());
	rods->_particle[0] = boxParticlePool[6];
	rods->_particle[1] = boxParticlePool[2];
	rods->length = glm::distance(boxParticlePool[6]->getPosition(), boxParticlePool[2]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());
	rods->_particle[0] = boxParticlePool[6];
	rods->_particle[1] = boxParticlePool[4];
	rods->length = glm::distance(boxParticlePool[6]->getPosition(), boxParticlePool[4]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());

	// Bottom
	rods->_particle[0] = boxParticlePool[1];
	rods->_particle[1] = boxParticlePool[3];
	rods->length = glm::distance(boxParticlePool[1]->getPosition(), boxParticlePool[3]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());
	rods->_particle[0] = boxParticlePool[1];
	rods->_particle[1] = boxParticlePool[5];
	rods->length = glm::distance(boxParticlePool[1]->getPosition(), boxParticlePool[5]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());
	rods->_particle[0] = boxParticlePool[7];
	rods->_particle[1] = boxParticlePool[3];
	rods->length = glm::distance(boxParticlePool[7]->getPosition(), boxParticlePool[3]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());
	rods->_particle[0] = boxParticlePool[7];
	rods->_particle[1] = boxParticlePool[5];
	rods->length = glm::distance(boxParticlePool[7]->getPosition(), boxParticlePool[5]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());

	/*
	// Inbetweens
	// Inner
	rods->_particle[0] = boxParticlePool[0];
	rods->_particle[1] = boxParticlePool[7];
	rods->length = glm::distance(boxParticlePool[0]->getPosition(), boxParticlePool[7]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());
	rods->_particle[0] = boxParticlePool[1];
	rods->_particle[1] = boxParticlePool[6];
	rods->length = glm::distance(boxParticlePool[1]->getPosition(), boxParticlePool[6]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());
	rods->_particle[0] = boxParticlePool[3];
	rods->_particle[1] = boxParticlePool[4];
	rods->length = glm::distance(boxParticlePool[3]->getPosition(), boxParticlePool[4]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());
	rods->_particle[0] = boxParticlePool[2];
	rods->_particle[1] = boxParticlePool[5];
	rods->length = glm::distance(boxParticlePool[2]->getPosition(), boxParticlePool[5]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());
	*/

	
	// Sides
	rods->_particle[0] = boxParticlePool[0];
	rods->_particle[1] = boxParticlePool[3];
	rods->length = glm::distance(boxParticlePool[0]->getPosition(), boxParticlePool[3]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());
	rods->_particle[0] = boxParticlePool[2];
	rods->_particle[1] = boxParticlePool[1];
	rods->length = glm::distance(boxParticlePool[2]->getPosition(), boxParticlePool[1]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());
	rods->_particle[0] = boxParticlePool[0];
	rods->_particle[1] = boxParticlePool[5];
	rods->length = glm::distance(boxParticlePool[0]->getPosition(), boxParticlePool[5]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());
	rods->_particle[0] = boxParticlePool[4];
	rods->_particle[1] = boxParticlePool[1];
	rods->length = glm::distance(boxParticlePool[4]->getPosition(), boxParticlePool[1]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());

	rods->_particle[0] = boxParticlePool[6];
	rods->_particle[1] = boxParticlePool[3];
	rods->length = glm::distance(boxParticlePool[6]->getPosition(), boxParticlePool[3]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());
	rods->_particle[0] = boxParticlePool[2];
	rods->_particle[1] = boxParticlePool[7];
	rods->length = glm::distance(boxParticlePool[2]->getPosition(), boxParticlePool[7]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());
	rods->_particle[0] = boxParticlePool[6];
	rods->_particle[1] = boxParticlePool[5];
	rods->length = glm::distance(boxParticlePool[6]->getPosition(), boxParticlePool[5]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());
	rods->_particle[0] = boxParticlePool[4];
	rods->_particle[1] = boxParticlePool[7];
	rods->length = glm::distance(boxParticlePool[4]->getPosition(), boxParticlePool[7]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());

	// Top/Bottom
	rods->_particle[0] = boxParticlePool[1];
	rods->_particle[1] = boxParticlePool[7];
	rods->length = glm::distance(boxParticlePool[1]->getPosition(), boxParticlePool[7]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());
	rods->_particle[0] = boxParticlePool[3];
	rods->_particle[1] = boxParticlePool[5];
	rods->length = glm::distance(boxParticlePool[3]->getPosition(), boxParticlePool[5]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());

	rods->_particle[0] = boxParticlePool[0];
	rods->_particle[1] = boxParticlePool[6];
	rods->length = glm::distance(boxParticlePool[0]->getPosition(), boxParticlePool[6]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());
	rods->_particle[0] = boxParticlePool[2];
	rods->_particle[1] = boxParticlePool[4];
	rods->length = glm::distance(boxParticlePool[2]->getPosition(), boxParticlePool[4]->getPosition());
	rodsPool.push_back(rods);
	rods.reset(new particleRod());

	// Pushing to the world's contact generators
	for(long unsigned int i = 0; i < rodsPool.size(); i++) {
		//std::cout << "[DEBUG] - BoxParticleA: " << rodsPool[i]->_particle[0] << std::endl;
		//std::cout << "[DEBUG] - BoxParticleB: " << rodsPool[i]->_particle[1] << std::endl;
		//std::cout << "[DEBUG] - BoxDistance: " << rodsPool[i]->length << std::endl;
		world.getContactGenPool().push_back(rodsPool[i]);
	}
	for(long unsigned int i = 0; i < boxParticlePool.size(); i++) {
		//std::cout << "[DEBUG] - BoxPPosition: " << glm::to_string(boxParticlePool[i]->getPosition()) << std::endl;
		world.getParticlePool().push_back(boxParticlePool[i]);
	}

	///* Testing rigidbodies
	std::shared_ptr<rigidBody> test(new rigidBody(&normalTransformLoc, &modelTransformLoc, planet));
	test->setPosition(glm::vec3(35.5f, 0.0f, 0.0f));
	//test->calculateDerivedData();
	test->inUse = true;
	rbodyWorld.getRBodyPool().push_back(test);

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

		projection = glm::perspective(glm::radians(90.0f), ratio, 0.1f, 500.0f);

		// Set projection matrix in shader
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

#pragma endregion

#pragma region View
		glm::mat4 view;

		view = glm::lookAt(eye, eye + center, up);

		glUniform3f(cameraPosLoc, eye.x, eye.y, eye.z);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		///Camera Position Debug
		//std::cout << "Eye\t" << glm::to_string(eye) << std::endl;
		//std::cout << "Center\t" << glm::to_string(center) << std::endl;
		//std::cout << "Up\t" << glm::to_string(up) << std::endl;


#pragma endregion

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//toggle to render wit GL_FILL or GL_LINE
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#pragma region Draw

		DrawSkybox(skybox, skyboxShaderProgram, view, projection);

		glUseProgram(shaderProgram);
		glUniform3f(ambientColorLoc, 1.0f, 1.0f, 1.0f);

		// Update Loop
		// Loops via the accumulator and a variable step design.
		double newTime = glfwGetTime();
		double frameTime = newTime - currentTime;
		if(frameTime > 0.25) {
			frameTime = 0.25;
		}
		currentTime = newTime;

		accumulator += frameTime;
			
		while(accumulator >= dT) {
			world.startFrame(); // Redundant due to particles already clearing forceAccum after calculation, but this is a sure (segurista) way.
			rbodyWorld.startFrame();
			
			world.runPhysics(dT);
			rbodyWorld.runPhysics(dT);

			t += dT;
			accumulator -= dT;
		}
		/*
		if(contact._particle[0] != NULL || contact._particle[1] != NULL) {
			contact.resolve(dT);
		}
		*/

		// A function here to draw a list of overall particles.
		world.draw(dT);
		rbodyWorld.draw(dT);

		/* Just in case I need to test inheritances and stuff
		rigidBody *test = new rigidBody(&normalTransformLoc, &modelTransformLoc, planet);
		test->setPosition(glm::vec3(25.5, 0.0f, 0.0f));
		test->radius = 1.0f;
		test->inUse = true;
		test->setMass(4.5f);
		std::cout << "Mass: " << test->getMass() << std::endl;
		test->draw();
		*/

		///* Testing rigidbodies
		test->addForceAtPoint(glm::vec3(400.0f, 0.0f, 0.0f), glm::vec3(5000.0f,250.0f,0.0f));
		std::cout << "currPos: " << glm::to_string(test->getPosition()) << std::endl;
		//*/
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

	//DEBUG KEYS
	if(glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		std::cout << "[DEBUG] - Pool size: " << world.getForcePool().getSize() << std::endl;
	}
	if(glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		std::cout << "[DEBUG] - Pool contents: ";
		world.getForcePool().getContents();
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	// onClick
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		std::shared_ptr<particle> fixedPoint(new particle(&normalTransformLoc, &modelTransformLoc, planet));
		fixedPoint->setPosition(glm::vec3(1.0f, 1.0f, 0.0f));

		std::shared_ptr<particle> totesNew(new particle(&normalTransformLoc, &modelTransformLoc, planet));
		totesNew->setParticleParams(particleType);
		totesNew->setPosition(glm::vec3(25.5, 0.0f, 0.0f));
		totesNew->radius = 1.0f;
		totesNew->inUse = true;
		totesNew->setMass(2.5f);

		glm::vec3 acceleration = totesNew->getAcceleration();
		std::shared_ptr<particleGravity> gpart(new particleGravity(acceleration));
		world.getForcePool().add(totesNew, gpart);

		switch (spring)
		{
		case BASIC: {

			std::shared_ptr<particleSpring> springParticle(new particleSpring(fixedPoint, 2.0f, 3.0f));
			world.getForcePool().add(totesNew, springParticle);
		}
				  break;
		case ANCHOR: {
			glm::vec3 fixedPos = fixedPoint->getPosition();
			std::shared_ptr<particleAnchoredSpring> anchoredSpring(new particleAnchoredSpring(fixedPos, 2.0f, 3.0f));
			world.getForcePool().add(totesNew, anchoredSpring);
		}
				   break;
		case ELASTIC: {
			std::shared_ptr<particleElasticBungee> elasticBungee(new particleElasticBungee(fixedPoint, 2.0f, 5.0f));
			world.getForcePool().add(totesNew, elasticBungee);
		}
		case NONE:
			// Do nothing
			break;
		}
		// Testing contacts
		/*
		contact._particle[0] = testParticle;
		contact._particle[1] = totesNew;
		contact.contactNormal = testParticle->getPosition() - totesNew->getPosition();
		contact.contactNormal = glm::normalize(contact.contactNormal);
		contact.restitution = 1;
		*/

		// Function here to add final particle to the particle list.
		world.getParticlePool().push_back(totesNew);
	}
}
