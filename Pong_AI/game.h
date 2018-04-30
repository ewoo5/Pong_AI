#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <random>

#define BALL_X	0
#define BALL_Y	1
#define VEL_X	2
#define VEL_Y	3
#define PAD_Y	4

typedef enum {
	UP,
	DOWN,
	NONE
}action_t;

class game {

private:

	float * state; // <ball_x, ball_y, velocity_x, velocity_y, paddle_y>
	bool ended;
	std::default_random_engine gen;
	std::uniform_real_distribution<float> y_dist{ -0.015f, 0.015f };
	std::uniform_real_distribution<float> x_dist{ -0.03f, 0.03f };

public:

	game();
	~game();

	void get_state(float * buffer);
	int act(action_t action); // returns the reward.
	bool is_ended();
	void restart();

};

#endif

