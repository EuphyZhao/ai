#include <cassert>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <set>
#include <iostream>

#include "game.h"
#include "player.h"
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

bool Game::DoMove(Position cur, int ro, int co, unsigned int nsteps)
{
	int row = cur.row, col = cur.col;
	// validate
	for (unsigned int step = 1; step <= nsteps; step++) {
		if (getpos(board_, row + ro * step, col + co * step)) {
			return false;
		}
	}
	// do move
	setpos(board_, row + ro * nsteps, col + co * nsteps);

	return true;
}

bool Game::ApplyMove(int mover, Position move)
{
	// check if loses
	if (move.row == -1 && move.col == -1) {
		cout << players_[mover]->name() << " has no way to go." << endl;
		return false;
	}

	// check if moves out of the board
	if (move.row < 0 || move.col < 0 ||
		move.row >= kBoardSize || move.col >= kBoardSize) {
		cout << players_[mover]->name() << " moves out of the board." << endl;
		return false;
	}

	// check if stay
	if (move == positions_[mover]) {
		cout << players_[mover]->name() << " did not move." << endl;
		return false;
	}

	Position current = positions_[mover];
	int ro, co, nsteps;

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
		cout << players_[mover]->name() << " move into a wrong direction." << endl;
		return false;
	}
	if (DoMove(current, ro, co, nsteps))
		return true;
	else {
		cout << players_[mover]->name() << " jumps over a occupied square" << endl;
		return false;
	}
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

		// update the board; if illegal move, terminate the game
		if (!ApplyMove(mover, move)) {
			Gameover(mover);
			Print();
			return 1-mover;
		}
		// update the mover's position
		positions_[mover] = move;

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

	int iwin = 0, shewin = 0;
	for (int i = 0; i < 10; i++) {
		Game g;
		Player *first = new MyPlayer("my");
		Player *second = new DumbPlayer("dumb");
		g.AddFirstPlayer(first);
		g.AddSecondPlayer(second);
		int winner = g.Play();
		if (winner==0)
			iwin++;
		else
			shewin++;

		getchar();
	}

	cout << iwin << ":" << shewin << endl;
}
