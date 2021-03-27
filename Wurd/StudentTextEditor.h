#ifndef STUDENTTEXTEDITOR_H_
#define STUDENTTEXTEDITOR_H_

#include "TextEditor.h"
#include <list>
#include <vector>
using namespace std;

class Undo;

class StudentTextEditor : public TextEditor {
public:
	//public member functions
	StudentTextEditor(Undo* undo);
	~StudentTextEditor();
	bool load(std::string file);
	bool save(std::string file);
	void reset();
	void move(Dir dir);
	void del();
	void backspace();
	void insert(char ch);
	void enter();
	void getPos(int& row, int& col) const;
	int getLines(int startRow, int numRows, std::vector<std::string>& lines) const;
	void undo();

private:
	//member variables to keep track of the current row and column
	int m_currRow;
	int m_currCol;
	//sets an iterator to iterate through the rows of the line
	list<string>::iterator m_row;
	list<string> text;
	//helper functions that do bounds checks and returns values that I can use in use to group lines of code in multiple places
	string getCurrRow();
	bool advanceRow(bool checkBounds = 1);
	bool prevRow(bool checkBounds = 1);
	void resetRow();

	//member variables for the undo function
	string undoString;
	bool doSubmit;
	

};

#endif // STUDENTTEXTEDITOR_H_
