#include <iostream>
#include <stdlib.h>
#include "Q_agent.h"
#include "game.h"

#define learning_c 100
#define DISCOUNT 0.75

q_agent::q_agent() {
	
	int a, b, c, d, e, f;
	this->state = new float *****[12]; // ball_x
	for (a = 0; a < 12; a++) {
		this->state[a] = new float****[12]; // ball_y
		for (b = 0; b < 12; b++) {
			this->state[a][b] = new float***[2]; // vel_x
			for (c = 0; c < 2; c++) {
				this->state[a][b][c] = new float**[3]; // vel_y
				for (d = 0; d < 3; d++) {
					this->state[a][b][c][d] = new float*[12]; // pad_y
					for (e = 0; e < 12; e++) {
						this->state[a][b][c][d][e] = new float[3]; // action
						for (f = 0; f < 3; f++) {
							this->state[a][b][c][d][e][f] = 0;
							this->currency[a][b][c][d][e][f] = 0;
						}
					}
				}
			}
		}
	}

	this->curr_game = NULL;

}

q_agent::~q_agent() {
	
	int a, b, c, d, e;
	for (a = 0; a < 12; a++) {
		for (b = 0; b < 12; b++) {
			for (c = 0; c < 2; c++) {
				for (d = 0; d < 3; d++) {
					for (e = 0; e < 12; e++) {
						delete[] this->state[a][b][c][d][e];
					}
					delete[] this->state[a][b][c][d];
				}
				delete[] this->state[a][b][c];
			}
			delete[] this->state[a][b];
		}
		delete[] this->state[a];
	}
	delete[] this->state;

	this->curr_game = NULL;

}

void q_agent::train(int epochs) {

	if (this->curr_game == NULL) {
		std::cout << "Please enter set up a game\n";
	}

	int * state_buffer = new int[5];
	float * actual_state = new float[5];
	int * old_int_buffer = new int[5];
	int n, i;

	for (n = 0; n < epochs; n++) {
		this->curr_game->restart();
		bool ended = false;
		while (!ended) {
			float max_q;
			int argmax_a_t;
			float this_q = 0;
			int reward;
			this->curr_game->get_state(actual_state);
			this->translate(actual_state, state_buffer);
			for (i = 0; i < 5; i++) {
				old_int_buffer[i] = state_buffer[i];
			}
			this->explore(state_buffer, &max_q, &argmax_a_t);
			float alpha = this->learning_rate(state_buffer, argmax_a_t);
			this_q += (1 - alpha) * max_q;
			reward = this->curr_game->act((action_t)argmax_a_t);
			//if (reward == 1) { std::cout << "got one bounce!\n"; }
			//std::cout << max_q << "\n";
			if (reward == -1) {
				ended = true;
				//std::cout << "Loss!\n";
			}
			this_q += alpha * reward;
			if (ended) {
				;
			}
			else {
				this->curr_game->get_state(actual_state);
				this->translate(actual_state, state_buffer);
				max_q = -999999;
				for (i = 0; i < 3; i++) {
					if (this->state[state_buffer[0]][state_buffer[1]][state_buffer[2]][state_buffer[3]][state_buffer[4]][i] > max_q) {
						max_q = this->state[state_buffer[0]][state_buffer[1]][state_buffer[2]][state_buffer[3]][state_buffer[4]][i];
					}
				}
				this_q += DISCOUNT * alpha * max_q;
			}
			this->state[old_int_buffer[0]][old_int_buffer[1]][old_int_buffer[2]][old_int_buffer[3]][old_int_buffer[4]][argmax_a_t] = this_q;
			this->currency[old_int_buffer[0]][old_int_buffer[1]][old_int_buffer[2]][old_int_buffer[3]][old_int_buffer[4]][argmax_a_t] += 1;
			//std::cout << old_int_buffer[0] << "," << old_int_buffer[1] << "," << old_int_buffer[2] << "," << old_int_buffer[3] << "," << old_int_buffer[4] << "," << argmax_a_t << "\n";
			//std::cout << alpha <<","<< this->currency[old_int_buffer[0]][old_int_buffer[1]][old_int_buffer[2]][old_int_buffer[3]][old_int_buffer[4]][argmax_a_t] <<"\n";
		}

	}

	delete[] state_buffer;
	delete[] actual_state;

}

void q_agent::set_game(game * in_game) {
	this->curr_game = in_game;
}

void q_agent::explore(int * buffer, float * q, int * a) {

	float max_q = -999999;
	int argmax_a = 0, i;
	int count = 0;
	
	for (i = 0; i < 3; i++) {
		float this_q = 0;
		this_q += this->state[buffer[0]][buffer[1]][buffer[2]][buffer[3]][buffer[4]][i];
		if (this_q > max_q) {
			max_q = this_q;
			argmax_a = i;
		}
	}
	/*
	count = this->currency[buffer[0]][buffer[1]][buffer[2]][buffer[3]][buffer[4]][argmax_a];
	if (count % 10 == 0) {
		argmax_a = (argmax_a + 1 + rand()%2) % 3;
		max_q = this->state[buffer[0]][buffer[1]][buffer[2]][buffer[3]][buffer[4]][argmax_a];
	}
	//std::cout << count << "\n";
	*/
	
	for (i = 0; i < 3; i++) {
		if (this->currency[buffer[0]][buffer[1]][buffer[2]][buffer[3]][buffer[4]][i] < 100) {
			argmax_a = i;
			max_q = this->state[buffer[0]][buffer[1]][buffer[2]][buffer[3]][buffer[4]][i];
		}
	}
	

	*q = max_q;
	*a = argmax_a;

}

void q_agent::translate(float * buffer, int * translated_buffer) {

	// translate ball_x
	int i = 0;
	if ((buffer[i] >= 0) && (buffer[i] < 1.0 / 12)) {
		translated_buffer[i] = 0;
	}
	else if ((buffer[i] >= 1.0/12) && (buffer[i] < 2.0 / 12)) {
		translated_buffer[i] = 1;
	}
	else if ((buffer[i] >= 2.0/12) && (buffer[i] < 3.0 / 12)) {
		translated_buffer[i] = 2;
	}
	else if ((buffer[i] >= 3.0/12) && (buffer[i] < 4.0 / 12)) {
		translated_buffer[i] = 3;
	}
	else if ((buffer[i] >= 4.0/12) && (buffer[i] < 5.0 / 12)) {
		translated_buffer[i] = 4;
	}
	else if ((buffer[i] >= 5.0/12) && (buffer[i] < 6.0 / 12)) {
		translated_buffer[i] = 5;
	}
	else if ((buffer[i] >= 6.0/12) && (buffer[i] < 7.0 / 12)) {
		translated_buffer[i] = 6;
	}
	else if ((buffer[i] >= 7.0/12) && (buffer[i] < 8.0 / 12)) {
		translated_buffer[i] = 7;
	}
	else if ((buffer[i] >= 8.0/12) && (buffer[i] < 9.0 / 12)) {
		translated_buffer[i] = 8;
	}
	else if ((buffer[i] >= 9.0/12) && (buffer[i] < 10.0 / 12)) {
		translated_buffer[i] = 9;
	}
	else if ((buffer[i] >= 10.0/12) && (buffer[i] < 11.0 / 12)) {
		translated_buffer[i] = 10;
	}
	else {
		translated_buffer[i] = 11;
	}

	//translate ball_y
	i = 1;
	if ((buffer[i] >= 0) && (buffer[i] < 1.0 / 12)) {
		translated_buffer[i] = 0;
	}
	else if ((buffer[i] >= 1.0 / 12) && (buffer[i] < 2.0 / 12)) {
		translated_buffer[i] = 1;
	}
	else if ((buffer[i] >= 2.0 / 12) && (buffer[i] < 3.0 / 12)) {
		translated_buffer[i] = 2;
	}
	else if ((buffer[i] >= 3.0 / 12) && (buffer[i] < 4.0 / 12)) {
		translated_buffer[i] = 3;
	}
	else if ((buffer[i] >= 4.0 / 12) && (buffer[i] < 5.0 / 12)) {
		translated_buffer[i] = 4;
	}
	else if ((buffer[i] >= 5.0 / 12) && (buffer[i] < 6.0 / 12)) {
		translated_buffer[i] = 5;
	}
	else if ((buffer[i] >= 6.0 / 12) && (buffer[i] < 7.0 / 12)) {
		translated_buffer[i] = 6;
	}
	else if ((buffer[i] >= 7.0 / 12) && (buffer[i] < 8.0 / 12)) {
		translated_buffer[i] = 7;
	}
	else if ((buffer[i] >= 8.0 / 12) && (buffer[i] < 9.0 / 12)) {
		translated_buffer[i] = 8;
	}
	else if ((buffer[i] >= 9.0 / 12) && (buffer[i] < 10.0 / 12)) {
		translated_buffer[i] = 9;
	}
	else if ((buffer[i] >= 10.0 / 12) && (buffer[i] < 11.0 / 12)) {
		translated_buffer[i] = 10;
	}
	else {
		translated_buffer[i] = 11;
	}

	// translate vel_x
	if (buffer[2] > 0) {
		translated_buffer[2] = 0;
	}
	else {
		translated_buffer[2] = 1;
	}

	// translate vel_y
	if ( (buffer[3] < 0.015) && (buffer[3] > -0.015)) {
		translated_buffer[3] = 0;
	}
	else if (buffer[3] > 0) {
		translated_buffer[3] = 1;
	}
	else {
		translated_buffer[3] = 2;
	}

	//translate pad_y
	i = 4;
	if ((buffer[i] >= 0) && (buffer[i] < 1.0 / 12)) {
		translated_buffer[i] = 0;
	}
	else if ((buffer[i] >= 1.0 / 12) && (buffer[i] < 2.0 / 12)) {
		translated_buffer[i] = 1;
	}
	else if ((buffer[i] >= 2.0 / 12) && (buffer[i] < 3.0 / 12)) {
		translated_buffer[i] = 2;
	}
	else if ((buffer[i] >= 3.0 / 12) && (buffer[i] < 4.0 / 12)) {
		translated_buffer[i] = 3;
	}
	else if ((buffer[i] >= 4.0 / 12) && (buffer[i] < 5.0 / 12)) {
		translated_buffer[i] = 4;
	}
	else if ((buffer[i] >= 5.0 / 12) && (buffer[i] < 6.0 / 12)) {
		translated_buffer[i] = 5;
	}
	else if ((buffer[i] >= 6.0 / 12) && (buffer[i] < 7.0 / 12)) {
		translated_buffer[i] = 6;
	}
	else if ((buffer[i] >= 7.0 / 12) && (buffer[i] < 8.0 / 12)) {
		translated_buffer[i] = 7;
	}
	else if ((buffer[i] >= 8.0 / 12) && (buffer[i] < 9.0 / 12)) {
		translated_buffer[i] = 8;
	}
	else if ((buffer[i] >= 9.0 / 12) && (buffer[i] < 10.0 / 12)) {
		translated_buffer[i] = 9;
	}
	else if ((buffer[i] >= 10.0 / 12) && (buffer[i] < 11.0 / 12)) {
		translated_buffer[i] = 10;
	}
	else {
		translated_buffer[i] = 11;
	}

}

float q_agent::learning_rate(int * buffer, int action) {

	float ret;
	ret = ((float)learning_c / (learning_c + (float)this->currency[buffer[0]][buffer[1]][buffer[2]][buffer[3]][buffer[4]][action]));

	return ret;

}

void q_agent::test() {

	int loop, i;
	int total_score = 0;
	float average_score = 0;
	int above_nine = 0;
	float * actual_state = new float[5];
	int * state_buffer = new int[5];

	for (loop = 0; loop < 200; loop++) {

		this->curr_game->restart();
		int curr_score = 0;
		while (!(this->curr_game->is_ended())) {

			int argmax_a;
			float max_q = -999999;
			this->curr_game->get_state(actual_state);
			this->translate(actual_state, state_buffer);
			for (i = 0; i < 3; i++) {
				if (this->state[state_buffer[0]][state_buffer[1]][state_buffer[2]][state_buffer[3]][state_buffer[4]][i] > max_q) {
					max_q = this->state[state_buffer[0]][state_buffer[1]][state_buffer[2]][state_buffer[3]][state_buffer[4]][i];
					argmax_a = i;
				}
			}
			curr_score += this->curr_game->act((action_t)argmax_a);
			//std::cout << argmax_a << "\n";

		}
		if (curr_score >= 8) {
			above_nine += 1;
		}
		total_score += 1 + curr_score;
		std::cout << curr_score << "\n";
	}

	average_score = (float)total_score / 200;
	std::cout << "average score = " << average_score << "\n";
	std::cout << "# above 9 bounce = " << above_nine << "\n";

	delete[] actual_state;
	delete[] state_buffer;

}

int q_agent::get_move() {

	int argmax_a, i;
	float max_q = -999999;
	float * actual_state = new float[5];
	int * state_buffer = new int[5];
	this->curr_game->get_state(actual_state);
	this->translate(actual_state, state_buffer);
	for (i = 0; i < 3; i++) {
		if (this->state[state_buffer[0]][state_buffer[1]][state_buffer[2]][state_buffer[3]][state_buffer[4]][i] > max_q) {
			max_q = this->state[state_buffer[0]][state_buffer[1]][state_buffer[2]][state_buffer[3]][state_buffer[4]][i];
			argmax_a = i;
		}
	}

	delete[] actual_state;
	delete[] state_buffer;

	return argmax_a;

}