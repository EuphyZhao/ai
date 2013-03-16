#ifndef PLAYER_H_
#define PLAYER_H_

#include <iostream>
#include <vector>
#include <queue>
#include <stdlib.h>
#include "game.h"
#include "types.h"
#include "table.h"

using namespace std;

class Game;

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
 private:

	// I want to evaluate when she is taking the move
	// otherwise I can go one step further
	// so set max depth to an even number

	// when kMaxDepth=0, it is equivalent to no alpha-beta
	// but only has isolation check
	static const int kMaxDepth = 3;

	bool Gameover(Board board, Position pos);

	double Eval(Board board, Position my, Position her);
	bool Cutoff(Board board, Position my, Position her, int depth);

	Position AlphaBeta(Board board, Position my, Position her);

	ScoreAction MaxValue(Board board, Position my, Position her,
						 double alpha, double beta, int depth);

	ScoreAction MinValue(Board board, Position my, Position her,
						 double alpha, double beta, int depth);

    bool IsIsolated(Board board, Position my, Position her);
	int MaxClosure(Board board, Position cur);
	Position LocalMove(Board board, Position my);
	int DoLocalMove(Board board, Position cur);

	Table table_;

	bool isolated_;
	int mysteps_;
	int hersteps_;

 public:
 MyPlayer(string name) : Player(name), isolated_(false) {}

	virtual Position Move(Board board, Position my, Position her);

};


Board TryMove(Board board, Position cur, Direction dir, unsigned int nsteps);
Position MakeMove(Position cur, Action action);

vector<Action> GenerateActions(Board board, Position cur);


#endif // PLAYER_H_
