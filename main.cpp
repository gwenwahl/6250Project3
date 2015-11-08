/*
*** FILE NAME   : animation_example.c
*** DESCRIPTION : This program is an example program using OpenGL for animation.
*** DATE        : May 2012
*** WRITTEN By  : JKL
*/

#include <stdio.h>               // standard C libraries
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <vector>
#include <GLUT/glut.h>             // GLUT library
#include "cs_graphics_setup.h"   // Header for CS4250/5250/6250 courses
using namespace std;

//@@***********************************************************************************@@
// Constants
#define WINDOW_XS 600							// Window size
#define WINDOW_YS 600
#define WINDOW_NAME "Project 3 Ray Tracing - Gwen Wahl"	// Window name

#define ANI_MSEC 10	 // gap between frames

//@@***********************************************************************************@@
// Structures
typedef struct pt {
   GLint x, y, z;
};

typedef struct sphere {
	pt center;
	GLfloat radius;
	GLfloat red;
	GLfloat green;
	GLfloat blue;
};

//@@***********************************************************************************@@
// Global Variables
MyPoint strtPt, endPt;
int go_up = 1;

//@@***********************************************************************************@@
// Function prototypes
void display_func(void);
void keyboard_func(unsigned char c, int x, int y);
void animation_func(int val);
vector<pt> build_sphere(sphere s);

//@@***********************************************************************************@@
int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	init_setup(WINDOW_XS, WINDOW_YS, WINDOW_NAME);

	glutDisplayFunc(display_func);
	glutKeyboardFunc(keyboard_func);
	glutTimerFunc(ANI_MSEC, animation_func, 0);
	glutMainLoop();

	return 1;

}	// end of main()

//@@***********************************************************************************@@
void display_func(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);   // background color (white)
	glClear(GL_COLOR_BUFFER_BIT);       // clearing the buffer not to keep the color
	

	glFlush();

	glutSwapBuffers();	// double buffering

}	// end of display_func()

//@@***********************************************************************************@@
void animation_func(int val)
{

	glutPostRedisplay();

	glutTimerFunc(ANI_MSEC, animation_func, 0);

}//end animation_func

//@@***********************************************************************************@@
void keyboard_func(unsigned char c, int x, int y)
{
	switch(c)
	{
		case 'Q' :
		case 'q' :	
			printf("Good Bye !\n");
			exit(0);				 // terminates the program
	}  // end of switch

}	// end of keyboard_func()
vector<pt> build_sphere(sphere s) {
	vector<pt> points;
	for (GLfloat u = 0; u < 2 * M_PI; u += 0.01) {
		for (GLfloat v = (-M_PI / 2); v < M_PI / 2; v += 0.01 ) {
			pt temp;
			temp.x = round( s.radius * cos(v) * cos(u)) + sphere.center.x;
			temp.y = round( s.radius * cos(v) * sin(u)) + sphere.center.y;
			temp.z = round( s.radius * sin(v)) + sphere.center.z;
			points.push_back(temp);
		}
	}
	return points;
}