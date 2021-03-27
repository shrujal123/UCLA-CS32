#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <stack>
using namespace std;

class StudentUndo : public Undo {
public:

	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();
	~StudentUndo();

private:

	//Undo object structure
	struct UndoObj
	{
		Undo::Action action;
		int row;
		int col;
		string undoStr; //string of characters
	};

	//stack of Undo Objects
	stack<UndoObj> undoStack;
};


#endif // STUDENTUNDO_H_
