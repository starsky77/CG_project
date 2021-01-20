#include"Parse.h"

using namespace std;

vector<float*> vec;					// store all of the vertices

void Parse::DrawLSystem()
{
	this->Parseinit();

	for (int i = 0; i < input.length(); i++) {
		switch (input[i]) {
		case 'F': {
			drawLine();
			break;
		}
		case 'f': {
			drawLine();
			break;
		}
		case '+': {
			RotateX(1);
			break;
		}
		case '-': {
			RotateX(0);
			break;
		}
		case '\\': {
			RotateY(1);
			break;
		}
		case '/': {
			RotateY(0);
			break;
		}
		case '^': {
			RotateZ(1);
			break;
		}
		case '&': {
			RotateZ(0);
			break;
		}
		case '[': {
			push();
			break;
		}
		case ']': {
			pop();
			break;
		}
		default:{
			cout << "ERROR :: Unknown symbol: " << input[i];
			return;
		}
		}
	}
}

void Parse::drawLine()
{
	float nowx = mystack.top().location[0];
	float nowy = mystack.top().location[1];
	float nowz = mystack.top().location[2];
	float nextx = mystack.top().direction[0] * length;
	float nexty = mystack.top().direction[1] * length;
	float nextz = mystack.top().direction[2] * length;

	float line = mystack.top().linewidth;

	static float v[] = {
		// positions														// normals           // texture coords
		// back 
		nowx,					nowy,				nowz,					0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		line + nowx,			nowy,				nowz,					0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		line + nowx + nextx,	nowy + nexty,		nowz + nextz,			0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		line + nowx + nextx,	nowy + nexty,		nowz + nextz,			0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		nowx + nextx,			nowy + nexty,		nowz + nextz,			0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		nowx,					nowy,				nowz,					0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		// front
		nowx,					nowy,				line + nowz,			0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		line + nowx,			nowy,				line + nowz,			0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		line + nowx + nextx,	nowy + nexty,		line + nowz + nextz,	0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		line + nowx + nextx,	nowy + nexty,		line + nowz + nextz,	0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		nowx + nextx,			nowy + nexty,		line + nowz + nextz,	0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		nowx,					nowy,				line + nowz,			0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		// left
		nowx + nextx,			nowy + nexty,		line + nowz + nextz,	-1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		nowx + nextx,			nowy + nexty,		nowz + nextz,			-1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		nowx,					nowy,				nowz,					-1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		nowx,					nowy,				nowz,					-1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		nowx,					nowy,				line + nowz,			-1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		nowx + nextx,			nowy + nexty,		line + nowz + nextz,	-1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		// right
		line + nowx + nextx,	nowy + nexty,		line + nowz + nextz,	1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		line + nowx + nextx,	nowy + nexty,		nowz + nextz,			1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		line + nowx,			nowy,				nowz,					1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		line + nowx,			nowy,				nowz,					1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		line + nowx,			nowy,				line + nowz,			1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		line + nowx + nextx,	nowy + nexty,		line + nowz + nextz,	1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		// down
		nowx,					nowy,				nowz,					0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		line + nowx,			nowy,				nowz,					0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		line + nowx,			nowy,				line + nowz,			0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		line + nowx,			nowy,				line + nowz,			0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		nowx,					nowy,				line + nowz,			0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		nowx,					nowy,				nowz,					0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		// up
		nowx + nextx,			nowy + nexty,		nowz + nextz,			0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		line + nowx + nextx,	nowy + nexty,		nowz + nextz,			0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		line + nowx + nextx,	nowy + nexty,		line + nowz + nextz,	0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		line + nowx + nextx,	nowy + nexty,		line + nowz + nextz,	0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		nowx + nextx,			nowy + nexty,		line + nowz + nextz,	0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		nowx + nextx,			nowy + nexty,		nowz + nextz,			0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	float *vertices = new float[288];			// 288=8*6*6
	for (int i = 0; i < 288; i++) {
		vertices[i] = v[i];
	}
	vec.push_back(vertices);

	// reset mystack
	mystack.top().linewidth -= 0.1;
	mystack.top().location = { nowx + nextx,nowy + nexty,nowz + nextz };
}
void Parse::RotateX(int check)
{
	if(check==1)	mystack.top().direction[0] += 1;
	else mystack.top().direction[0] -= 1;
}
void Parse::RotateY(int check)
{
	if(mystack.top().direction[0]>0) mystack.top().direction[0] -= 1;
	else if(mystack.top().direction[0]<0) mystack.top().direction[0] += 1;

	if (mystack.top().direction[2] > 0) mystack.top().direction[2] -= 1;
	else if (mystack.top().direction[2] < 0) mystack.top().direction[2] += 1;
}
void Parse::RotateZ(int check)
{
	if (check == 1) mystack.top().direction[2] += 1;
	else mystack.top().direction[2] -= 1;
}
void Parse::push()
{
	mystack.push(mystack.top());
}
void Parse::pop()
{
	mystack.pop();
}
void Parse::Parseinit()
{
	// init stack
	vector<float> inilocation = { 0.0,0.0,0.0 };
	vector<float> inidirection = { 0.0,1.0,0.0 };
	Tmp initmp;
	initmp.location = inilocation;	initmp.direction = inidirection; initmp.linewidth = width;
	mystack.push(initmp);					// initialize mystack
}
void Parse::ParseDefault()
{
	Grammar a;
	a.defaultinit();
	this->input = a.getResult();
	this->length = 1;
	this->width = 0.3;
	this->rotateX = 1; this->rotateY = 1; this->rotateZ = 1;
}