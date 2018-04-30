#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "gl_helper.h"

#define WINDOW_W 688
#define WINDOW_H 528

void framebuffer_size_callback(GLFWwindow * window, int width, int height) {
	glViewport(0, 0, width, height);
}

GLFWwindow * init_window() {

	/*
	* Initialize GLFW, and set the options.
	* Set the verions for minor and major to 3, because 3.3 is the lowest we'll want to support.
	* Use the core profile. (Gives access to smaller subset of Opengl functions,
	* i.e. some backwards-compatible functions which we won't need).
	* If on Mac OS X, uncomment the last option.
	*/
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	/*
	* Creates a windows object.
	*/
	GLFWwindow* window = glfwCreateWindow(WINDOW_W, WINDOW_H, "Pong AI", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		//return -1;
	}
	glfwMakeContextCurrent(window);

	/*
	* Set up GLAD to use as the function pointers standardized by OpenGL.
	*/
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		//return -1;
	}

	// Set up rendering window.
	glViewport(0, 0, WINDOW_W, WINDOW_H);

	// Set up window callback function for when the window is resized by the user.
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	return window;

}
unsigned int setup_vertexShader() {

	const char * vertexShaderSource = 
	"#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main() {\n"
	"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\n";
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION FAILED\n" << infoLog << std::endl;
	}

	return vertexShader;

}

unsigned int setup_fragmentShader_pad() {

	int success;
	char infoLog[512];
	const char * fragmentShaderSource = 
	"#version 330 core\n"
	"out vec4 FragColor;\n"
	"\nvoid main(){\n"
	"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}";
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION FAILED\n" << infoLog << std::endl;
	}

	return fragmentShader;

}

unsigned int setup_fragmentShader_wall() {

	int success;
	char infoLog[512];
	const char * fragmentShaderSource =
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"\nvoid main(){\n"
		"	FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);\n"
		"}";
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION FAILED\n" << infoLog << std::endl;
	}

	return fragmentShader;

}

unsigned int setup_fragmentShader_ball() {

	int success;
	char infoLog[512];
	const char * fragmentShaderSource =
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"\nvoid main(){\n"
		"	FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
		"}";
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION FAILED\n" << infoLog << std::endl;
	}

	return fragmentShader;

}

unsigned int setup_shaderProgram(unsigned int vertexShader, unsigned int fragmentShader) {

	int success;
	char infoLog[512];
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetShaderiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING FAILED\n" << infoLog << std::endl;
	}

	return shaderProgram;

}

