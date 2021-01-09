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


enum ObjType
{
	MeshModel,selfMake
};

typedef struct ObjTree
{
	std::string name;
	glm::mat4 model;
	ObjType type;
	bool isLeaf;
	unsigned int ObjVAO;
	unsigned int ObjVBO;
	Model* mesh;
	//二叉树表示多叉树
	struct ObjTree* leftChild, * rightSibling;

	void (*Drawfp)();

}ObjTree;

//selfMake类原子物体
ObjTree* CreatLeafnode(std::string name, unsigned int VAO, unsigned int VBO, ObjType type,void (*Draw)())
{
	if (type != selfMake)
	{
		std::cout << "ERROR:Input doesn't follow the type!" << std::endl;
		return NULL;
	}
	ObjTree* node = new ObjTree;
	node->type = type;
	node->name = name;
	node->ObjVAO = VAO;
	node->ObjVBO = VBO;
	node->isLeaf = true;
	node->mesh = NULL;
	node->leftChild = NULL;
	node->rightSibling = NULL;
	//单位矩阵
	node->model = glm::mat4(1.0f);
	//绘制函数
	node->Drawfp = Draw;
	return node;
}

//mesh model类原子物体
ObjTree* CreatLeafnode(std::string name, Model* mesh, ObjType type, void (*Draw)())
{
	if (type != MeshModel)
	{
		std::cout << "ERROR:Input doesn't follow the type!" << std::endl;
		return NULL;
	}
	ObjTree* node = new ObjTree;
	node->name = name;
	node->type = type;
	node->isLeaf = true;
	node->leftChild = NULL;
	node->rightSibling = NULL;
	node->mesh = mesh;
	node->ObjVAO = 0;
	node->ObjVBO = 0;
	//单位矩阵
	node->model = glm::mat4(1.0f);
	//绘制函数
	node->Drawfp = Draw;
	return node;
}


ObjTree* UnionTree(std::string PName, ObjTree* T1, ObjTree* T2)
{
	if (!T1 || !T2)
	{
		std::cout << "Empty input node!" << std::endl;
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
		//绘制原子物体
		T->Drawfp();
	}
	return;
}

//更新T之下所有叶子节点的model矩阵
void UpdataModel(ObjTree* T, glm::mat4 newMat)
{
	if (T->leftChild != NULL)
	{
		UpdataModel(T->leftChild, T->model * newMat);
		if (T->rightSibling != NULL)
		{
			UpdataModel(T->rightSibling, T->model * newMat);
		}
	}
	if (T->isLeaf)
	{
		T->model = newMat * T->model;
	}
	return;
}

