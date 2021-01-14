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

unsigned int shader_program, VAO;
bool wireframe_mode = false;
bool key_w_held = false;

float vertices[] = {
	0.5f,  0.5f, 0.0f,  
	 0.5f, -0.5f, 0.0f, 
	-0.5f, -0.5f, 0.0f, 
	-0.5f,  0.5f, 0.0f  
};

unsigned int indices[] = {
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};


void updateScreen(GLFWwindow* window) {
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shader_program);
	glBindVertexArray(VAO);
	glDrawElements(
		GL_TRIANGLES,
		6,
		GL_UNSIGNED_INT,
		0
	);

	glfwSwapBuffers(window);
}

const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\n\0";


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

	// Compile programs
	unsigned int vertex_shader = rtCompile(&vertexShaderSource, GL_VERTEX_SHADER);
	// Verify compilations
	int success;
	char infoLog[512];
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
	assertSuccess(success, "Failed to compile vertex shader:\n" << infoLog);

	unsigned int fragment_shader = rtCompile(&fragmentShaderSource, GL_FRAGMENT_SHADER);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
	assertSuccess(success, "Failed to compile fragment shader:\n" << infoLog);
	

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
	assertSuccess(success, "Failed to link shaders into program:\n" << infoLog);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	

	unsigned int VBO, EOB;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EOB);

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

