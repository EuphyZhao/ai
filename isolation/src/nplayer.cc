#include <iostream>
#include <limits>
#include <queue>
#include <set>
#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "game.h"
#include "table.h"
#include "util.h"
#include "nplayer.h"

using namespace std;

bool AggressivePlayer::IsIsolated(Board board, Position my, Position her)
{
	// check the table first
	// int lookup = table_.isolated(board, my, her);

	// if (lookup == 1)
	// 	return true;
	// else if (lookup == -1)
	// 	return false;
	
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

		// cutoff
		if (frontier.size() > kMaxIsolationNodes)
			return false;

		for (int d = 0; d < 8; d++) {
			Position next = MakeMove(node.cur, Action((Direction)d, 1));
			// we are reachable
			if (next == her) {
				//				table_.insert(board,my,her,false);
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

	//	table_.insert(board,my,her,true);
	return true;
}


// return the maximum number of steps
// conditioned on that we are isolated
// this is deterministic
int AggressivePlayer::MaxClosureDFS(Board board, Position cur, int depth, int maxdepth)
{
	// make sure when we reach the cut off
	// we will have one more step than the cutoff
	if (depth > maxdepth)
		return 0;

	int maxsteps = -1; // keep max steps I left
	for (int d = 0; d < 8; d++) {
		Position next = MakeMove(cur, Action((Direction)d, 1));
		Board nboard;
		if ((nboard=TryMove(board, cur, (Direction)d, 1))) {
			int steps = MaxClosureDFS(nboard, next, depth+1, maxdepth);
			if (steps > maxsteps)
				maxsteps = steps;
		}
	}

	return 1 + maxsteps;
}

// return the maximum number of steps
// conditioned on that we are isolated
// this is deterministic
int AggressivePlayer::MaxClosure(Board board, Position cur)
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

		// cutoff
		if (frontier.size() > kMaxIsolationNodes) {
			return maxsteps; // do not need to relax
		}

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



double AggressivePlayer::Eval(Board board, Position my, Position her, int depth)
{
	vector<Action> myactions = GenerateActions(board, my);
	vector<Action> heractions = GenerateActions(board, her);

	// she cannot move
	// we win!
	if (heractions.size() == 0)
		return DMAX;

	return 1.0 * myactions.size() / heractions.size();
}



// Estimate the likelihood that we are isolated
// and she controls more squres than me
double AggressivePlayer::Eval2(Board board, Position my, Position her)
{
	if (IsIsolated(board, my, her)) {
		int hersteps = MaxClosureDFS(board, her, 0, kMaxDFSDepth);
		int mysteps = MaxClosureDFS(board, my, 0, kMaxDFSDepth);
		if (mysteps > hersteps)
			return DMAX;
		else if (mysteps < hersteps)
			return DMIN;
	}
	return 0;
}

bool AggressivePlayer::Cutoff(Board board, Position my, Position her, int depth)
{
	if (steps_ > kEndGameSteps)
		return depth > kEndGameMaxDepth;
	else if (steps_ > kMidGameSteps)
		return depth > kMidGameMaxDepth;
	else
		return depth > kDefaultMaxDepth;
}


int AggressivePlayer::DoLocalMove(Board board, Position cur)
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
Position AggressivePlayer::LocalMove(Board board, Position my)
{
	Position best = Position(-1,-1);
	int maxsteps = 0;
	for (int d = 0; d < 8; d++) {
		Position next = MakeMove(my, Action((Direction)d, 1));
		Board nboard=TryMove(board, my, (Direction)d, 1);
		if (nboard) {
			int steps = DoLocalMove(nboard, next);
			if (steps >= maxsteps) {
				maxsteps = steps;
				best = next;
				if (steps > hersteps_) // we only need to do better than her
					break;
			}
		}
	}
	hersteps_--;
	return best;
}



Position AggressivePlayer::AlphaBeta(Board board, Position my, Position her)
{
	double alpha = DMIN;
	double beta = DMAX;

	ScoreAction sa = MaxValue(board, my, her, alpha, beta, 0);

	if (sa.action.steps == -1) {
		//		cout << "We will lose. But try our best." << endl;
		return LocalMove(board, my);
	}

	if (TryMove(board, my, sa.action.dir, sa.action.steps)) {
		return MakeMove(my, sa.action);
	}
	else {
		return Position(-1, -1);
	}
}

ScoreAction AggressivePlayer::MaxValue(Board board, Position my, Position her,
									   double alpha, double beta, int depth)
{
	// I lose
	if (IsDead(board, my))
		return ScoreAction(DMIN, kInvalidAction);

	// She lose
	if (IsDead(board, her)) {
		vector<Action> actions = GenerateActions(board, my);
		return ScoreAction(DMAX, actions[0]);
	}


	if (depth <= 4 && steps_ > kEagerGameSteps) {
		double score = Eval2(board, my, her);
		if (score < -1) { // it is the same as i will be killed
			return ScoreAction(DMIN, kInvalidAction);
		}
	}

	// Cutoff test
	if (Cutoff(board, my, her, depth))
		return ScoreAction(Eval(board, my, her, depth), kInvalidAction);

	Action maxaction = kInvalidAction;
	vector<Action> actions = GenerateActions(board, my);

	while (!actions.empty()) {
		int index = rand() % actions.size();
		Action action = actions[index];
		actions.erase(actions.begin()+index);

		Direction d = action.dir;
		int steps = action.steps;
		Position npos = MakeMove(my, action);

		Board nboard;
		double score = 0;
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


ScoreAction AggressivePlayer::MinValue(Board board, Position my, Position her,
									   double alpha, double beta, int depth)
{
	// She lose
	if (IsDead(board, her))
		return ScoreAction(DMAX, kInvalidAction);

	// I lose
	if (IsDead(board, my)) {
		vector<Action> actions = GenerateActions(board, her);
		return ScoreAction(DMIN, actions[0]);
	}

	// THIS IS AGGRESSIVE MODE

	if (depth <= 3 && steps_ > kEagerGameSteps) {
		double score = Eval2(board, my, her);
		if (score > 1) { // avoid rounding error
			return ScoreAction(DMAX, kInvalidAction);
		}
	}
	
	
	// Cutoff test
	if (Cutoff(board, my, her, depth))
		return ScoreAction(Eval(board, her, my, depth), kInvalidAction);

	Action minaction = kInvalidAction;
	vector<Action> actions = GenerateActions(board, her);

	while (!actions.empty()) {
		int index = rand() % actions.size();
		Action action = actions[index];
		actions.erase(actions.begin()+index);

		Direction d = action.dir;
		int steps = action.steps;
		Position npos = MakeMove(her, action);

		Board nboard;
		double score = 0;
		if (!(nboard = TryMove(board, her, d, steps))) // she tries to move
			continue;

		if (score == 0)
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

Position AggressivePlayer::Move(Board board, Position my, Position her)
{
	// increase the steps as we move
	steps_++;

	if (isolated_) // if isolated, do local move!
		return LocalMove(board, my);
	else { // otherwise, adversial game!
		// check isolation

		bool test = IsIsolated(board, my, her);
		if (test) {
			//			cout << "Switching to local mode." << endl;
			isolated_ = true;
			hersteps_ = MaxClosure(board, her);
			return LocalMove(board, my);
		}
		return AlphaBeta(board, my, her);
	}
}

