#include <iostream>
#include <stdlib.h>


#include "types.h"
#include "player.h"
#include "game.h"

bool TryMove(Board board, Position cur, Direction dir, unsigned int nsteps)
{
	int row = cur.row, col = cur.col;
	int ro = kOffsets[dir][0];
	int co = kOffsets[dir][1];
	
	int rend = row + ro*nsteps;
	int cend = col + co*nsteps;
	if (rend < 0 || cend < 0 ||
		rend >= kBoardSize || rend >= kBoardSize)
		return false;

	for (unsigned int step = 1; step <= nsteps; step++) {
		if (get(board, row + ro * step, col + co * step)) {
			return false;
		}
	}
	return true;
}

Position MakeMove(Position cur, Direction dir, unsigned int nsteps)
{
	int row = cur.row, col = cur.col;
	int ro = kOffsets[dir][0];
	int co = kOffsets[dir][1];

	return Position(row+ro*nsteps, col+co*nsteps);
}

Position HumanPlayer::Move(Board board, Position my, Position her)
{
	int row, col;
	cin >> row;
	cin >> col;
	return Position(row-1, col-1);
}

Position DumbPlayer::Move(Board board, Position my, Position her)
{
	unsigned int attempts = 0;
	while (1) {
		if (attempts > kMaxAttempts)
			return Position(-1,-1);

		// random direction
		Direction d = (Direction)(rand() % 8);

		// random step length
		unsigned int nsteps = rand() % 8 + 1; // at least move on step

		if (TryMove(board, my, d, nsteps)) {
			return MakeMove(my, d, nsteps);
		}
	}
}


Position MyPlayer::Move(Board board, Position my, Position her)
{
	return Position();
}
