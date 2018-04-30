#pragma once
#include <iostream>
#include "game.h"

class q_agent {

private:
	float ****** state; // <ball_x><ball_y><vel_x><vel_y><pad_y><action>;
	int currency[12][12][2][3][12][3]; // for storing how many times a (state,action) pair has been tried.
	game * curr_game;

public:
	q_agent();
	~q_agent();

	void train(int epochs);
	void set_game(game * in_game);
	void explore(int * buffer, float * q, int * a);
	void translate(float * buffer, int * translated_buffer);

	float learning_rate(int * buffer, int action);

	void test();
	int get_move();

};