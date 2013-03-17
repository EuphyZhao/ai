#ifndef DUMB_H_
#define DUMB_H_

#include "player.h"

class DumbPlayer : public Player
{
 public:
 DumbPlayer(string name) : Player(name) {}

	virtual Position Move(Board board, Position my, Position her)
	{
		return RandomMove(board, my);
	}

};


#endif // DUMB_H_
