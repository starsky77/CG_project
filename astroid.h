#pragma once 
#include <vector>
class obit;
static const float default_color[] = {1.0f, 1.0f, 1.0f},
                   default_axis[] = {0, 0, 1}, default_speed = 1,
                   POINTSIZE = 0.05, zaxis[] = {0, 0, 1};
static const int SLICES = 20, STACKS = 20;

class astroid{
    public:
    astroid(float r, const float * axis=default_axis,float omega=0 ,const float* color=default_color);
    std::vector<obit> satelites;
	void animate();
    void draw();
    private:
    float r,rad,axis[3],omega;
    float color[3];
    friend class obit;
};
class obit{
    public:
    obit(float r, float R, const float *normal, float omega=default_speed, const float* color=default_color);
	obit(float R,const float *normal=default_axis,float omega=default_speed,astroid &a=*new astroid(0.1));
    astroid& mass;
    private:
    void draw();
    float r,rad,omega;
    float normal[3];
    friend class astroid;
};