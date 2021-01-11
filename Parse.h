#ifndef _PARSE_H
#define _PARSE_H

#include "Grammar.h"

#include <string>
#include <stack>
#include <vector>
using namespace std;

struct Tmp {							// we use a class Tmp to store information in recursion
public:
	vector<float> location;			// current location x,y,z | default to (0,0,0)
	vector<float> direction;		// current grow direction | default to (0,1,0)
	float linewidth;				// trunk width
};

class Parse{
private:	
	string input;
	const vector<char> alpha = { 'F','f','+','-','\\','/','^','&','[',']' };

	stack<Tmp> mystack;

	float length;
	float width;
	float rotateX;
	float rotateY;
	float rotateZ;

public:
	Parse(){}
	Parse(string s, float l, float w, float x, float y, float z) :input(s),length(l),width(w),rotateX(x),rotateY(y),rotateZ(z) {}
	
private:
	void DrawLSystem();
	
	void drawLine();

	void RotateX(int check);
	void RotateY(int check);
	void RotateZ(int check);

	void push();
	void pop();

};

#endif // !_PARSE_H


