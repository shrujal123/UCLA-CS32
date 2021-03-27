#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>
#include <iostream>
using namespace std;

const int CHILDREN_SIZE = 27;

class StudentSpellCheck : public SpellCheck {
public:
    StudentSpellCheck() { root = getNode(); } //initializes the root node and all its children to null
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
    void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:

    //struct of words and positions
    struct Word
    {
        string text;
        Position pos;
    };

    //creates the trie structure
	struct TrieNode {

		struct TrieNode* children[CHILDREN_SIZE];
		bool isWordEnd;

	};

    //sets all 27 nodes of the child node to NULL
    TrieNode* getNode()
    {
        TrieNode* newNode = new TrieNode;
        newNode->isWordEnd = false;

        for (int i = 0; i < CHILDREN_SIZE; i++)
        {
            newNode->children[i] = NULL;
        }

        return newNode;
    }

    //inserts a word into the trie
    void insert(TrieNode *root, string word)
    {
        //sets the root node
        TrieNode* currNode = root;
        int ind = 0;

        for (int i = 0; i < word.size(); i++)
        {
            //check for apostrophe
            if (word[i] == '\'')
            {
                ind = 26;
            }
            else
            {
                ind = (word[i] - 'a');
            }
            if (!currNode->children[ind])
            {
                currNode->children[ind] = getNode();
            }

            //moves to the next node so next letter of the word
            currNode = currNode->children[ind];
        }

        currNode->isWordEnd = true;
    }

    // Returns true if the word exists in the trie 
    bool search(TrieNode *root, string word)
    {
        TrieNode* currNode = root;
        int ind = 0;

        for (int i = 0; i < word.size(); i++)
        {
            //check for apostrophe
            if (word[i] == '\'')
            {
                ind = 26;
            }
            else
            {
                ind = (word[i] - 'a');
            }
            if (!currNode->children[ind])
            {

                return false;
            }

            //moves to the next node so next letter of the word
            currNode = currNode->children[ind];
        }

        if (currNode != NULL && currNode->isWordEnd)
        {
            return true;
        }

        return false;
    }

    //declare the root node of the trie
    TrieNode *root;
    void clearTree(TrieNode *node);

};

#endif  // STUDENTSPELLCHECK_H_
