#include <cassert>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <set>
#include <iostream>

#include "game.h"
#include "player.h"
#include "dumb.h"
#include "util.h"

using namespace std;

Game::Game() {
	board_ = kInvalidBoard;
	positions_[0] = kFirstPos;
	positions_[1] = kSecondPos;
	setpos(board_, kFirstPos.row, kFirstPos.col);
	setpos(board_, kSecondPos.row, kSecondPos.col);
}


Game::~Game()
{
	for (int player = 0; player < 1; player++) {
		if (players_[player] != NULL)
			delete players_[player];
		players_[player] = NULL;
	}
}


bool IsDead(Board board, Position pos)
{
	return GenerateActions(board, pos).size() == 0;
}


bool ValidateMove(Board board, Position current, Position move)
{
	if (move.row == -1 && move.col == -1) {
		cout << "no way to go." << endl;
		return false;
	}

	// check if moves out of the board
	if (move.row < 0 || move.col < 0 ||
		move.row >= kBoardSize || move.col >= kBoardSize) {
		cout << "moves out of the board." << endl;
		return false;
	}

	// check if stay
	if (move == current) {
		cout << "did not move." << endl;
		return false;
	}

	int ro, co;
	unsigned int nsteps;

	if (current.row == move.row) { // horizontal move
		ro = 0;
		co = current.col < move.col ? 1 : -1;
		nsteps = abs(current.col - move.col);
	}
	else if (current.col == move.col) { 	// vertical move
		ro = current.row < move.row ? 1 : -1;
		co = 0;
		nsteps = abs(current.row - move.row);
	}
	else if (abs(current.row-move.row)==abs(current.col-move.col)) {
		// diagonal move
		ro = current.row < move.row ? 1 : -1;
		co = current.col < move.col ? 1 : -1;
		nsteps = abs(current.row-move.row);
	}
	else {
		cout << "move into a wrong direction." << endl;
		return false;
	}

	for (unsigned int step = 1; step <= nsteps; step++) {
		if (getpos(board, current.row + ro * step, current.col + co * step)) {
			cout << "jumps over a occupied square" << endl;
			return false;
		}
	}
	return true;
}

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


void Game::ApplyMove(int mover, Position move)
{
	setpos(board_, move.row, move.col);
	positions_[mover] = move;
}

void Game::Gameover(int mover)
{
	cout << players_[mover]->name() << " lose!" << endl;
}

int Game::Play()
{
	//	Print();

	int mover = 0;
	while (1) {
		Position yourpos = positions_[mover];
		Position herpos = positions_[1-mover];
		Position move = players_[mover]->Move(board_, yourpos, herpos);

		// Validate
		if (!ValidateMove(board_,positions_[mover], move)) {
			Gameover(mover);
			Print();
			return 1 - mover;
		}

		// update the board
		ApplyMove(mover, move);

		cout << players_[mover]->name() << " moves to (" << move.row+1 << "," << move.col+1 << ")" << endl;

		Print();

		// flip the turn
		mover = 1 - mover;
	}
}


void Game::Print()
{
	print(board_, positions_[0], positions_[1]);
}

int main(int argc, char *argv[])
{
	srand(time(NULL));

	int firstwin = 0, secondwin = 0;
	for (int i = 0; i < 20; i++) {
		Game g;
		Player *first = new MyPlayer("my simple", 3);
		Player *second = new MyPlayer("my advanced", 5);
		g.AddFirstPlayer(second);
		g.AddSecondPlayer(first);
		int winner = g.Play();
		if (winner==0)
			firstwin++;
		else
			secondwin++;
	}

	cout <<  firstwin << ":" << secondwin << endl;
}
