#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <list>
#include <iostream>
#include <fstream>

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
	: TextEditor(undo), m_currRow(0), m_currCol(0), doSubmit(true) //initializes the member variables
{
	text.push_back(""); //push an empty string onto the list
	m_row = text.begin(); //initialize the iterator
}

StudentTextEditor::~StudentTextEditor() { text.clear(); } //destructor isn't doing anything

//loads the file
bool StudentTextEditor::load(std::string file) {

	if (text.size() != 0)
	{
		reset();
	}

	ifstream infile(file);
	if (!infile)
	{
		cerr << "Error: Cannot open file" << endl;
		return false;
	}

	string s;
	while (getline(infile, s))
	{
		//strips the \r character
		for (int i = 0; i < s.size(); i++)
		{
			if (s.at(i) == '\r')
			{
				s.erase(s.begin() + i);
			}
		}
		text.push_back(s);
	}
	resetRow();
	return true;
}

//saves the file to the specified file name
bool StudentTextEditor::save(std::string file) {

	ofstream outfile(file);
	if (!outfile)
	{
		cerr << "Error: Cannot create results.txt!" << endl;
		return false;
	}

	list<string>::iterator lines = text.begin();
	for (int i = 0; i < text.size(); i++)
	{
		outfile << *(lines++) << '\n';
	}
	return true;
}

//resets the editor
void StudentTextEditor::reset() {
	getUndo()->clear();
	text.clear();
	m_currRow = 0;
	m_currCol = 0;
}

//moves the cursor based on the arrow keys
void StudentTextEditor::move(Dir dir) {
	//gets the direction of the key press and increments or decrements the row/col
	switch (dir)
	{
	case UP:
		prevRow();
		break;
	case DOWN:
		advanceRow();
		break;
	case RIGHT:
		m_currCol++;
		break;
	case LEFT:
		m_currCol--;
		break;
	case HOME:
		m_currCol = 0;
		break;
	case END:
		m_currCol = getCurrRow().size();
		break;
	default:
		break;
	}
	//bounds check
	if (m_currCol < 0)
	{
		if (m_currRow > 0 && dir == LEFT)
		{
			prevRow();
			m_currCol = getCurrRow().size();
		}
		else
		{
			m_currCol = 0;
		}
	}
	if (m_currCol > getCurrRow().size())
	{
		int nextRow = m_currRow + 1;
		if (nextRow < text.size() && dir == RIGHT)
		{
			m_currCol = 0;
			advanceRow();
		}
		else
		{
			m_currCol = getCurrRow().size();
		}
	}
}

//delete key using the backspace function
void StudentTextEditor::del() {
	int row = m_currRow;
	int col = m_currCol;
	move(RIGHT);
	if (row != m_currRow || col != m_currCol)
	{
		backspace();
	}
}

//removes a character by splitting the line
void StudentTextEditor::backspace() {
	
	int prevCol = 0;
	if (m_currCol - 1 > 0)
	{
		prevCol = m_currCol - 1;
	}
	if (m_currCol > 0)
	{
		string beforeBackspace = getCurrRow().substr(0, prevCol);
		string afterBackspace = getCurrRow().substr(m_currCol);
		char ch = getCurrRow().at(prevCol);
		(*m_row) = beforeBackspace + afterBackspace;
		m_currCol--;
		if (doSubmit)
		{
			getUndo()->submit(Undo::Action::DELETE, m_currRow, m_currCol, ch);
		}
	}
	else
	{
		//if the cursor is at the beginning of the line, join the lines 
		if (m_currRow > 0)
		{
			string currRow = getCurrRow();
			prevRow();
			int pos = getCurrRow().size(); 
			(*m_row) += currRow;
			advanceRow();
			m_row = text.erase(m_row);
			prevRow();
			m_currCol = pos;
			getUndo()->submit(Undo::Action::JOIN, m_currRow, m_currCol);
		}
		//else don't do anything if at beginning of file
		else
		{
			m_currCol = 0;
		}
	}
}

//inserts a character at the position of the cursor in the current line
void StudentTextEditor::insert(char ch) {
	string beforeInsert = getCurrRow().substr(0, m_currCol);
	string afterInsert = ch + getCurrRow().substr(m_currCol);
	(*m_row) = beforeInsert + afterInsert;
	m_currCol++;
	if (doSubmit)
	{
		getUndo()->submit(Undo::Action::INSERT, m_currRow, m_currCol, ch);
	}
}

//adds a line break or a new line
void StudentTextEditor::enter() {
	string beforeSplit;
	string afterSplit;
	beforeSplit = getCurrRow().substr(0, m_currCol);
	afterSplit = getCurrRow().substr(m_currCol);
	advanceRow(false); //for bounds check on last line
	m_row = text.insert(m_row, afterSplit);
	prevRow();
	(*m_row) = beforeSplit;
	advanceRow();
	m_currCol = 0;
	getUndo()->submit(Undo::Action::SPLIT, m_currRow, m_currCol);
}

//returns the position of row and col
void StudentTextEditor::getPos(int& row, int& col) const {
	row = m_currRow;
	col = m_currCol;
}

//gets the lines of text in the editor and pushes it to the lines array
int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
	int numAvailable = text.size();
	if ((startRow < 0 || numRows < 0) || startRow >= numAvailable)
	{
		return -1;
	}
	lines.clear();

	list<string>::const_iterator it = text.begin();
	for (int row = startRow; row < numAvailable && row < startRow + numRows; row++, it++)
	{
		lines.push_back(*it);
	}

	return lines.size();
}

//Undo function that checks all the different actions
void StudentTextEditor::undo() {
	// checks if there is an object to undo
	if (getUndo() == nullptr)
	{
		return;
	}
	//initializes variables to 0
	int count = 0;
	int row = 0;
	int col = 0;

	//switch statement to check the action performed by the undo object
	switch (getUndo()->get(m_currRow, m_currCol, count, undoString))
	{
	//inserts characters back into the text file for wanting to undo a delete 
	case Undo::Action::INSERT:
		row = m_currRow;
		col = m_currCol;
		doSubmit = false;
		//adds the batch of characters
		for (int i = 0; i < undoString.size(); i++)
		{
			insert(undoString[i]);
		}
		doSubmit = false;
		m_currRow = row;
		m_currCol = col;
		break;
	//deletes characters from the text file for wanting to undo an insert
	case Undo::Action::DELETE:
		row = m_currRow;
		col = m_currCol;
		doSubmit = false;
		move(RIGHT);
		//removes the batch of characters
		for (int i = 0; i < count; i++)
		{
			backspace();
		}
		doSubmit = true;
		m_currRow = row;
		m_currCol = col - count + 1;
		break;
	//splits a line for wanting to undo a join
	case Undo::Action::SPLIT:
		row = m_currRow;
		col = m_currCol;
		doSubmit = false;
		enter();
		doSubmit = true;
		m_currRow = row + 1;
		m_currCol = 0;
		break;
	//joins a line for wanting to undo a line split
	case Undo::Action::JOIN:
		row = m_currRow;
		m_currRow = row - 1;
		col = getCurrRow().size();
		m_currRow = row;
		doSubmit = false;
		backspace();
		doSubmit = true;
		m_currRow = row - 1;
		m_currCol = col;
		break;
	default:
		break;
	}
}

//gets a pointer to the current row - helper function
string StudentTextEditor::getCurrRow()
{
	return *m_row;
}

//advanced the row iterator and number after doing a bounds check
bool StudentTextEditor::advanceRow(bool checkBounds) 
{
	int nextRow = m_currRow + 1;
	if (checkBounds && (nextRow < 0 || nextRow >= text.size()))
	{
		return false;
	}
	m_row++;
	m_currRow++;
	return true;
}

//retreats the row iterator and number after doing a bounds check
bool StudentTextEditor::prevRow(bool checkBounds)
{
	int prevRow = m_currRow - 1;
	if (checkBounds && (prevRow  < 0 || prevRow  >= text.size()))
	{
		return false;
	}
	m_row--;
	m_currRow--;
	return true;
}

//resets the position of the rowto the beginning of the file
void StudentTextEditor::resetRow()
{
	m_row = text.begin();
	m_currRow = 0;
}
