#ifndef PLAYER_H_
#define PLAYER_H_

#include <cassert>
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


class MyPlayer : public Player
{
 private:
	static const int kDefaultMaxDepth = 5;
	static const unsigned int kMaxIsolationNodes = 100000;
	
	int kMaxDepth;

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
	int hersteps_;

 public:
 MyPlayer(string name) : Player(name), kMaxDepth(kDefaultMaxDepth), isolated_(false), hersteps_(IMAX) {}

 MyPlayer(string name, int maxDepth) : Player(name), kMaxDepth(maxDepth), isolated_(false), hersteps_(IMAX)
	{
		assert(maxDepth % 2 != 0);
	}

	virtual Position Move(Board board, Position my, Position her);

};


Board TryMove(Board board, Position cur, Direction dir, unsigned int nsteps);
Position MakeMove(Position cur, Action action);
Position RandomMove(Board board, Position current);


#endif // PLAYER_H_
