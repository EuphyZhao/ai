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


#endif // TYPES_H_
