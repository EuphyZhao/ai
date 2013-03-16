#include <iostream>
#include "util.h"
#include "game.h"
#include "types.h"

using namespace std;

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


void print(Board board, Position playerA, Position playerB)
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
			if (playerA.row == r && playerA.col == c)
				cout << kFirstSymbol;
			else if (playerB.row == r && playerB.col == c)
				cout << kSecondSymbol;
			else if (getpos(board, r, c))
				cout << '*';
			else
				cout << '-';
		}
		cout << endl;
	}
}
