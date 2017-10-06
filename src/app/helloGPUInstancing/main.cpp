#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "SOIL\SOIL.h"

#if defined _WIN32
// OpenGL on Windows needs Windows.h
#include <Windows.h>
#endif
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include "glm\glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common\Shader.h"
#include "common\Camera.h"
#include "common\Model.h"

#include <queue>
#include <thread>
#include <mutex>


// Properties
GLuint screenWidth = 1600, screenHeight = 900;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();
void UpdateThread( );

// Camera
common::Camera camera(glm::vec3(0.0f, 20.0f, 120.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

std::mutex gMutex;

int threadCount = 1;

int liveThread = 0;

const float tick = 1 / 60.0f;
float circleTime = 0.1f;

// number of rock to be instanced.
int amount = 10000;

// for convenience access 
int offset = 20;
int radius = 100;
glm::mat4 * models;
float * angles ;
float * radiusArray ;
float * rot ;
float * scales ;

// The MAIN function, from here we start our application and run our Game loop
int main()
{


	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);

	glfwSwapInterval(0);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glCullFace(GL_BACK);

	// Setup and compile our shaders
	//common::Shader shader("model_loading.vs", "model_loading.frag");
	common::Shader asteroidShader("shader/instancing.vs", "shader/screen.frag" );
	common::Shader planetShader("shader/basic.vs", "shader/basic.fs");

	common::Model rock = common::Model("model/rock/rock.obj");
	common::Model planet = common::Model("model/planet/planet.obj");	



	//int amount = 30000;
	//int offset = 20;
	//int radius = 100;
	srand(glfwGetTime());
	//glm::mat4 * models = new glm::mat4[amount];
	//float * angles = new float[amount];
	//float * radiusArray = new float[amount];
	//float * rot = new float[amount];
	//float * scales = new float[amount];
	models = new glm::mat4[amount];
	angles = new float[amount];
	radiusArray = new float[amount];
	rot = new float[amount];
	scales = new float[amount];


	for (int i = 0; i < amount; ++i) {
		glm::mat4 model;
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (offset));
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (offset));
		float y = displacement * 0.5f;
		displacement = (rand() % (offset));
		float z = cos(angle)* radius + displacement;

		model = glm::translate(model, glm::vec3(x, y, z));

		float scale = (rand() % 20) / 100.0f + 0.05f ;
		model = glm::scale(model, glm::vec3(scale));

		float rotAngle = rand() % 360;

		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		models[i] = model;
		angles[i] = angle;
		rot[i] = rotAngle;
		scales[i] = scale;
		radiusArray[i] = sqrt( x*x + z*z );
	}


	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &models[0], GL_DYNAMIC_DRAW);

	
	for (unsigned int i = 0; i < rock.meshes.size(); ++i) {
		unsigned int VAO = rock.meshes[i].GetVAO();
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}


	float delta = 0.0f;
	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		delta += currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check and call events
		glfwPollEvents();
		Do_Movement();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		planetShader.use();
		planetShader.SetMat4("projection", projection);
		planetShader.SetMat4("view", view);

		
		while (delta > tick) {
			// dispatch worker thread
			if (liveThread < 10) {
				std::thread worker = std::thread(UpdateThread);
				worker.detach();
				liveThread++;
			}

			delta -= tick;
		}

		// draw planet
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -20.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
		planetShader.SetMat4("model", model);
		planet.Draw(planetShader);


		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &models[0], GL_DYNAMIC_DRAW);
		//glBufferSubData(GL_ARRAY_BUFFER, 0, amount * sizeof(glm::mat4), &models[0]);

		// draw meteorites
		asteroidShader.use();
		//asteroidShader.use();
		asteroidShader.SetMat4("projection", projection);
		asteroidShader.SetMat4("view", view);
		asteroidShader.SetInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rock.textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
		for (unsigned int i = 0; i < rock.meshes.size(); i++)
		{
			glBindVertexArray(rock.meshes[i].GetVAO());
			glDrawElementsInstanced(GL_TRIANGLES, rock.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);			
			glBindVertexArray(0);
		}



		glfwSwapBuffers(window);

		//std::cout << glfwGetTime() - lastFrame << " seconds\n";
		std::cout << 1 / (glfwGetTime() - currentFrame) << " fps\n";

	}

	glfwTerminate();
	return 0;
}

void UpdateThread() {
	for (int i = 0; i < amount; ++i) {
		
		angles[i] += tick * circleTime;

		float x = sin(angles[i]) * radiusArray[i];
		float z = cos(angles[i]) * radiusArray[i];

		models[i][3][0] = x;
		models[i][3][2] = z;

	}
	liveThread--;

}


#pragma region "User input"

// Moves/alters the camera positions based on user input
void Do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

#pragma endregion