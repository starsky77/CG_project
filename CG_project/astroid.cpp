#include "astroid.h"
#include <GL\freeglut.h>
#include <GL\GLU.h>
#define _USE_MATH_DEFINES
#include <math.h>
astroid::astroid(float r,const float* axis,float omega,const float * c):r(r),rad(0),omega(omega){
    for(int i=0;i<3;i++){
		this->axis[i]=axis[i];
		color[i]=c[i];
    }
}
void astroid::draw(){
	glPushMatrix();
	glRotated(rad*180/M_PI,0,0,1);
    //glutSolidSphere(r,SLICES,STACKS);
	glutSolidCube(r);
	glPopMatrix();
    for(auto it=satelites.begin();it!=satelites.end();it++)
        it->draw();
}
void astroid::animate(){
	static const float dt= 0.002;
	rad+=omega*dt;
	for(auto it=satelites.begin();it!=satelites.end();it++){
		it->rad+=dt*it->omega;
		while(it->rad>M_PI*2)
			it->rad-=M_PI*2;
		it->mass.animate();
	}
}
obit::obit(float R, const float *normal, float omega,astroid &a):r(R),omega(omega),mass(a),rad(0){
  for (int i = 0; i < 3; i++) this->normal[i] = normal[i];
}
obit::obit(float r, float R,const float *normal, float omega, const float* color):
    r(R),omega(omega),mass(*new astroid(r,color)),rad(0){
    for(int i=0;i<3;i++)this->normal[i]=normal[i];
}
void obit::draw(){
	glPushMatrix();
    glRotated(acos(normal[2])*180.0/M_PI,normal[1],-normal[0],0);
	// draw orbit
	glutWireTorus(POINTSIZE,r,1,2*SLICES);
	glTranslated(r*cos(rad),r*sin(rad),0);
    glRotated(-acos(normal[2]) * 180.0 / M_PI, normal[1], -normal[0], 0);
	// inverse, reserve the direction
	float *n=mass.axis;
    glRotated(acos(n[2]) * 180.0 / M_PI, n[1], -n[0], 0);
    mass.draw();
	glPopMatrix();
}