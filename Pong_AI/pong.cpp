
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "gl_helper.h"
#include "game.h"
#include "Q_agent.h"
#include "pong.h"
#include <chrono>
#include <thread>

#define THICKNESS	16	// the lengths for the pad, and the square ball.
#define WIDTH		688 // width before the walls and pad are deducted. (640 + 32 + 32 + 32) 
#define	HEIGHT		528 // height before the walls are deducted. (480 + 32 + 32 + 32)

static bool UP_KEY = false;
static bool DOWN_KEY = false;

void process_q_in(GLFWwindow * window, game * curr_game, q_agent * agent, float ** vertices) {

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	action_t act = (action_t)agent->get_move();

	curr_game->act(act);
	if (curr_game->is_ended()) {
		curr_game->restart();
	}

	float * buffer = new float[5];
	curr_game->get_state(buffer);

	// Modify vertices of the ball
	float radius_w = 16.0 / WIDTH;
	float radius_h = 16.0 / HEIGHT;
	float center_x = (buffer[0] * 2 - 1.0) * 640.0 / WIDTH;
	float center_y = (-buffer[1] * 2 + 1.0) * 480.0 / HEIGHT;
	vertices[2][0] = center_x - radius_w; //top left
	vertices[2][1] = center_y + radius_h; //top left
	vertices[2][3 + 0] = center_x + radius_w; //top right
	vertices[2][3 + 1] = center_y + radius_h; //top right
	vertices[2][6 + 0] = center_x + radius_w; //bottom right
	vertices[2][6 + 1] = center_y - radius_h; //bottom right
	vertices[2][9 + 0] = center_x - radius_w; //bottom left
	vertices[2][9 + 1] = center_y - radius_h; //bottom left

	if (act < NONE) {

		// Modify the vertices fo the pad
		float pad_w = 16.0 / WIDTH;
		float pad_h = 0.2 * 480.0 / HEIGHT;
		float pad_x = 1 - 16.0 / WIDTH;
		float pad_y = (-(buffer[4] + 0.1) * 2 + 1.0)  * (480.0 + 32.0) / HEIGHT;// +(32.0 + 16.0) / HEIGHT;
		vertices[0][0] = pad_x - pad_w;
		vertices[0][1] = pad_y + pad_h;
		vertices[0][3 + 0] = pad_x + pad_w;
		vertices[0][3 + 1] = pad_y + pad_h;
		vertices[0][6 + 0] = pad_x + pad_w;
		vertices[0][6 + 1] = pad_y - pad_h;
		vertices[0][9 + 0] = pad_x - pad_w;
		vertices[0][9 + 1] = pad_y - pad_h;
	}

	delete[] buffer;

}

void process_user_in(GLFWwindow * window, game * curr_game, float ** vertices) {
	
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		glfwSetWindowShouldClose(window, true);
	}
	
	action_t act = NONE;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		act = UP;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		act = DOWN;
	}

	curr_game->act(act);
	if (curr_game->is_ended()) {
		curr_game->restart();
	}

	float * buffer = new float[5];
	curr_game->get_state(buffer);

	// Modify vertices of the ball
	float radius_w = 16.0 / WIDTH;
	float radius_h = 16.0 / HEIGHT;
	float center_x = (buffer[0] * 2 - 1.0) * 640.0 / WIDTH;
	float center_y = (-buffer[1] * 2 + 1.0) * 480.0 / HEIGHT;
	vertices[2][0] = center_x - radius_w; //top left
	vertices[2][1] = center_y + radius_h; //top left
	vertices[2][3 + 0] = center_x + radius_w; //top right
	vertices[2][3 + 1] = center_y + radius_h; //top right
	vertices[2][6 + 0] = center_x + radius_w; //bottom right
	vertices[2][6 + 1] = center_y - radius_h; //bottom right
	vertices[2][9 + 0] = center_x - radius_w; //bottom left
	vertices[2][9 + 1] = center_y - radius_h; //bottom left

	if (act < NONE) {

		// Modify the vertices fo the pad
		float pad_w = 16.0 / WIDTH;
		float pad_h = 0.2 * 480.0 / HEIGHT;
		float pad_x = 1 - 16.0 / WIDTH;
		float pad_y = (-(buffer[4] + 0.1) * 2 + 1.0)  * (480.0 + 32.0) / HEIGHT;// +(32.0 + 16.0) / HEIGHT;
		vertices[0][0] = pad_x - pad_w;
		vertices[0][1] = pad_y + pad_h;
		vertices[0][3 + 0] = pad_x + pad_w;
		vertices[0][3 + 1] = pad_y + pad_h;
		vertices[0][6 + 0] = pad_x + pad_w;
		vertices[0][6 + 1] = pad_y - pad_h;
		vertices[0][9 + 0] = pad_x - pad_w;
		vertices[0][9 + 1] = pad_y - pad_h;
	}
	
}

void run_pong(game * curr_game, q_agent * agent) {

	int i;
	float width = (float)THICKNESS / WIDTH, height = (float)THICKNESS / HEIGHT;
	std::cout << width << "," << height << "\n";

	GLFWwindow * window = init_window();

	unsigned int vertexShader = setup_vertexShader();
	unsigned int fragmentShader[3];
	fragmentShader[0] = setup_fragmentShader_pad();
	fragmentShader[1] = setup_fragmentShader_wall();
	fragmentShader[2] = setup_fragmentShader_ball();
	unsigned int shaderProgram[3];
	for (i = 0; i < 3; i++) {
		shaderProgram[i] = setup_shaderProgram(vertexShader, fragmentShader[i]);
	}

	// Don't need them after being linked.
	glDeleteShader(vertexShader);
	for (i = 0; i < 3; i++) {
		glDeleteShader(fragmentShader[i]);
	}
	float ** vertices;
	vertices = new float*[3];

	unsigned int ** indices;
	indices = new unsigned int*[3];

	// default pad position (4 vertices)
	float rescale = 0.2 * (480.0 + 32.0) / HEIGHT;
	float vertices0[4 * 3] = {
		1-2*width, rescale*(1-2*height), 0.0,		// top left
		1.0, rescale*(1 - 2 * height), 0.0,		// top right
		1.0, -rescale *(1 - 2 * height), 0.0,		// bottom right
		1-2*width, -rescale *(1 - 2 * height), 0.0		// bottom left
	};
	vertices[0] = vertices0;
	unsigned int indices0[2 * 3] = {
		0, 1, 2,
		0, 3, 2
	};
	indices[0] = indices0;

	// wall (8 vertices)
	float vertices1[8 * 3] = {
		// going from top > bottom, left > right.
		-1.0, 1.0, 0.0,		//0
		1.0, 1.0, 0.0,		//1
		-1+2*width, 1-2*height, 0.0,	//2
		1.0, 1-2*height, 0.0,		//3
		-1+2*width, -1+2*height, 0.0,	//4
		1.0, -1+2*height, 0.0,	//5
		-1.0, -1.0, 0.0,	//6
		1.0, -1.0, 0.0		//7
	};
	vertices[1] = vertices1;
	unsigned int indices1[6 * 3] = {
		0, 1, 3,
		0, 2, 3,
		0, 2, 4,
		0, 6, 4,
		4, 5, 7,
		4, 6, 7
	};
	indices[1] = indices1;

	// ball
	float vertices2[4 * 3] = {
		-width, height, 0.0,	// top left
		width, height, 0.0,	// top right
		width, -height, 0.0,	// bottom right
		-width, -height, 0.0	// bottom left
	};
	vertices[2] = vertices2;
	unsigned int indices2[2 * 3] = {
		0, 1, 2,
		0, 2, 3
	};
	indices[2] = indices2;

	// Generate the required buffers.
	unsigned int * VBO, *VAO, *EBO;
	VBO = new unsigned int[3]; // 0-pad; 1-walls; 2-ball
	VAO = new unsigned int[3];
	EBO = new unsigned int[3];
	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(3, EBO);

	int VertexNum, IndexNum;
	for (i = 0; i < 3; i++) {
		VertexNum = (i%2 == 0)? 12: 24;
		IndexNum = (i % 2 == 0) ? 6 : 18;
		glBindVertexArray(VAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
		//std::cout << "sizeof(vertices[" << i << "] = " << sizeof(vertices1) << "\n";
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[i])*VertexNum, vertices[i], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[i])*IndexNum, indices[i], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Render loop.
	while (!glfwWindowShouldClose(window)) {

		process_q_in(window, curr_game, agent, vertices);

		// Clear buffer with the specified color.
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		for (i = 0; i < 3; i++) {
			glUseProgram(shaderProgram[i]);
			VertexNum = (i % 2 == 0) ? 12 : 24;
			IndexNum = (i % 2 == 0) ? 6 : 18;
			glBindVertexArray(VAO[i]);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
			//std::cout << "sizeof(vertices[" << i << "] = " << sizeof(vertices1) << "\n";
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[i])*VertexNum, vertices[i], GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[i])*IndexNum, indices[i], GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDrawElements(GL_TRIANGLES, IndexNum, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		/*
		for (i = 0; i < 3; i++) {
			int actual_vertices = (i % 2 == 0) ? 6 : 18;
			glUseProgram(shaderProgram[i]);
			glBindVertexArray(VAO[i]);
			glDrawElements(GL_TRIANGLES, actual_vertices, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		*/

		// Get ready for the next frame.
		glfwSwapBuffers(window);
		glfwPollEvents(); // Updates each interrupt, like whether a keyboard key was pressed.
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	glDeleteVertexArrays(3, VAO);
	glDeleteBuffers(3, VBO);
	glDeleteBuffers(3, EBO);

	glfwTerminate();

}
