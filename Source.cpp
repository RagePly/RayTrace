#include <stdlib.h>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "rtTypes.h"
#include "rtShader.h"



#define assertSuccess(condition, err_message) if (!condition) {	\
	std::cout << err_message << std::endl;						\
	glfwTerminate();											\
	exit(EXIT_FAILURE);											\
}

const RayTrace::ScreenDim origScreenSize = {
	800,	// Width
	600		// Height
};

RayTrace::Shader shader_program, shader_program2;
unsigned int VAO, VAO2;
bool wireframe_mode = false;
bool key_w_held = false;

float vertices[] = {
	1.0f,  0.5f, 0.0f,  
	1.0f, -0.5f, 0.0f, 
	0.0f, -0.5f, 0.0f, 
	0.0f,  0.5f, 0.0f
};

unsigned int indices[] = {
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};

float vertices2[] = {
	 0.0f,  0.5f, 0.0f,
	 0.0f, -0.5f, 0.0f,
	-1.0f, -0.5f, 0.0f,
	-1.0f,  0.5f, 0.0f
};

unsigned int indices2[] = {
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};




void updateScreen(GLFWwindow* window) {
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	shader_program.use();
	glBindVertexArray(VAO);
	glDrawElements(
		GL_TRIANGLES,
		6,
		GL_UNSIGNED_INT,
		0
	);

	float timeNow = (float) glfwGetTime();
	float timeVal = (sin(timeNow*10) / 2.0f) + 0.5f;
	int varLoc = glGetUniformLocation(shader_program2, "our_color");
	
	shader_program2.use();
	glUniform4f(varLoc, timeVal, 1.0f - timeVal, 0.0f, 1.0f);
	glBindVertexArray(VAO2);
	glDrawElements(
		GL_TRIANGLES,
		6,
		GL_UNSIGNED_INT,
		0
	);

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

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (!key_w_held) {
			if (wireframe_mode)
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			wireframe_mode = !wireframe_mode;
			key_w_held = true;
		}
	}
	else {
		key_w_held = false;
	}
}

/*
Might seem confusing with const char**, but the glShaderSource can accept many fragments as a complete source. But in the beginning we only specify one,
and therefore we just give it the address.
*/
unsigned int rtCompile(const char** source_code, unsigned int shader_type) {
	unsigned int shader_handle = glCreateShader(shader_type);
	glShaderSource(shader_handle, 1, source_code, NULL);
	glCompileShader(shader_handle);

	return shader_handle;
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

	shader_program.compileFromPath("vertex.glsl", "fragment.glsl");
	shader_program2.compileFromPath("vertex.glsl", "fragment2.glsl");
	

	unsigned int VBO, VBO2, EOB, EOB2;
	glGenVertexArrays(1, &VAO);
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EOB);
	glGenBuffers(1, &EOB2);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EOB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(	// When this is called, the bound buffer is also associated with VAO.
		0,					// the attribute to setup. Attribute is what the vertex shader specifies as (location = 0)
		3,					// count
		GL_FLOAT,			// type
		GL_FALSE,			// do normalize
		3 * sizeof(float),	// The stride length
		(void*)0			// entry point
	);
	glEnableVertexAttribArray(0); // there are up to 15 you could specify, maybe

	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EOB2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);

	glVertexAttribPointer(	// When this is called, the bound buffer is also associated with VAO.
		0,					// the attribute to setup. Attribute is what the vertex shader specifies as (location = 0)
		3,					// count
		GL_FLOAT,			// type
		GL_FALSE,			// do normalize
		3 * sizeof(float),	// The stride length
		(void*)0			// entry point
	);
	glEnableVertexAttribArray(0); // there are up to 15 you could specify, maybe

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		updateScreen(window);
		glfwPollEvents();
	}

	std::cout << "A bye bye!" << std::endl;

	glfwTerminate();
	exit(EXIT_SUCCESS);
}

