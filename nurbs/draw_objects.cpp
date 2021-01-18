#include "draw_objects.h"
GLuint surfArray;
int shaderprogram;
int numberOfPtsU = 50; // specify the number of points to represent the U curve 50
int numberOfPtsV = 30;  // specify the number of points to represent the V curve 30
// for curve
vector<vector<Point3D>> controlPts;
vector<vector<double>>  knot_vec;
vector<int>             p_degree;
// for surface
vector<vector<Point3D>> controlNet;
vector<vector<Point4D>> ctrlNetw;
vector<double>          knotU;
vector<double>          knotV;
int                     p;
int                     q;

void genNURBSSurface(int p, vector<double> U, int q, vector<double> V, vector<vector<Point4D>> P)
{
	double TOL = 1e-6;
	// the row of P,    fixed u, v varies
	// the column of P, fixed v, u varies

	int n = P.size() - 1;
	int m = P[0].size() - 1;


	vector<vector<Point3D>> vCurves; // v-fixed curves
	vCurves.resize(numberOfPtsV);
	for (int i = 0; i < vCurves.size(); i++)
		vCurves[i].resize(numberOfPtsU);

	double intervalU = 1.0 / (numberOfPtsU - 1);
	double intervalV = 1.0 / (numberOfPtsV - 1);
	double u = 0;
	double v = 0;

	for (int i = 0; i < numberOfPtsV; i++)
	{
		// populate a v-isoparametric curve
		for (int j = 0; j < numberOfPtsU; j++)
		{
			vCurves[i][j] = getNURBSSufacePoint(n, p, U, m, q, V, P, u, v);
			u = u + intervalU;
			if (u > 1 && u - 1 < TOL) u = 1;
		}

		v = v + intervalV;
		if (v > 1 && v - 1 < TOL) v = 1;
		u = 0;
	}
	genNURBSBuffer(vCurves);
}
void drawNURBSSurface()
{

	glBindVertexArray(surfArray);
	//glUseProgram(shaderprogram);

	//glDrawElements(GL_TRIANGLES, 55 * 2 * 3, GL_UNSIGNED_INT, 0);
	glDrawElements(GL_TRIANGLES, (numberOfPtsV - 1) * (numberOfPtsU - 1) * 2 * 3, GL_UNSIGNED_INT, 0);
	//glClearColor(1, 0, 1, 1);
	//glPointSize(4);
	//glDrawArrays(GL_POINTS, 0, numberOfPtsV * numberOfPtsU);

	glBindVertexArray(0);

}


void genNURBSBuffer(vector<vector<Point3D>> surf)
{
	GLuint nurbsBuffer = 0;
	GLuint nurbsIndex = 0;
	// transfer two dimensional array into one dimension
	vector<Point3D> surfOneDim;
	vector<Index3> indices;
	if (surf.size() > 0) {
		cout << "success!!" << endl;
	}
	surfOneDim.resize(surf.size() * surf[0].size());
	for (int i = 0; i < surf.size(); i++) {
		for (int j = 0; j < surf[i].size(); j++) {
			surfOneDim[i * surf[i].size() + j] = surf[i][j] / 8.0;
		}
	}

#define TRANGLE
#ifdef TRANGLE
	indices.resize((surf.size() - 1) * (surf[0].size() - 1) * 2);
	int column, row, n;
	column = surf[0].size();
	row = surf.size();
	n = 0;
	cout << "row: " << row << ", column:" << column << endl;
	/*
	for (int j = 0; j < column - 1; j++) { // column
		for (int i = 0; i < row - 1; i++) { // row
			indices[i * (column - 1) * 2 + j * 2] = Index3(j + i * column, j + i * column + 1, j + (i + 1) * column);
			indices[i * (column - 1) * 2 + j * 2 + 1] = Index3(j + i * column + 1, j + (i + 1) * column, j + (i + 1) * column + 1);
		}
	}*/
	for (int i = 0; i < row - 1; i++) { // row
		for (int j = 0; j < column - 1; j++) { // column
			// ABCD -> ABD + BCD
			indices[n++] = Index3(i * column + j, i * column + j + 1, (i + 1) * column + j);
			indices[n++] = Index3(i * column + j + 1, (i + 1) * column + j, (i + 1) * column + j + 1);
		}
	}
#endif

#ifdef LINE
	indices.resize()
#endif
	/*
	for (int i = 0; i < indices.size(); i++) {
		cout << indices[i].getX() << "," << indices[i].getY() << "," << indices[i].getZ() << endl;
	}
	for (int i = 0; i < surfOneDim.size(); i++) {
		surfOneDim[i].print();
		if ((i + 1) % numberOfPtsU == 0) cout << endl;
	}
	*/


	glGenVertexArrays(1, &surfArray);
	glBindVertexArray(surfArray);

	glGenBuffers(1, &nurbsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, nurbsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(surfOneDim[0]) * surfOneDim.size(), &surfOneDim[0], GL_STATIC_DRAW);

	glGenBuffers(1, &nurbsIndex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nurbsIndex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), (void*)0);
	glEnableVertexAttribArray(0);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void init_nurbs()
{
	// curves tests begins
	controlPts.clear();
	knot_vec.clear();
	p_degree.clear();

	//runTestCases(1);
	runTestCases(4);
	runTestCases(5);
	//runTestCases(4);

	// move the section curve to origin
	Point3D translation(controlPts[0][0]);
	for (int i = 0; i < controlPts[0].size(); i++)
	{
		controlPts[0][i] = controlPts[0][i] - translation;
	}
	genCtrlNet();

	//genNURBSBuffer(controlNet);
	init_suf();
}

void genCtrlNet()
{
	SweptSurface sweptSurf;

#ifdef TRANS
	// 1. get the translational swept surface
	sweptSurf.getTranslationalSweep(p_degree[0], knot_vec[0], p_degree[1], knot_vec[1], controlPts[0], controlPts[1]);
#endif

#define TRANJ
#ifdef TRANJ
	// 2. get the trajectory interpolated surface
	BSplineCurve C(p_degree[0], controlPts[0].size() - 1, knot_vec[0], controlPts[0]);
	BSplineCurve T(p_degree[1], controlPts[1].size() - 1, knot_vec[1], controlPts[1]);
	vector3D Bv(1, 0, 0);
	Matrix33 identity = Matrix33::getIdentityMatrix();
	sweptSurf.getSweepSurface1(T, C, Bv, identity, 10);

#endif
	sweptSurf.outputSweptSur(p, q, knotU, knotV, controlNet);
}
void init_suf()
{
	//genCtrlNet();
	static bool first = true;
	//#define CIRCLE
#ifdef CIRCLE
	static const float circle_w = sqrt(2.0) / 2.0;
	static double points[16] = { 1, 0, 0, 1, 1, 1, 0, circle_w, 0, 1, 0, 1, 0, 0, 1, 1 };
	static double curve[16] = { 1, 0, 0, 1, 1, 0, 1, circle_w, 0, 0, 1, 1, 0, 1, 1, 1 };
	static double knots[7] = { 0, 0, 0, 0.5, 1, 1, 1 };
#endif
	// draw the swept surface
	// drawBSplineSurface(p,knotU,q,knotV,controlNet);
	// draw nurbs swept surface
	if (controlNet.size() > 0)
	{
		if (first) {

			srand(time(0));

#ifdef CIRCLE
			ctrlNetw.resize(4);
			for (int i = 0; i < ctrlNetw.size(); i++) {
				ctrlNetw[i].resize(4);
			}
			for (int i = 0; i < ctrlNetw.size(); i++) {
				for (int j = 0; j < ctrlNetw[i].size(); j++) {
					double x, y, z, w;
					x = curve[i * 4] * points[j * 4];
					y = curve[i * 4] * points[j * 4 + 1];
					z = curve[i * 4 + 2];
					w = curve[i * 4 + 3] * points[j * 4 + 3];
					ctrlNetw[i][j] = Point4D(x, y, z, w);
					ctrlNetw[i][j].scaleSelf();
				}
			}
			knotU.resize(7);
			knotV.resize(7);
			for (int i = 0; i < 7; i++) {
				knotU[i] = knots[i];
				knotV[i] = knots[i];
			}
			p = q = 3;
#else
			ctrlNetw.resize(controlNet.size());
			for (int i = 0; i < ctrlNetw.size(); i++) {
				ctrlNetw[i].resize(controlNet[i].size());
			}
			for (int i = 0; i < controlNet.size(); i++) {
				for (int j = 0; j < controlNet[i].size(); j++) {
					Point3D temp = controlNet[i][j];
					double w = rand() / (double)RAND_MAX;
					w = 0.5;
					ctrlNetw[i][j] = Point4D(temp.getX(), temp.getY(), temp.getZ(), w);
				}
			}
#endif
			first = false;
			cout << p << "," << q << endl;
			for (int i = 0; i < knotU.size(); i++) {
				cout << knotU[i] << ",";
			}
			cout << endl;
			for (int i = 0; i < knotV.size(); i++) {
				cout << knotV[i] << ",";
			}
			cout << endl;
			cout << ctrlNetw.size() << endl;
			cout << ctrlNetw[0].size() << endl;
			for (int i = 0; i < ctrlNetw.size(); i++) {
				for (int j = 0; j < ctrlNetw[i].size(); j++) {
					ctrlNetw[i][j].print();
				}
			}
		}

		genNURBSSurface(p, knotU, q, knotV, ctrlNetw);
	}
}