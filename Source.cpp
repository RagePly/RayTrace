#include <stdlib.h>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "rtTypes.h"



#define assertSuccess(condition, err_message) if (!condition) {	\
	std::cout << err_message << std::endl;						\
	glfwTerminate();											\
	exit(EXIT_FAILURE);											\
}

const RayTrace::ScreenDim origScreenSize = {
	800,	// Width
	600		// Height
};

void updateScreen(GLFWwindow* window) {
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glfwSwapBuffers(window);
}

void framebuffer_size_callback(GLFWwindow * window, int width, int height) {
	glViewport(0, 0, width, height);
	updateScreen(window);
}

void processInput(GLFWwindow* window) {
	// Does not register anywhere, only if the window is in focus
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


int main() {
	
	// Initialize GLFW
	assertSuccess(glfwInit(), "Failed to initialize glfw!");

	// Specifies to GLFW that we are using OpenGL version 3.3(=Major.Minor)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// We are choosing to use the core profile, which is a smaller subset of OpenGL features. 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	
	// Create a window
	GLFWwindow* window = glfwCreateWindow(origScreenSize.width, origScreenSize.height, "Hello Window", NULL, NULL);
	assertSuccess(window, "Failed to create window!");
	// The main context of the current thread, all gl functions called on this thread will apply to this window
	glfwMakeContextCurrent(window);

	// Register our custom, callback function
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Initialize GLAD
	assertSuccess(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Failed to initialize GLAD");

	glViewport(0,0,origScreenSize.width, origScreenSize.height);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		updateScreen(window);

		glfwPollEvents();
	}

	std::cout << "A bye bye!" << std::endl;

	glfwTerminate();
	exit(EXIT_SUCCESS);
}