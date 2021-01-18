#ifndef _GRAMMAR_H
#define _GRAMMAR_H

#include<string>
#include<vector>
#include<iostream>
using namespace std;

class Tuple
{
public:
	char key;						// left
	string t;						// right

	Tuple() {}
	Tuple(char c, string st) :key(c), t(st) {}

	void print() { cout << "key: " << key << "\t" << "to: " << t; }
};

class Grammar
{
private:
	string Axiom;					// start string
	vector<Tuple> Generation;		// generation rules
	int Level;						// generation level
	string Result;					// result of generations

public:
	Grammar() {}					// default cotr
	Grammar(string axiom, vector<Tuple> generation, int level) :Axiom(axiom), Generation(generation), Level(level) {
		Result = Axiom;				// init result
		Generate();					// generate result
		cout << "Create Grammar success!" << endl;
		print();					// print information
	}

	void Generate();				// generate result
	void print();					// print information of Grammar

	string getResult() { return Result; }

	void defaultinit();
};

#endif // !_GRAMMAR_H

