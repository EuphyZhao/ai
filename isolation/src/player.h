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

class DumbPlayer : public Player
{
 private:
	static const unsigned int kMaxAttempts = 100;
 public:
 DumbPlayer(string name) : Player(name) {}

	virtual Position Move(Board board, Position my, Position her);

};


class MyPlayer : public Player
{
 public:
 MyPlayer(string name) : Player(name) {}
	virtual Position Move(Board board, Position my, Position her);
};


bool TryMove(Board board, Position cur, Direction dir, unsigned int nsteps);


#endif // PLAYER_H_
