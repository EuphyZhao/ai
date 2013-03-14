#ifndef TYPES_H_
#define TYPES_H_

struct Position
{
	int row;
	int col;
Position() : row(0), col(0) {}
Position(int r, int c) : row(r), col(c) {}
};


typedef unsigned long Board;


enum Direction {
	N,S,W,E,NW,NE,SW,SE,
	INVALID_DIR
};


extern const int kOffsets[8][2];


struct Action
{
	Direction dir;
	int steps;

Action(Direction d, int s) : dir(d), steps(s) {}
};

struct ScoreAction
{
	double score;
	Action action;
ScoreAction(double s, Action act) : score(s), action(act) {}
};

const Action kInvalidAction = Action(INVALID_DIR, -1);

#endif // TYPES_H_
