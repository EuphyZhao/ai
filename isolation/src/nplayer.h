#ifndef AGGR_PLAYER_H_
#define AGGR_PLAYER_H_

#include <cassert>
#include <iostream>
#include <vector>
#include <queue>
#include <stdlib.h>

#include "game.h"
#include "types.h"
#include "table.h"
#include "player.h"

using namespace std;

class Game;

class AggressivePlayer : public Player
{
 private:
	static const int kMidGameSteps = 5;
	static const int kEndGameSteps = 10;

	static const int kEagerGameSteps = 15;

	static const int kMaxDFSDepth = 20;

	static const int kDefaultMaxDepth = 5;
	static const int kMidGameMaxDepth = 7;
	static const int kEndGameMaxDepth = 11;

	static const unsigned int kMaxIsolationNodes = 100000;

	double Eval2(Board board, Position my, Position her);
	double Eval(Board board, Position my, Position her, int depth);
	bool Cutoff(Board board, Position my, Position her, int depth);

	Position AlphaBeta(Board board, Position my, Position her);

	ScoreAction MaxValue(Board board, Position my, Position her,
						 double alpha, double beta, int depth);

	ScoreAction MinValue(Board board, Position my, Position her,
						 double alpha, double beta, int depth);

    bool IsIsolated(Board board, Position my, Position her);
	int MaxClosureDFS(Board board, Position cur, int depth, int maxdepth);
	int MaxClosure(Board board, Position cur);
	Position LocalMove(Board board, Position my);
	int DoLocalMove(Board board, Position cur);

	Table table_;

	bool isolated_;
	int hersteps_;

	int steps_;

 public:
 AggressivePlayer(string name) : Player(name), isolated_(false), hersteps_(IMAX), steps_(0) {}

	virtual Position Move(Board board, Position my, Position her);

};


Board TryMove(Board board, Position cur, Direction dir, unsigned int nsteps);
Position MakeMove(Position cur, Action action);
Position RandomMove(Board board, Position current);


#endif // PLAYER_H_
