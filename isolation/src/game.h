#ifndef GAME_H_
#define GAME_H_

#include "types.h"
#include "player.h"

bool get(Board board, int row, int col);
void set(Board &board, int row, int col);

const int kBoardSize = 8;
const Board kInvalidBoard = 0L;

class Game
{
 private:
	static const char kFirstSymbol = 'x';
	static const char kSecondSymbol = 'o';
	static const Position kFirstPos;
	static const Position kSecondPos;

	Board board_;
	Position positions_[2];
	Player *players_[2];

	/* return false if the move is illegal */
	bool ApplyMove(int mover, Position move);
	bool DoMove(Position cur, int ro, int co, unsigned int nsteps);
	void Gameover(int mover);

 public:
	Game() {
		board_ = 0L;
		positions_[0] = kFirstPos;
		positions_[1] = kSecondPos;
		set(board_, kFirstPos.row, kFirstPos.col);
		set(board_, kSecondPos.row, kSecondPos.col);
	}

	~Game();
	
	void AddFirstPlayer(Player *player) {
		players_[0] = player;
	}
	void AddSecondPlayer(Player *player) {
		players_[1] = player;
	}

	void Play();

	/* print the game board */
	void print();
};

#endif // GAME_H_
