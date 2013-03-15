#ifndef PLAYER_H_
#define PLAYER_H_

#include <iostream>
#include <vector>
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
 DumbPlayer(string name) : Player(name) {
		srand(time(NULL));
}

	virtual Position Move(Board board, Position my, Position her);

};


class MyPlayer : public Player
{
 private:
	// I want to evaluate when she is taking the move
	// otherwise I can go one step further
	// so set max depth to an even number
	static const int kMaxDepth = 6;

	bool Gameover(Board board, Position pos);
	double Eval(Board board, Position my, Position her);
	bool Cutoff(Board board, Position my, Position her, int depth);
	Position AlphaBeta(Board board, Position my, Position her);
	ScoreAction MaxValue(Board board, Position my, Position her,
						 double alpha, double beta, int depth);
	ScoreAction MinValue(Board board, Position my, Position her,
						 double alpha, double beta, int depth);


 public:
 MyPlayer(string name) : Player(name) {}
	virtual Position Move(Board board, Position my, Position her);
};


Board TryMove(Board board, Position cur, Direction dir, unsigned int nsteps);
Position MakeMove(Position cur, Direction dir, unsigned int nsteps);

vector<Action> GenerateActions(Board board, Position cur);


#endif // PLAYER_H_
