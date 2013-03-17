#ifndef GAME_H_
#define GAME_H_

#include <vector>

#include "types.h"
#include "player.h"

const int kBoardSize = 8;
const Board kInvalidBoard = 0L;
const char kFirstSymbol = 'x';
const char kSecondSymbol = 'o';
const Position kFirstPos(0,0);
const Position kSecondPos(7,7);

const int kOffsets[8][2] = {
	{-1,0},{1,0},{0,-1},{0,1},
	{-1,-1},{-1,1},{1,-1},{1,1}};


/*******************************************
 * Utilities
 ******************************************/
bool ValidateMove(Board board, Position current, Position move);
bool IsDead(Board board, Position pos);
std::vector<Action> GenerateActions(Board board, Position cur);

class Player;

class Game
{
 private:
	Board board_;
	Position positions_[2];
	Player *players_[2];

	void ApplyMove(int mover, Position move);
	void Gameover(int mover);

 public:

	Game();

	~Game();
	
	void AddFirstPlayer(Player *player) {
		players_[0] = player;
	}
	void AddSecondPlayer(Player *player) {
		players_[1] = player;
	}
	
	// return the index of the winner
	int Play();
	void Print();
};

#endif // GAME_H_
