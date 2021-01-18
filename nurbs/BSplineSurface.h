#include "BSplineCurve.h"

Point3D getSurfacePoint(int n, int p, vector<double> U, int m, int q, vector<double> V, vector<vector<Point3D>> P, double u, double v);

Point3D getNURBSSufacePoint(int n, int p, vector<double> U, int m, int q, vector<double> V, vector<vector<Point4D>> P, double u, double v);