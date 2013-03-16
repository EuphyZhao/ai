#include <cassert>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <set>
#include <iostream>
#include "game.h"
#include "player.h"

using namespace std;

const Position Game::kFirstPos(0,0);
const Position Game::kSecondPos(7,7);

const int kOffsets[8][2] = {
	{-1,0},{1,0},{0,-1},{0,1},
	{-1,-1},{-1,1},{1,-1},{1,1}};


bool operator==(const Position& lhs, const Position& rhs)
{
    return lhs.row == rhs.row && lhs.col == rhs.col;
}

bool operator<(const Position& lhs, const Position& rhs)
{
	if (lhs.row != rhs.row)
		return lhs.row < rhs.row;
	else
		return lhs.col < rhs.col;
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
	for (unsigned int step = 1; step <= nsteps; step++) {
		setpos(board_, row + ro * step, col + co * step);
	}
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

/*
 * Current mover lose the game
 */
void Game::Gameover(int mover)
{
	cout << players_[mover]->name() << " lose!" << endl;
}

void Game::Play()
{
	print();

	int mover = 0;
	while (1) {
		Position yourpos = positions_[mover];
		Position herpos = positions_[1-mover];
		Position move = players_[mover]->Move(board_, yourpos, herpos);

		// update the board; if illegal move, terminate the game
		if (!ApplyMove(mover, move)) {
			Gameover(mover);
			return;
		}
		// update the mover's position
		positions_[mover] = move;

		cout << players_[mover]->name() << " moves to (" << move.row+1 << "," << move.col+1 << ")" << endl;

		print();

		getchar();

		// flip the turn
		mover = 1 - mover;
	}
}


bool getpos(Board board, int row, int col)
{
	int shift = row * 8 + col;
	return (board >> shift) & 1L;
}

void setpos(Board &board, int row, int col)
{
	int shift = row * 8 + col;
	board |= (1L << shift);
}


void print(Board board, Position my, Position her)
{

	// print column header
	for (int c = 0; c < kBoardSize; c++) {
		if (c==0)
			cout << " ";
		cout << " " << c + 1;
	}
	cout << endl;


	for (int r = 0; r < kBoardSize; r++) {
		cout << r + 1;
		for (int c = 0; c < kBoardSize; c++) {
			cout << " ";
			if (my.row == r && my.col == c)
				cout << Game::kFirstSymbol;
			else if (her.row == r && her.col == c)
				cout << Game::kSecondSymbol;
			else if (getpos(board, r, c))
				cout << '*';
			else
				cout << '-';
		}
		cout << endl;
	}
}


void Game::print()
{

	// print column header
	for (int c = 0; c < kBoardSize; c++) {
		if (c==0)
			cout << " ";
		cout << " " << c + 1;
	}
	cout << endl;


	for (int r = 0; r < kBoardSize; r++) {
		cout << r + 1;
		for (int c = 0; c < kBoardSize; c++) {
			cout << " ";
			if (positions_[0].row == r && positions_[0].col == c)
				cout << kFirstSymbol;
			else if (positions_[1].row == r && positions_[1].col == c)
				cout << kSecondSymbol;
			else if (getpos(board_, r, c))
				cout << '*';
			else
				cout << '-';
		}
		cout << endl;
	}
}


int main(int argc, char *argv[])
{
	Game g;
	Player *first = new MyPlayer("my");
	Player *second = new DumbPlayer("dumb");
	g.AddFirstPlayer(first);
	g.AddSecondPlayer(second);
	g.Play();
}
