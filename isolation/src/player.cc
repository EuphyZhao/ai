#include <iostream>
#include "player.h"

Position HumanPlayer::Move(Board board, Position my, Position her)
{
	int row, col;
	cin >> row;
	cin >> col;
	return Position(row, col);
}


Position MyPlayer::Move(Board board, Position my, Position her)
{
	return Position();
}
