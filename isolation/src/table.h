#ifndef TABLE_H_
#define TABLE_H_

#include <set>
#include "types.h"
#include "game.h"

using namespace std;

struct Entry
{
	Board board;
	Position my;
	Position her;
	int depth;
	double score;
	bool isolated;

Entry(Board b, Position m, Position h, bool i) : 
	board(b), my(m), her(h), isolated(i)
{}	

};

struct EntryCompare {
	bool operator() (const Entry& lhs, const Entry& rhs) const {
		if (lhs.board != rhs.board)
			return lhs.board < rhs.board;
		else if (lhs.my.row != rhs.my.row || lhs.my.col != rhs.my.col)
			return lhs.my < rhs.my;
		else
			return lhs.her < rhs.her;
	}
};


/*
 * Tranposition table
 */
class Table
{
 private:
	set<Entry, EntryCompare> table_;
	int hits_;
	
 public:
 Table() : hits_(0) {}
		

	// check if current entry has exists
	// return 0, if not exists
	// return 1, if isolated
	// return -1 if not isolated
	int isolated(Board board, Position my, Position her) {
		set<Entry>::iterator it = table_.find(Entry(board,my,her,true));
		if (it == table_.end())
			return 0;

		hits_++;
		if (hits_ % 10000 == 0)
			cout << "hits:" << hits_ << endl;

		Entry e = *it;
		if (e.isolated)
			return 1;
		else
			return -1;
	}

	void insert(Board board, Position my, Position her, bool isolated) {
		table_.insert(Entry(board, my, her, isolated));
		if (table_.size() % 10000 == 0)
			cout << "Table size:" << table_.size() << endl;

	}

	int size() { return table_.size();}
};

#endif // TABLE_H_
