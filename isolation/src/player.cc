#include <iostream>
#include <limits>
#include <stdlib.h>

#include "types.h"
#include "player.h"
#include "game.h"

using namespace std;

vector<Action> GenerateActions()
{
	vector<Action> actions;
	for (int d = 0; d < 8; d++)
		for (int steps = 1; steps < kBoardSize; steps++) 
			actions.push_back(Action((Direction)d, steps));
	return actions;
}


Board TryMove(Board board, Position cur, Direction dir, unsigned int nsteps)
{
	int row = cur.row, col = cur.col;
	int ro = kOffsets[dir][0];
	int co = kOffsets[dir][1];
	
	int rend = row + ro*nsteps;
	int cend = col + co*nsteps;

	Board nboard = board;
	if (rend < 0 || cend < 0 ||
		rend >= kBoardSize || rend >= kBoardSize)
		return kInvalidBoard;

	for (unsigned int step = 1; step <= nsteps; step++) {
		int r = row + ro * step, c = col + co * step;
		if (get(board, r, c)) {
			return kInvalidBoard;
		}
		else {
			set(nboard, r, col + c);
		}
	}

	return nboard;
}

Position MakeMove(Position cur, Direction dir, unsigned int nsteps)
{
	int row = cur.row, col = cur.col;
	int ro = kOffsets[dir][0];
	int co = kOffsets[dir][1];

	return Position(row+ro*nsteps, col+co*nsteps);
}

Position HumanPlayer::Move(Board board, Position my, Position her)
{
	int row, col;
	cout << ">>";
	cin >> row;
	cin >> col;
	return Position(row-1, col-1);
}

Position DumbPlayer::Move(Board board, Position my, Position her)
{
	unsigned int attempts = 0;
	while (1) {
		if (attempts > kMaxAttempts)
			return Position(-1,-1);

		// random direction
		Direction d = (Direction)(rand() % 8);

		// random step length
		unsigned int nsteps = rand() % 8 + 1; // at least move on step

		if (TryMove(board, my, d, nsteps)) {
			return MakeMove(my, d, nsteps);
		}
	}
}


double MyPlayer::Eval(Board board, Position my, Position her)
{
	return 0;
}

bool MyPlayer::Cutoff(Board board, Position my, Position her, int depth)
{
	return depth > kMaxDepth;
}

Position MyPlayer::AlphaBeta(Board board, Position my, Position her)
{
	double alpha = numeric_limits<double>::min();
	double beta = numeric_limits<double>::max();

	ScoreAction sa = MaxValue(board, my, her, alpha, beta, 0);
	
	if (TryMove(board, my, sa.action.dir, sa.action.steps)) {
		return MakeMove(my, sa.action.dir, sa.action.steps);
	}
	else // we are dead
		return Position(-1, -1);
}

ScoreAction MyPlayer::MaxValue(Board board, Position my, Position her,
							   double alpha, double beta, int depth)
{
	if (Cutoff(board, my, her, depth))
		return ScoreAction(Eval(board, my, her), kInvalidAction);

	double max = numeric_limits<double>::min();
	Action maxaction = kInvalidAction;

	vector<Action> actions = GenerateActions();
	for (unsigned int i = 0; i < actions.size(); i++) {
		Direction d = actions[i].dir;
		int steps = actions[i].steps;

		Board nboard;
		if (!(nboard = TryMove(board, my, d, steps)))
			continue;
			

		ScoreAction sa = MinValue(nboard, MakeMove(my, d, steps), her, alpha, beta, depth+1);
		double score = sa.score;
		if (score > max) {
			max = score;
			maxaction = Action(d,steps);
		}

		if (max >= beta)
			return ScoreAction(max, maxaction);
			
		alpha = max > alpha ? max : alpha;
	}

	return ScoreAction(max, maxaction);
}


ScoreAction MyPlayer::MinValue(Board board, Position my, Position her,
							   double alpha, double beta, int depth)
{
	if (Cutoff(board, my, her, depth))
		return ScoreAction(Eval(board, her, my), kInvalidAction);

	double min = numeric_limits<double>::max();
	Action minaction = kInvalidAction;

	vector<Action> actions = GenerateActions();
	for (unsigned int i = 0; i < actions.size(); i++) {
		Direction d = actions[i].dir;
		int steps = actions[i].steps;

		Board nboard;
		if (!(nboard = TryMove(board, her, d, steps))) // she tries to move
			continue;
			

		ScoreAction sa = MaxValue(nboard, my, MakeMove(her, d, steps), alpha, beta, depth+1);
		double score = sa.score;
		if (score < min) {
			min = score;
			minaction = Action(d, steps);
		}

		if (min <= beta)
			return ScoreAction(min, minaction);
			
		beta = min < beta ? min : beta;
	}

	return ScoreAction(min, minaction);
}


Position MyPlayer::Move(Board board, Position my, Position her)
{
	Position move = AlphaBeta(board, my, her);
	return move;
}
