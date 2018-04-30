#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "gl_helper.h"
#include "game.h"
#include "Q_agent.h"

void process_q_in(GLFWwindow * window, game * curr_game, q_agent * agent, float ** vertices);

void process_user_in(GLFWwindow * window, game * curr_game, float ** vertices);

void run_pong(game * curr_game, q_agent * agent);

