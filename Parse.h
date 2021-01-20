#ifndef _PARSE_H
#define _PARSE_H

#include "Grammar.h"
#include "shader.h"

#include <string>
#include <stack>
#include <vector>


struct Tmp {							// we use a class Tmp to store information in recursion
public:
	vector<float> location;				// current location x,y,z | default to (0,0,0)
	vector<float> direction;			// current grow direction | default to (0,1,0)
	float linewidth;					// trunk width
};

class Parse{
private:	
	string input;
	const vector<char> alpha = { 'F','f','+','-','\\','/','^','&','[',']' };

	stack<Tmp> mystack;

	float length;						// tree trunk length
	float width;						// tree trunk width
	float rotateX;						// rotate x degree
	float rotateY;						// rotate y degree
	float rotateZ;						// rotate z degree	

public:
	Parse(){}
	Parse(string s, float l, float w, float x, float y, float z) :input(s),length(l),width(w),rotateX(x),rotateY(y),rotateZ(z) {}
	


	void ParseDefault();				// create a default Parse test demo
	void DrawLSystem();					// draw L-system based tree

private:
	void drawLine();					// for draw the trunk in recursion

	void RotateX(int check);			// rotate x function
	void RotateY(int check);			// rotate y function
	void RotateZ(int check);			// rotate z function

	void push();						// push stack
	void pop();							// pop stack

	void Parseinit();					// initialization of stack and shader
};

#endif // !_PARSE_H


