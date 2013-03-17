#ifndef DUMB_H_
#define DUMB_H_

#include "player.h"

class DumbPlayer : public Player
{
 private:
	static const unsigned int kMaxAttempts = 100;
 public:
 DumbPlayer(string name) : Player(name) {}

	virtual Position Move(Board board, Position my, Position her)
	{
		vector<Action> actions = GenerateActions(board, my);
		if (actions.empty()) {
			return Position(-1, -1);
		}
		else {
			Action action = actions[rand() % actions.size()];
			return MakeMove(my, action);
		}
	}

};


#endif // DUMB_H_
