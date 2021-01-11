#include"Parse.h"
#include"shader.h"
#include<string>
#include<iostream>
#define STB_IMAGE_IMPLEMENTATION
#include<stb_image.h>
using namespace std;

#define pi 3.14

void Parse::DrawLSystem()
{
	// init
	vector<float> inilocation = { 0.0,0.0,0.0 };
	vector<float> inidirection = { 0.0,1.0,0.0 };
	Tmp initmp;
	initmp.location = inilocation;	initmp.direction = inidirection; initmp.linewidth = width;
	mystack.push(initmp);					// initialize mystack

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

	float vertices[] = {
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
	
	Shader ourShader("shaders/4.1.texture.vs", "shaders/4.1.texture.fs");

	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// load and create a texture 
	// -------------------------
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char *data = stbi_load("containers.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	ourShader.use();

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