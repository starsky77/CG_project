#pragma once
#include <iostream>

using namespace std;

class Point2D
{
private:
	double x;
	double y;

public:
	// constructer
	Point2D() { x = 0; y = 0; }
	Point2D(double x_, double y_) { x = x_; y = y_; }

	// getters
	double getX() { return x; }
	double getY() { return y; }

	//operator
	Point2D operator*(double n)  { return Point2D(x*n, y*n); }
	Point2D operator+(Point2D p) { return Point2D(x+p.x , y+p.y);	}
	Point2D operator-(Point2D p) { return Point2D(x-p.x, y-p.y); }
	void   scaleX(double s) { x = x/s; }
	void   scaleY(double s) { y = y/s; }

	void print() { cout << "(" << x << "," << y << ")" << endl; }
};

class Point3D
{
private:
	double x;
	double y;
	double z;

public:
	// constructer
	Point3D() { x = 0; y = 0; z = 0;}
	Point3D(double x_, double y_,double z_) { x = x_; y = y_; z= z_;}
	

	// getters
	double getX() const { return x; }
	double getY() const { return y; }
	double getZ() const { return z; }

	//operator
	Point3D operator*(double n)  { return Point3D(x*n, y*n, z*n); }
	Point3D operator/(double n) { return Point3D(x / n, y / n, z / n); }
	Point3D operator+(Point3D p) { return Point3D(x+p.x , y+p.y,z+p.z);	}
	Point3D operator-(Point3D p) { return Point3D(x-p.x, y-p.y,z-p.z); }
	//Point3D operator-(vector3D v) {return Point3D(x-v.x, y-v.y,z-v.z); }

	void   scaleX(double s) { x = x/s; }
	void   scaleY(double s) { y = y/s; }
	void   scaleZ(double s) { z = z/s; }

	void print() { cout << "(" << x << "," << y <<  "," << z << ")" << endl; }
};

class Index3
{
private:
	int x;
	int y;
	int z;

public:
	// constructer
	Index3() { x = 0; y = 0; z = 0; }
	Index3(int x_, int y_, int z_) { x = x_; y = y_; z = z_; }

	// getters
	double getX() const { return x; }
	double getY() const { return y; }
	double getZ() const { return z; }

	//operator
	Index3 operator*(int n) { return Index3(x * n, y * n, z * n); }
	Index3 operator+(Index3 p) { return Index3(x + p.x, y + p.y, z + p.z); }
	Index3 operator-(Index3 p) { return Index3(x - p.x, y - p.y, z - p.z); }

	void   scaleX(int s) { x = x / s; }
	void   scaleY(int s) { y = y / s; }
	void   scaleZ(int s) { z = z / s; }

	void print() { cout << "(" << x << "," << y << "," << z << ")" << endl; }
};

class Point4D
{
private:
	double x;
	double y;
	double z;
	double w;

public:
	// constructer
	Point4D() { x = 0; y = 0; z = 0; w = 0; }
	Point4D(double x_, double y_, double z_, double w_) { x = x_; y = y_; z = z_; w = w_; }

	// getters
	double getX() const { return x; }
	double getY() const { return y; }
	double getZ() const { return z; }
	double getW() const { return w; }

	void setX(double x) { this->x = x; }
	void setY(double y) { this->y = y; }
	void setZ(double z) { this->z = z; }
	void setW(double w) { this->w = w; }

	//operator
	Point4D operator*(double n) { return Point4D(x * n, y * n, z * n, w * n); }
	Point4D operator/(double n) { return Point4D(x / n, y / n, z / n, w / n); }
	Point4D operator+(Point4D p) { return Point4D(x + p.x, y + p.y, z + p.z, w + p.w); }
	Point4D operator-(Point4D p) { return Point4D(x - p.x, y - p.y, z - p.z, w - p.w); }
	

	void   scaleX(double s) { x = x / s; }
	void   scaleY(double s) { y = y / s; }
	void   scaleZ(double s) { z = z / s; }
	void   scaleW(double s) { w = w / s; }

	void   scaleSelf() { x = x * w; y = y * w; z = z * w; }

	void print() { cout << "(" << x << "," << y << "," << z << "," << w << ")" << endl; }
};