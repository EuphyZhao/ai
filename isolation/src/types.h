#ifndef TYPES_H_
#define TYPES_H_

#include <limits>

const double DMIN = std::numeric_limits<double>::min();
const double DMAX = std::numeric_limits<double>::max();

struct Position
{
	int row;
	int col;
Position() : row(-1), col(-1) {}
Position(int r, int c) : row(r), col(c) {}
};


typedef unsigned long Board;

enum Direction {
	N,S,W,E,NW,NE,SW,SE,
	INVALID_DIR
};


//extern const int kOffsets[8][2];

struct Action
{
	Direction dir;
	int steps;

Action(Direction d, int s) : dir(d), steps(s) {}
};

const Action kInvalidAction = Action(INVALID_DIR, -1);

struct ScoreAction
{
	double score;
	Action action;
ScoreAction(double s, Action act) : score(s), action(act) {}
};

bool inline operator==(const Position& lhs, const Position& rhs)
{
    return lhs.row == rhs.row && lhs.col == rhs.col;
}

bool inline operator<(const Position& lhs, const Position& rhs)
{
	if (lhs.row != rhs.row)
		return lhs.row < rhs.row;
	else
		return lhs.col < rhs.col;
}


struct BfsNode
{
	Board board;
	Position cur;
	
	int depth; // used in max closure

BfsNode(Board b, Position pos) : board(b), cur(pos) {}	

BfsNode(Board b, Position pos, int d)
: board(b), cur(pos), depth(d) {}	
};

struct BfsNodeCompare {
	bool operator() (const BfsNode& lhs, const BfsNode& rhs) const {
		if (lhs.board != rhs.board)
			return lhs.board < rhs.board;
		else
			return lhs.cur < rhs.cur;
	}
};

#endif // TYPES_H_
