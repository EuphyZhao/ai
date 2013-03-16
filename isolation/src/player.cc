#include <iostream>
#include <limits>
#include <queue>
#include <set>
#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "player.h"
#include "game.h"
#include "table.h"

using namespace std;

// I can prune actions for my moves
// But I cannot prune actions for my opponenents
// Avoid trap yourself
vector<Action> GenerateActions(Board board, Position cur)
{
	vector<Action> actions;
	for (int d = 0; d < 8; d++)
		for (int steps = 1; steps < kBoardSize; steps++) 
			if (TryMove(board, cur, (Direction)d, steps))
				actions.push_back(Action((Direction)d, steps));
			else
				break;
	return actions;
}


// Prune all the suicide moves
// A suicide move is defined as
// if I am isolated from my opponent
// the slot I can reach is less than her
// The pruning needs to be precicely accurate
// because win/loss is decided by these small steps

// return 0 if isolated
// return +n if i can win in n steps
// return -n if i will lose in n steps
int MyPlayer::IsIsolated(Board board, Position my, Position her)
{
	// check the table first
	int lookup = table_.isolated(board, my, her);

	if (lookup == 1)
		return true;
	else if (lookup == -1)
		return false;
	
	// BFS
	queue<BfsNode> frontier;
	frontier.push(BfsNode(board, my, 0));

	std::set<BfsNode, BfsNodeCompare> visited;
	visited.insert(BfsNode(board, my));

	int maxsteps = 0; // keep max steps I left
	while (!frontier.empty()) {
		BfsNode node = frontier.front();
		if (node.depth > maxsteps)
			maxsteps = node.depth;

		frontier.pop();

		for (int d = 0; d < 8; d++) {
			Position next = MakeMove(node.cur, (Direction)d, 1);
			// we are reachable
			if (next == her) {
				table_.insert(board,my,her,false);
				return 0;
			}

			Board nboard;
			if ((nboard=TryMove(board, node.cur, (Direction)d, 1))
				&& visited.find(BfsNode(nboard, next))==visited.end()) {
				frontier.push(BfsNode(nboard, next, node.depth+1));
				visited.insert(BfsNode(nboard, next));
			}
		}
	}

	// check how many steps does she have
	int hersteps = MaxClosure(board, her);

	table_.insert(board,my,her,true);
	
	if (maxsteps > hersteps)
		return maxsteps;
	else
		return -maxsteps; // i am going to lose
}


// return the maximum number of steps
// conditioned on that we are isolated
// this is deterministic
int MyPlayer::MaxClosure(Board board, Position cur)
{
	queue<BfsNode> frontier;
	frontier.push(BfsNode(board, cur, 0));

	std::set<BfsNode, BfsNodeCompare> visited;
	visited.insert(BfsNode(board, cur));

	int maxsteps = 0; // keep max steps I left
	while (!frontier.empty()) {
		BfsNode node = frontier.front();
		if (node.depth > maxsteps)
			maxsteps = node.depth;

		frontier.pop();

		for (int d = 0; d < 8; d++) {
			Position next = MakeMove(node.cur, (Direction)d, 1);

			Board nboard;
			if ((nboard=TryMove(board, node.cur, (Direction)d, 1))
				&& visited.find(BfsNode(nboard, next))==visited.end()) {
				frontier.push(BfsNode(nboard, next, node.depth+1));
				visited.insert(BfsNode(nboard, next));
			}
		}
	}
	return maxsteps;
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
		rend >= kBoardSize || cend >= kBoardSize)
		return kInvalidBoard;

	for (unsigned int step = 1; step <= nsteps; step++) {
		int r = row + ro * step, c = col + co * step;
		if (getpos(board, r, c)) {
			return kInvalidBoard;
		}
		else {
			setpos(nboard, r, col + c);
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
	vector<Action> actions = GenerateActions(board, my);
	if (actions.empty()) {
		return Position(-1, -1);
	}
	else {
		Action action = actions[rand() % actions.size()];
		return MakeMove(my, action.dir, action.steps);
	}
}


double MyPlayer::Eval(Board board, Position my, Position her)
{
	// Version 1:
	// number of empty slots I can move / number of slots she can move

	vector<Action> myactions = GenerateActions(board, my);
	vector<Action> heractions = GenerateActions(board, her);

	// she cannot move
	// we win!
	if (heractions.size() == 0)
		return DMAX;

	return 1.0 * myactions.size() / heractions.size();

	// Version 2:
	// Number of slots I can reach in two steps

	// Version 3:
	// Number of slots I can control (I will reach earlier than her)
}

bool MyPlayer::Cutoff(Board board, Position my, Position her, int depth)
{
	return depth > kMaxDepth;
}

bool MyPlayer::Gameover(Board board, Position pos)
{
	return GenerateActions(board, pos).size() == 0;
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
	// I lose
	if (Gameover(board, my)) {
		//		cout << "Goal test reached in MaxValue." << endl;
		return ScoreAction(DMIN, kInvalidAction);
	}
	// She lose. We can pick a random step
	if (Gameover(board, her)) {
		vector<Action> actions = GenerateActions(board, my);
		return ScoreAction(DMAX, actions[0]);
	}
	
	if (Cutoff(board, my, her, depth)) {
		return ScoreAction(Eval(board, my, her), kInvalidAction);
	}

	Action maxaction = kInvalidAction;

	vector<Action> actions = GenerateActions(board, my);
	for (unsigned int i = 0; i < actions.size(); i++) {
		Direction d = actions[i].dir;
		int steps = actions[i].steps;
		Position npos = MakeMove(my, d, steps);

		Board nboard;
		double score;
		if (!(nboard = TryMove(board, my, d, steps)))
			continue;

		int isolation = IsIsolated(nboard, npos, her);
		if (isolation!=0) { // you can reach an isolation state
			if (isolation > 0)
				score = DMAX;
			else
				score = DMIN;
		}
		else // you have to search further
			score = MinValue(nboard, npos, her, alpha, beta, depth+1).score;

		if (score > alpha) {
			alpha = score;
			maxaction = Action(d,steps);
		}

		if (alpha >= beta)
			return ScoreAction(alpha, maxaction);

		// I have lost
		if (alpha == DMIN && depth == 0) {
			cout << "I have forseen my loss!" << endl;
		}

		// monitor alpha
		if (depth == 0) {
			cout << "alpha = " << alpha << endl;
		}
	}

	return ScoreAction(alpha, maxaction);
}


ScoreAction MyPlayer::MinValue(Board board, Position my, Position her,
							   double alpha, double beta, int depth)
{
	// She lose
	if (Gameover(board, her)) {
		//		cout << "Goal test reached in MinValue at depth " << depth << endl;
		return ScoreAction(DMAX, kInvalidAction);
	}
	// When she is moving, she find that I am losing
	// She can pick a random move
	if (Gameover(board, my)) {
		vector<Action> actions = GenerateActions(board, her);
		return ScoreAction(DMIN, actions[0]);
	}

	if (Cutoff(board, my, her, depth)) {
		return ScoreAction(Eval(board, her, my), kInvalidAction);
	}

	Action minaction = kInvalidAction;

	vector<Action> actions = GenerateActions(board, her);
	for (unsigned int i = 0; i < actions.size(); i++) {
		Direction d = actions[i].dir;
		int steps = actions[i].steps;
		Position npos = MakeMove(her, d, steps);

		Board nboard;
		double score;
		if (!(nboard = TryMove(board, her, d, steps))) // she tries to move
			continue;
			
		int isolation = IsIsolated(nboard, npos, her);
		if (isolation!=0) { // you can reach an isolation state
			if (isolation > 0)
				score = DMAX;
			else
				score = DMIN;
		}
		else
			score = MaxValue(nboard, my, npos, alpha, beta, depth+1).score;

		if (score < beta) {
			beta = score;
			minaction = Action(d, steps);
		}

		if (beta <= alpha) // don't need to search further, I will pick a better path
			return ScoreAction(beta, minaction);

		// monitor beta
		if (depth == 1) {
			cout << "beta = " << beta << endl;
		}

	}

	return ScoreAction(beta, minaction);
}


Position MyPlayer::Move(Board board, Position my, Position her)
{
	// if we are in isolation
	if (isolated_) {
		if (stored_path_.size() != 0) {
			Position move = stored_path_.front();
			stored_path_.pop();
			return move;
		}
		else
			return Position(-1, -1);
	}
	

	Position move = AlphaBeta(board, my, her);
	return move;
}
