#ifndef PLAYER_H_
#define PLAYER_H_

#include <iostream>
#include "types.h"

using namespace std;

class Player
{
 protected:
	string name_;
 public:
	Player(string name) {
		name_ = name;
	}
	virtual ~Player() {}

	string name() { return name_; }
	virtual Position Move(Board board, Position my, Position her) = 0;
};


class HumanPlayer : public Player
{
 public:
 HumanPlayer(string name) : Player(name) {}

	virtual Position Move(Board board, Position my, Position her);

};

class MyPlayer : public Player
{
 public:
 MyPlayer(string name) : Player(name) {}
	virtual Position Move(Board board, Position my, Position her);
};

#endif // PLAYER_H_
