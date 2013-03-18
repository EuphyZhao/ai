#ifndef HUMAN_H_
#define HUMAN_H_

#include "game.h"
#include "types.h"
#include "player.h"

class HumanPlayer : public Player
{
 public:
 HumanPlayer(string name) : Player(name) {}

	virtual Position Move(Board board, Position my, Position her)
	{
		
		if (IsDead(board, my)) {
			cout << "I have no way to go..." << endl;
			return Position(-1, -1);
		}

		int row, col;
		cout << ">>";
		cin >> row;
		cin >> col;
		
		// Validate the move
		while (!ValidateMove(board, my, Position(row-1, col-1))) {
			cout << "Invalid move, please try again." << endl;
			cout << ">>";
			cin >> row;
			cin >> col;
		}
		return Position(row-1, col-1);
	}

};


#endif // HUMAN_H_
