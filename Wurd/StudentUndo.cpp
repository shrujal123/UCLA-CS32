#include "StudentUndo.h"

Undo* createUndo()
{
	return new StudentUndo;
}

StudentUndo::~StudentUndo()
{
	clear();
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
	// check what the top of the stack is after popping and if its a letter or \' then change the text then push back onto the stack
	//check if its a space then it should be its own struct

	//initializes the undo object and its components
	UndoObj undo;
	undo.action = action;
	undo.row = row;
	undo.col = col;
	undo.undoStr = ch;

	//checks if the stack is empty
	if (!undoStack.empty())
	{
		//creates a temp tring that we batch together characters of the same action into
		string temp = undoStack.top().undoStr;
		if (undoStack.top().action == Undo::Action::INSERT)
		{
			if (ch == ' ')
			{
				undo.undoStr = " ";
			}
			else if (action == Undo::Action::INSERT)
			{
				undoStack.pop();
				temp += ch;
				undo.undoStr = temp;
			}
		}
		else if (undoStack.top().action == Undo::Action::DELETE)
		{
			if (action == Undo::Action::DELETE)
			{
				undoStack.pop();
				temp = ch + temp;
				undo.undoStr = temp;
			}
		}
	}

	//pushes the undo object onto the stack
	undoStack.push(undo);
	
}

//gets the undo action to be performed
StudentUndo::Action StudentUndo::get(int &row, int &col, int& count, std::string& text) {
	//checks if the stack is empty
	if (undoStack.empty())
	{
		return Undo::Action::ERROR;
	}
	row = undoStack.top().row;
	col = undoStack.top().col;
	//checks all the actions, gets the text to remove and position of the cursor
	switch (undoStack.top().action)
	{
	case INSERT:
		//count is the size of the word because we batched these letters
		count = undoStack.top().undoStr.size();
		col--;
		text = undoStack.top().undoStr;
		undoStack.pop();
		return Undo::Action::DELETE;
	case DELETE:
		count = 1;
		text = undoStack.top().undoStr;
		undoStack.pop();
		return Undo::Action::INSERT;
	case SPLIT:
		count = 1;
		undoStack.pop();
		return Undo::Action::JOIN;
	case JOIN:
		count = 1;
		undoStack.pop();
		return Undo::Action::SPLIT;
	default:
		return Undo::Action::ERROR;
	}
}

//pops all the items off the stack
void StudentUndo::clear() {
	for (int i = 0; i < undoStack.size(); i++)
	{
		undoStack.pop();
	}
}
