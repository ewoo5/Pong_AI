#include <iostream>
#include <random>
#include "game.h"

/*
* NOTE: In this state space, (0,0) is at the top left corner, unlike OpenGL which treats it as the middle.
*/

#define PADDLE_HEIGHT 0.2

game::game() {

	this->state = new float[5];

	this->state[0] = 0.5;
	this->state[1] = 0.5;
	this->state[2] = 0.03;
	this->state[3] = 0.01;
	this->state[4] = 0.5 - PADDLE_HEIGHT / 2;
	
	this->ended = false;

}

game::~game() {
	delete[] this->state;
}

void game::get_state(float * buffer) {
	if (buffer == NULL) {
		return;
	}

	buffer[0] = this->state[0];
	buffer[1] = this->state[1];
	buffer[2] = this->state[2];
	buffer[3] = this->state[3];
	buffer[4] = this->state[4];
}

int game::act(action_t action) {

	int ret = 0;

	// First, update the pad position, this makes the game easier ;)
	if(action == UP){
		this->state[PAD_Y] = this->state[PAD_Y] - 0.04;
		if (this->state[PAD_Y] < 0) {
			this->state[PAD_Y] = 0;
		}
	}
	else if (action == DOWN) {
		this->state[PAD_Y] += 0.04;
		if (this->state[PAD_Y] > 0.8) {
			this->state[PAD_Y] = 0.8;
		}
	}

	//std::cout << this->state[PAD_Y] << "\n";

	// Now update ball vertical position.
	this->state[BALL_Y] += this->state[VEL_Y];
	if (this->state[BALL_Y] > 1) {
		this->state[BALL_Y] = 2 - this->state[BALL_Y];
		this->state[VEL_Y] = -this->state[VEL_Y];
	}
	else if (this->state[BALL_Y] < 0) {
		this->state[BALL_Y] = -this->state[BALL_Y];
		this->state[VEL_Y] = -this->state[VEL_Y];
	}

	// Now update ball horizontal position.
	this->state[BALL_X] += this->state[VEL_X];
	if (this->state[BALL_X] < 0) {
		this->state[BALL_X] = -this->state[BALL_X];
		this->state[VEL_X] = -this->state[VEL_X];
	}
	else if (this->state[BALL_X] > 1) {
		if ((this->state[BALL_Y] >= this->state[PAD_Y]) && (this->state[BALL_Y] <= this->state[PAD_Y] + PADDLE_HEIGHT)) {
			this->state[BALL_X] = 2 - this->state[BALL_X];
			this->state[VEL_X] = -this->state[VEL_X] + this->x_dist(this->gen);
			this->state[VEL_Y] = this->state[VEL_Y] + this->y_dist(this->gen);
			ret = 1;
		}
		else {
			this->ended = true;
			ret = -1;
		}
	}
	if (this->state[VEL_X] < 0) {
		if (this->state[VEL_X] > -0.03) {
			this->state[VEL_X] = -0.03;
		}
	}
	else if (this->state[VEL_X] >= 0) {
		if (this->state[VEL_X] < 0.03) {
			this->state[VEL_X] = 0.03;
		}
	}

	return ret;
}

bool game::is_ended() {
	return this->ended;
}

void game::restart() {
	this->state[0] = 0.5;
	this->state[1] = 0.5;
	this->state[2] = 0.03;
	this->state[3] = 0.01;
	this->state[4] = 0.5 - PADDLE_HEIGHT / 2;

	this->ended = false;
}