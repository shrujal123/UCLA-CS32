#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

//destructor deletes the root node
StudentSpellCheck::~StudentSpellCheck() {
	clearTree(root);
}

void StudentSpellCheck::clearTree(TrieNode* node)
{
	if (node != nullptr)
	{
		for (int i = 0; i < CHILDREN_SIZE; i++)
		{
			clearTree(node->children[i]);
		}
		delete node;
	}
}

//load the dictionary file into trie 
bool StudentSpellCheck::load(std::string dictionaryFile) {

	ifstream infile(dictionaryFile);
	if (!infile)
	{
		cerr << "Error: Cannot open file" << endl;
		return false;
	}

	clearTree(root);
	root = getNode();

	string s;
	while (getline(infile, s))
	{
		
		//takes out unnecessary characters or spaces from 
		string insertStr;
		for (int i = 0; i < s.size(); i++)
		{
			if (isalpha(s[i]) || s[i] == '\'')
			{
				insertStr += tolower(s[i]);
			}
		}

		insert(root, insertStr);

	}

	return true; 
}

//loops through every possible character change of a mispelled word and adds it to the suggestions array
bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {

	//makes the search word syntactically correct
	string searchWord;
	for (int i = 0; i < word.size(); i++)
	{
		if (isalpha(word[i]) || word[i] == '\'')
		{
			searchWord += tolower(word[i]);
		}
	}

	if (search(root, searchWord))
	{
		return true;
	}

	//loops through the words in the trie to give suggestions
	suggestions.clear();
	int countSuggestions = 0;
	for (int i = 0; i < searchWord.size(); i++)
	{
		string suggestWord = searchWord;
		//checks every character at that position of the word 
		for (int j = 0; j < 27; j++)
		{
			if (j == 26)
			{
				suggestWord[i] = '\'';
			}
			else
			{
				suggestWord[i] = 'a' + j;
			}
			if (search(root, suggestWord))
			{
				if (countSuggestions < max_suggestions)
				{
					suggestions.push_back(suggestWord);
					countSuggestions++;
				}
			}
			if (countSuggestions == max_suggestions)
			{
				return false;
			}
		}
	}

	return false;
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {

	//create a vector of Word structs which holds a word text and the position of it
	problems.clear();
	vector<Word> words;
	Word word;
	int start = 0;
	int end = 0;
	bool first = true;
	string tempLine = line;
	tempLine += " ";
	//finds the position and text of the word and adds it to the vector
	for (int i = 0; i < tempLine.size(); i++)
	{
		if (isalpha(tempLine[i]) || tempLine[i] == '\'')
		{
			word.text += tolower(tempLine[i]);
			end++;
			if (first)
			{
				start = i;
				first = false;
				end = i;
			}
		}
		else
		{
			if (word.text != "")
			{
				word.pos.start = start;
				word.pos.end = end;
				words.push_back(word);
				word.text = "";
				first = true;
			}
		}

	}

	//checks all the parsed words from the line to the trie and if its not a word, then add it to the problems vector
	for (int i = 0; i < words.size(); i++)
	{
		if (!search(root, words[i].text))
		{
			problems.push_back(words[i].pos);
		}
	}
	
}
