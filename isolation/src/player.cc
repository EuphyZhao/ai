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
#include "util.h"

using namespace std;

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


bool MyPlayer::IsIsolated(Board board, Position my, Position her)
{
	cout << "begin isolation." << endl;
	
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
			Position next = MakeMove(node.cur, Action((Direction)d, 1));
			// we are reachable
			if (next == her) {
	cout << "out isolation." << endl;

				table_.insert(board,my,her,false);
				return false;
			}

			Board nboard;
			if ((nboard=TryMove(node.board, node.cur, (Direction)d, 1))
				&& visited.find(BfsNode(nboard, next))==visited.end()) {
				frontier.push(BfsNode(nboard, next, node.depth+1));
				visited.insert(BfsNode(nboard, next));
			}
		}
	}

	cout << "out isolation." << endl;
	table_.insert(board,my,her,true);
	return true;
}


// return the maximum number of steps
// conditioned on that we are isolated
// this is deterministic
int MyPlayer::MaxClosure(Board board, Position cur)
{

	cout << "begin" << endl;

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
			Position next = MakeMove(node.cur, Action((Direction)d, 1));

			Board nboard;
			if ((nboard=TryMove(node.board, node.cur, (Direction)d, 1))) {
				if (visited.find(BfsNode(nboard, next))==visited.end()) {
					frontier.push(BfsNode(nboard, next, node.depth+1));
					visited.insert(BfsNode(nboard, next));
				}
			}
		}
	}

	cout << "out" << endl;

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
			setpos(nboard, r, c);
		}
	}

	return nboard;
}

Position MakeMove(Position cur, Action action)
{
	int row = cur.row, col = cur.col;
	int ro = kOffsets[action.dir][0];
	int co = kOffsets[action.dir][1];

	return Position(row+ro*action.steps, col+co*action.steps);
}

// Estimate the likelihood that we are isolated
// and she controls more squres than me
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
	return (depth > kMaxDepth);
}

bool MyPlayer::Gameover(Board board, Position pos)
{
	return GenerateActions(board, pos).size() == 0;
}
int MyPlayer::DoLocalMove(Board board, Position cur)
{
	queue<BfsNode> frontier;
	frontier.push(BfsNode(board, cur, 0));

	std::set<BfsNode, BfsNodeCompare> visited;
	visited.insert(BfsNode(board, cur));

	int maxsteps = 0; // keep max steps I left
	while (!frontier.empty()) {
		BfsNode node = frontier.front();
		if (node.depth > maxsteps) {
			maxsteps = node.depth;
			
			// early termination
			if (maxsteps > hersteps_) 
				return maxsteps;
		}

		frontier.pop();

		for (int d = 0; d < 8; d++) {
			Position next = MakeMove(node.cur, Action((Direction)d, 1));

			Board nboard;
			if ((nboard=TryMove(node.board, node.cur, (Direction)d, 1))) {
				if (visited.find(BfsNode(nboard, next))==visited.end()) {
					frontier.push(BfsNode(nboard, next, node.depth+1));
					visited.insert(BfsNode(nboard, next));
				}
			}
		}
	}
	return maxsteps;
}


// Find the next best local move
Position MyPlayer::LocalMove(Board board, Position my)
{
	cout << "Local move start" << endl;
	Position best = Position(-1,-1);
	int maxsteps = 0;
	for (int d = 0; d < 8; d++) {
		Position next = MakeMove(my, Action((Direction)d, 1));
		Board nboard=TryMove(board, my, (Direction)d, 1);
		if (nboard) {
			int steps = DoLocalMove(nboard, next);
			if (steps > hersteps_) {
				maxsteps = steps;
				best = next;
				break;
			}
		}
	}
	mysteps_ = maxsteps - 1; // may be lower
	hersteps_--;
	cout << "Local move end" << endl;
	return best;
}



Position MyPlayer::AlphaBeta(Board board, Position my, Position her)
{
	double alpha = DMIN;
	double beta = DMAX;

	ScoreAction sa = MaxValue(board, my, her, alpha, beta, 0);

	// We will win because we have reached a
	// Good Isolation
	// Pick best move
	// if (sa.score == DMAX) {
	// 	isolated_ = true;
	// 	cout << "I will win! Switching to local mode" << endl;
	// 	return LocalMove(board, my);
	// }

	// We will lose because we reached a 
	// Bad Isolation
	// But try out best, hopefully the opponent will make
	// a mistake
	// else if (sa.score == DMIN) {
	// 	isolated_ = true;
	// 	cout << "I will lose! Switching to local mode" << endl;
	// 	return LocalMove(board, my);
	// }
	
	
	if (TryMove(board, my, sa.action.dir, sa.action.steps)) {
		return MakeMove(my, sa.action);
	}
	else {
		return Position(-1, -1);
	}
}

ScoreAction MyPlayer::MaxValue(Board board, Position my, Position her,
							   double alpha, double beta, int depth)
{
	// I lose
	if (Gameover(board, my))
		return ScoreAction(DMIN, kInvalidAction);

	// She lose
	if (Gameover(board, her)) {
		vector<Action> actions = GenerateActions(board, my);
		return ScoreAction(DMAX, actions[0]);
	}

	// Isolation test
	if (IsIsolated(board, my, her)) {
		int mysteps = MaxClosure(board, my);
		int hersteps = MaxClosure(board, her);
		cout << mysteps << ":" << hersteps << endl;

		if (mysteps > hersteps)
			return ScoreAction(DMAX, kInvalidAction);
		else
			return ScoreAction(DMIN, kInvalidAction);
	}

	// Cutoff test
	if (Cutoff(board, my, her, depth))
		return ScoreAction(Eval(board, my, her), kInvalidAction);

	Action maxaction = kInvalidAction;
	vector<Action> actions = GenerateActions(board, my);
	for (unsigned int i = 0; i < actions.size(); i++) {
		Direction d = actions[i].dir;
		int steps = actions[i].steps;
		Position npos = MakeMove(my, actions[i]);

		Board nboard;
		double score;
		if (!(nboard = TryMove(board, my, d, steps)))
			continue;

		score = MinValue(nboard, npos, her, alpha, beta, depth+1).score;

		if (score > alpha) {
			alpha = score;
			maxaction = Action(d,steps);
		}

		if (alpha >= beta)
			return ScoreAction(alpha, maxaction);
	}

	return ScoreAction(alpha, maxaction);
}


ScoreAction MyPlayer::MinValue(Board board, Position my, Position her,
							   double alpha, double beta, int depth)
{
	// She lose
	if (Gameover(board, her))
		return ScoreAction(DMAX, kInvalidAction);

	// I lose
	if (Gameover(board, my)) {
		vector<Action> actions = GenerateActions(board, her);
		return ScoreAction(DMIN, actions[0]);
	}

	// Isolation test
	if (IsIsolated(board, my, her)) {
		int mysteps = MaxClosure(board, my);
		int hersteps = MaxClosure(board, her);
		if (mysteps >= hersteps)
			return ScoreAction(DMAX, kInvalidAction);
		else
			return ScoreAction(DMIN, kInvalidAction);
	}

	// Cutoff test
	if (Cutoff(board, my, her, depth))
		return ScoreAction(Eval(board, her, my), kInvalidAction);

	Action minaction = kInvalidAction;
	vector<Action> actions = GenerateActions(board, her);
	for (unsigned int i = 0; i < actions.size(); i++) {
		Direction d = actions[i].dir;
		int steps = actions[i].steps;
		Position npos = MakeMove(her, actions[i]);

		Board nboard;
		double score;
		if (!(nboard = TryMove(board, her, d, steps))) // she tries to move
			continue;
			
		score = MaxValue(nboard, my, npos, alpha, beta, depth+1).score;

		if (score < beta) {
			beta = score;
			minaction = Action(d, steps);
		}

		if (beta <= alpha)
			return ScoreAction(beta, minaction);
	}

	return ScoreAction(beta, minaction);
}


Position MyPlayer::Move(Board board, Position my, Position her)
{
	if (isolated_) // if isolated, do local move!
		return LocalMove(board, my);
	else { // otherwise, adversial game!
		// check isolation
		if (IsIsolated(board, my, her)) {
			isolated_ = true;
			hersteps_ = MaxClosure(board, her);
			return LocalMove(board, my);
		}
		else
			return AlphaBeta(board, my, her);
	}
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
		return MakeMove(my, action);
	}
}
