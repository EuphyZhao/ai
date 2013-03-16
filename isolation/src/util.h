#ifndef UTIL_H_
#define UTIL_H_

#include "types.h"

void print(Board board, Position playerA, Position playerB);

bool getpos(Board board, int row, int col);

void setpos(Board &board, int row, int col);


#endif // UTIL_H_
