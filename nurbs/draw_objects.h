#pragma once
#include <iostream>

#include <windows.h>

#include <glad/glad.h>

#include "GLFW/glfw3.h"

#include "Point.h"
#include "testcases.h"
#include "BSplineCurve.h"
#include "BSplineSurface.h"
#include "sweptSurface.h"
#include <vector>
#include <algorithm>
#include <time.h>

using namespace std;

extern int shaderprogram;
extern GLuint surfArray;

void genNURBSSurface(int p, vector<double> U, int q, vector<double> V, vector<vector<Point4D>> P);

void drawNURBSSurface();

void genNURBSBuffer(vector<vector<Point3D>> surf);

void init_suf();

void genCtrlNet();

void init_nurbs();
