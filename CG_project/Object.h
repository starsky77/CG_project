#pragma once

#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "model.h"

using namespace std;



typedef struct ObjTree
{
	string name;
	glm::mat4 model;
	//仅当isLeaf=true时，VAO以及VBO中的值才有意义
	bool isLeaf;
	unsigned int ObjVAO;
	unsigned int ObjVBO;
	//二叉树表示多叉树
	struct ObjTree* leftChild, * rightSibling;

}ObjTree;


ObjTree* CreatLeafnode(string name, unsigned int VAO, unsigned int VBO)
{
	ObjTree* node = new ObjTree;
	node->name = name;
	node->ObjVAO = VAO;
	node->ObjVBO = VBO;
	node->isLeaf = true;
	node->leftChild = NULL;
	node->rightSibling = NULL;
	//单位矩阵
	node->model = glm::mat4(1.0f);
	
}


ObjTree* UnionTree(string PName, ObjTree* T1, ObjTree* T2)
{
	if (!T1 || !T2)
	{
		cout << "Empty input node!" << endl;
		return NULL;
	}
	T1->rightSibling = T2;
	ObjTree* ParentNode = new ObjTree;
	ParentNode->name = PName;
	ParentNode->isLeaf = false;
	ParentNode->leftChild = T1;
	ParentNode->rightSibling = NULL;
	//无意义取值
	ParentNode->ObjVAO = 0;
	ParentNode->ObjVBO = 0;
	ParentNode->model = glm::mat4(1.0f);
	return ParentNode;
}


//绘制T为根的所有物体集合
void DrawObjCollection(ObjTree* T)
{

	if (T->leftChild != NULL)
	{
		DrawObjCollection(T->leftChild);
	}
	else if (T->rightSibling != NULL)
	{
		DrawObjCollection(T->rightSibling);
	}
	if (T->isLeaf)
	{
		DrawBaseElement();
	}
	return;
}

//更新T之下所有叶子节点的model矩阵
void UpdataModel(ObjTree* T, glm::mat4 newMat)
{
	if (T->leftChild != NULL)
	{
		UpdataModel(T->leftChild, T->model * newMat);
	}
	else if (T->rightSibling != NULL)
	{
		UpdataModel(T->rightSibling, T->model * newMat);
	}
	if (T->isLeaf)
	{
		T->model = newMat * T->model;
	}
	return;
}


//绘制基本图元
void DrawBaseElement()
{

}



