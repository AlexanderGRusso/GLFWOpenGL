// GLFWOpenGL.cpp : Defines the entry point for the console application.
//

#include <iostream>

#define GLEW_STATIC
#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SOIL2/SOIL2.h"

#include "Shader.h"
#include "Camera.h"

const GLint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function Prototype definitions utilized to process controls.
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow *window, double xPos, double yPos);
void doMovement();

// "Player" Camera object to manage the location of the Camera within the given scene.
Camera camera(glm::vec3(0.0F, 0.0F, 3.0F));

// GLfloat variables holding information about the mouse's previous position to determine camera rotation.
// Initially set to center of the screen.
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;

// Boolean array holding information for key presses.
bool keys[1024];

// In the event that the mouse has not been 
bool firstMouse = true;

// GLfloat variables holding information for timing.
GLfloat deltaTime = 0.0F;
GLfloat lastFrame = 0.0F;

// A pre-determined vector defining the location of the light source within the scene.
glm::vec3 lightPos(0.0F, 1.0F, 0.0F);

int main()
{
	camera.setMovementSpeed(2.0f);

	// Initialization of the GLFW Window, specifying the OpenGL version and profile being utilized.
	// In this case, OpenGL Version 3.3, utilizing the Core Profile instead of the compatibility profile.
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Constructing the GLFWwindow object utilizing the pre-determined width and height;
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Learn OpenGL", nullptr, nullptr);

	// Check to ensure that the window was successfully constructed by GLFW. 
	// Further function calls may result in unhandled exceptions if this is not done.
	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// Once GLFW constructs the window,  the actual screen size will change from the initial inputs.
	// This retrieves the actual screen width and height and stores them for later use.
	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Sets this GLFW window as the one currently being acted upon.
	glfwMakeContextCurrent(window);

	// Establishes a key-callback and mouse-callback to retrieve information being polled by GLFW.
	glfwSetKeyCallback(window, keyCallback); 
	glfwSetCursorPosCallback(window, mouseCallback);

	// Sets the cursor to be invisible within our application.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;

	// Attempts to initialize GLEW, and handles failure appropriately.
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;

		return EXIT_FAILURE;
	}
	// Specifies the viewport within which we are rendering utilizing OpenGL.
	// In this case, the entire screen is holding the render.
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Enables rendering in 3D space. (Allows for depth in the "World")
	glEnable(GL_DEPTH_TEST);

	// Enables the utilization of textures with alpha.
	// This allows for the rendering of transparent sections of textures.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Construction of shader objects. 
	// These objects load the vertex and fragment shaders from their files.
	// They then compile and attach these shaders to a new shader program and link them.
	// This makes the shader program readily accessible for use later.
	Shader lightingShader("lighting_vertex.glsl", "lighting_fragment.glsl");
	Shader lampShader("lamp_vertex.glsl", "lamp_fragment.glsl");

	// Declaration of information to render our cubes in 3D space.
	// Format: 
	//		First 3F: Coordinates in 3D space for orienting the object for rendering.
	//		Second 3F: Normal vector for the given cube-face to perform lighting calculations.
	//		Third 2F: Texture Coordinates
	GLfloat vertices[] =
	{
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};
	
	// Declaration of absolute cube-positions to render multiple varieties of the cube above.
	glm::vec3 cubePositions[] =
	{
		glm::vec3(0.0F, 0.0F, 0.0F),
		glm::vec3(2.0F, 5.0F, -15.0F),
		glm::vec3(-1.5F, -2.2F, -2.5F),
		glm::vec3(-3.8F, -2.0F, -12.3F),
		glm::vec3(2.4F, -0.4F, -3.5F),
		glm::vec3(-1.7F, 3.0F, -7.5F),
		glm::vec3(1.3F, -2.0F, -2.5F),
		glm::vec3(1.5F, 2.0F, -2.5F),
		glm::vec3(-1.5F, 0.0F, 0.0F),
	};

	GLuint VBO, boxVAO;

	glGenVertexArrays(1, &boxVAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(boxVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position Coordinates
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Normal Attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Texture Coords Attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position Coordinates
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	GLuint diffuseMap, specularMap;

	glGenTextures(1, &diffuseMap);
	glGenTextures(1, &specularMap);

	int textureWidth, textureHeight;

	unsigned char *image;

	// Diffuse Map
	image = SOIL_load_image("container2.png", &textureWidth, &textureHeight, 0, SOIL_LOAD_RGB);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	image = SOIL_load_image("container2_specular.png", &textureWidth, &textureHeight, 0, SOIL_LOAD_RGB);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	lightingShader.useProgram();
	glUniform1i(glGetUniformLocation(lightingShader.program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.program, "material.specular"), 1);

	glm::mat4 projection = glm::perspective(camera.getZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1F, 1000.0F);

	while (!glfwWindowShouldClose(window))
	{
		lightPos.x -= cos(glfwGetTime()) * 0.001F;
		lightPos.z -= cos(glfwGetTime()) * 0.001F;

		GLfloat currentFrame = (GLfloat) glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();

		doMovement();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightingShader.useProgram();
		GLint lightPosLoc = glGetUniformLocation(lightingShader.program, "light.position");
		GLint viewPosLoc = glGetUniformLocation(lightingShader.program, "viewPos");
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(viewPosLoc, camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

		glUniform3f(glGetUniformLocation(lightingShader.program, "light.ambient"), 0.2F, 0.2F, 0.2F);
		glUniform3f(glGetUniformLocation(lightingShader.program, "light.diffuse"), 0.5F, 0.5F, 0.5F);
		glUniform3f(glGetUniformLocation(lightingShader.program, "light.specular"), 1.0F, 1.0F, 1.0F);

		glUniform1f(glGetUniformLocation(lightingShader.program, "material.shininess"), 32.0F);

		glm::mat4 view;
		view = camera.getViewMatrix();
		GLint modelLoc = glGetUniformLocation(lightingShader.program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.program, "view");
		GLint projectionLoc = glGetUniformLocation(lightingShader.program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		glBindVertexArray(boxVAO);
		glm::mat4 model;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);

		lampShader.useProgram();

		modelLoc = glGetUniformLocation(lampShader.program, "model");
		viewLoc = glGetUniformLocation(lampShader.program, "view");
		projectionLoc = glGetUniformLocation(lampShader.program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2F));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);


		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &boxVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();

    return EXIT_SUCCESS;
}

void doMovement() 
{
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.processKeyboard(FORWARD, deltaTime);
	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.processKeyboard(BACKWARD, deltaTime);
	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.processKeyboard(LEFT, deltaTime);
	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.processKeyboard(RIGHT, deltaTime);
	}

	if (keys[GLFW_KEY_SPACE])
	{
		camera.processKeyboard(UP, deltaTime);
	}

	if (keys[GLFW_KEY_LEFT_CONTROL])
	{
		camera.processKeyboard(DOWN, deltaTime);
	}
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode) 
{
	if (GLFW_KEY_ESCAPE == key && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	
	if (key >= 0 && key < 1024)
	{
		if (GLFW_PRESS == action)
		{
			keys[key] = true;
		} 
		else if(GLFW_RELEASE == action)
		{
			keys[key] = false;
		}
	}
}


void mouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	camera.processMouseMovement(xOffset, yOffset);
}