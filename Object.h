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


// enum ObjType
// {
// 	MeshModel,selfMake
// };

struct ObjTree
{
	int alias;
	glm::mat4 model;
	char type;	// 's'elfMade, 'm'esh
	bool isLeaf;
	Model* mesh;
	//二叉树表示多叉树
	struct ObjTree* leftChild, * rightSibling;

	void (*Drawfp)(void);

};

//selfMake类原子物体
ObjTree* CreatLeafnode(int alias, char type,glm::mat4 &model,void (*Draw)(void))
{
	if (type != 's')
	{
		std::cout << "ERROR:Input doesn't follow the type!" << std::endl;
		return NULL;
	}
	ObjTree* node = new ObjTree;
	node->type = type;
	node->alias = alias;
	node->isLeaf = true;
	node->mesh = NULL;
	node->leftChild = NULL;
	node->rightSibling = NULL;
	//单位矩阵
	node->model = model;
	//绘制函数
	node->Drawfp = Draw;
	return node;
}

//mesh model类原子物体
ObjTree* CreatLeafnode(int alias, Model* mesh, char type, void (*Draw)(void))
{
	if (type != 'm')
	{
		std::cout << "ERROR:Input doesn't follow the type!" << std::endl;
		return NULL;
	}
	ObjTree* node = new ObjTree;
	node->alias = alias;
	node->type = type;
	node->isLeaf = true;
	node->leftChild = NULL;
	node->rightSibling = NULL;
	node->mesh = mesh;
	//单位矩阵
	node->model = glm::mat4(1.0f);
	//绘制函数
	node->Drawfp = Draw;
	return node;
}


ObjTree* UnionTree(int Palias, ObjTree* T1, ObjTree* T2)
{
	if (!T1 || !T2)
	{
		std::cout << "Empty input node!" << std::endl;
		return NULL;
	}
	T1->rightSibling = T2;
	ObjTree* ParentNode = new ObjTree;
	ParentNode->alias = Palias;
	ParentNode->isLeaf = false;
	ParentNode->leftChild = T1;
	ParentNode->rightSibling = NULL;
	//无意义取值
	ParentNode->model = glm::mat4(1.0f);
	return ParentNode;
}


//绘制T为根的所有物体集合
void DrawObjCollection(ObjTree* T,Shader &shader)
{

	if (T->leftChild != NULL)
	{
		DrawObjCollection(T->leftChild,shader);
	}
	else if (T->rightSibling != NULL)
	{
		DrawObjCollection(T->rightSibling,shader);
	}
	shader.setInt("alias",T->alias);
	shader.setMat4("model",T->model);
    T->Drawfp();
	return;
}

//更新T之下所有叶子节点的model矩阵
void UpdateModel(ObjTree* T, glm::mat4 newMat)
{
	if (T->leftChild != NULL)
	{
		UpdateModel(T->leftChild, T->model * newMat);
		if (T->rightSibling != NULL)
		{
			UpdateModel(T->rightSibling, T->model * newMat);
		}
	}
	if (T->isLeaf)
	{
		T->model = newMat * T->model;
	}
	return;
}

