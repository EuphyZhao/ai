#ifndef HUMAN_H_
#define HUMAN_H_

#include <sstream>
#include "game.h"
#include "types.h"
#include "player.h"

class HumanPlayer : public Player
{
 private:
	bool parse_string(string str, int *row, int *col) {
		istringstream iss(str);
		iss >> *row >> *col >> std::ws;
		if (iss.fail() || !iss.eof())
			return false;
		else
			return true;
	}

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

		string str;
		getline(cin, str);

		// Validate the move
		while (!parse_string(str, &row, &col) || !ValidateMove(board, my, Position(row-1, col-1))) {
			cout << "Invalid move, please try again." << endl;
			cout << ">>";
			getline(cin, str);
		}
		return Position(row-1, col-1);
	}

};


#endif // HUMAN_H_
