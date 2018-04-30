#ifndef GLHELPER_H
#define GLHELPER_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow * window, int width, int height);

GLFWwindow * init_window();

/*
	The corresponding Vertex Shader in GLSL:

	#version 330 core
	layout (location = 0) in vec3 aPos;

	void main()
	{
		gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	}
*/
unsigned int setup_vertexShader();

/*
	The corresponding fragment Shader in GLSL:

	#version 330 core
	out vec4 FragColor;

	void main()
	{
		FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
	}

*/
unsigned int setup_fragmentShader_pad();
unsigned int setup_fragmentShader_wall();
unsigned int setup_fragmentShader_ball();

unsigned int setup_shaderProgram(unsigned int vertexShader, unsigned int fragmentShader);

#endif

