#ifndef TABLE_H_
#define TABLE_H_

#include <set>
#include <map>
#include "types.h"
#include "game.h"

using namespace std;

struct Entry
{
	Board board;
	Position my;
	Position her;
	int depth;
    int isolated; // 0 if unknown, 1 if isolated, -1 if not
	double score;

Entry(Board b, Position m, Position h) : 
	board(b), my(m), her(h), isolated(0) {}	

Entry(Board b, Position m, Position h, int i) : 
	board(b), my(m), her(h), isolated(i) {}	

Entry(Board b, Position m, Position h, double s) : 
	board(b), my(m), her(h), isolated(0), score(s) {}	
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
	// return 0, if unknown
	// return 1, if isolated
	// return -1 if not isolated
	int isolated(Board board, Position my, Position her) {
		set<Entry>::iterator it = table_.find(Entry(board,my,her,true));
		
		// if entry does not exists
		// or if isolation test is not done
		if (it == table_.end() || it->isolated == 0)
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
		int i = isolated ? 1 : -1;
		table_.insert(Entry(board, my, her, i));
		if (table_.size() % 10000 == 0)
			cout << "Table size:" << table_.size() << endl;

	}

	double LookupScore(Board board, Position my, Position her) {
		set<Entry>::iterator it = table_.find(Entry(board,my,her));
		
		// if entry does not exists
		// or if the isolation test is done
		if (it == table_.end() || it->isolated)
			return 0;

		hits_++;
		if (hits_ % 10000 == 0)
			cout << "hits:" << hits_ << endl;

		return it->score;
	}


	void InsertScore(Board board, Position my, Position her, double score) {
		table_.insert(Entry(board, my, her, score));
		if (table_.size() % 10000 == 0)
			cout << "Table size:" << table_.size() << endl;
	}


	unsigned int size() { return table_.size();}
};

#endif // TABLE_H_
