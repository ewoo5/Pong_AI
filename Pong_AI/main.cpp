#include <iostream>
#include "pong.h"
#include "Q_agent.h"
#include "game.h"

using namespace std;

int main() {

	game * the_game = new game;

	q_agent * q_learner = new q_agent;
	q_learner->set_game(the_game);
	q_learner->train(100000);
	//std::cout << "Reached here\n";
	q_learner->test();

	run_pong(the_game, q_learner);

	return 0;

}