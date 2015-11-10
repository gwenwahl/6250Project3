/*
*** FILE NAME   : animation_example.c
*** DESCRIPTION : This program is an example program using OpenGL for animation.
*** DATE        : May 2012
*** WRITTEN By  : JKL
*/

#include "stdafx.h"
#include <stdio.h>               // standard C libraries
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <time.h>
#include <string.h>
#include <vector>
#include <GL/glut.h>             // GLUT library
#include "cs_graphics_setup.h"   // Header for CS4250/5250/6250 courses

using namespace std;

//@@***********************************************************************************@@
// Constants
#define WINDOW_XS 200							// Window size
#define WINDOW_YS 200
#define WINDOW_NAME "Project 3 Ray Tracing - Gwen Wahl"	// Window name
#define RECURSION_LEVEL 5
#define ANI_MSEC 10	 // gap between frames
GLfloat tau = (1.0 + sqrtf(5.0)) / 2.0;

//@@***********************************************************************************@@
// Structures
typedef struct pt {
	GLfloat x, y, z;
};

typedef struct triangle {
	pt one, two, three;
};

typedef struct color {
	GLfloat r, g, b;
};

typedef struct light {
	pt center;
	color ambient;
	color diffuse;
	color specular;
};

typedef struct sphere {
	pt center;
	GLfloat radius;
	color base_color;
	vector<triangle> triangles;
	vector<color> colors;
};

//@@***********************************************************************************@@
// Global Variables
sphere one, two, three, four, five, six, seven, eight;
light main_light;
//main_light.center.x = 0;
//main_light.

//@@***********************************************************************************@@
// Function prototypes
void display_func(void);
void keyboard_func(unsigned char c, int x, int y);
void animation_func(int val);
void draw_sphere(sphere s);
pt get_middle_point(pt v1, pt v2, sphere s);
sphere set_sphere(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLfloat red, GLfloat green, GLfloat blue);
pt set_point(GLfloat x, GLfloat y, GLfloat z);
triangle set_triangle(pt one, pt two, pt three);
pt get_triangle_midpoint(triangle t);
vector< color > get_phong_intensities(sphere s, light l);
vector < triangle > build_sphere(sphere s);

//@@***********************************************************************************@@
int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	init_setup(WINDOW_XS, WINDOW_YS, WINDOW_NAME);

	one = set_sphere(-25, -25, 25, 15, 0.8, 0, 0);
	two = set_sphere(-25, 25, 25, 15, 0.22, 0.08, 0.69);
	three = set_sphere(25, 25, 25, 15, 0.95, 0.83, 0);
	four = set_sphere(25, -25, 25, 15, 0.0, 0.8, 0.0);
	five = set_sphere(-25, -25, -25, 24, 0.3, 0.2, 0.7);
	six = set_sphere(-25, 25, -25, 24, 1.0, 0.83, 0.0);
	seven = set_sphere(25, 25, -25, 24, 0.0, 0.8, 0.0);
	eight = set_sphere(25, -25, -25, 24, 0.9, 0, 0);

	glutDisplayFunc(display_func);
	glutKeyboardFunc(keyboard_func);
	glutTimerFunc(ANI_MSEC, animation_func, 0);
	glutMainLoop();

	return 1;

}	// end of main()

	//@@***********************************************************************************@@
void display_func(void)
{
	glClearColor(0.0, 0.0, 0.0, 1.0);   // background color (black)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	draw_sphere(one);
	draw_sphere(two);
	draw_sphere(three);
	draw_sphere(four);
	draw_sphere(five);
	draw_sphere(six);
	draw_sphere(seven);
	draw_sphere(eight);

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
	switch (c)
	{
	case 'Q':
	case 'q':
		printf("Good Bye !\n");
		exit(0);				 // terminates the program
	}  // end of switch

}	// end of keyboard_func()
vector<triangle> build_sphere(sphere s) {
	//First create the 12 vertices of an icosahedron
	vector<pt> vertices;
	vector<triangle> triangles;
	vertices.push_back(set_point(-1,    tau, 0));
	vertices.push_back(set_point( 1,    tau, 0));
	vertices.push_back(set_point(-1, -1*tau, 0));
	vertices.push_back(set_point( 1, -1*tau, 0));

	vertices.push_back(set_point(0, -1,    tau));
	vertices.push_back(set_point(0,  1,    tau));
	vertices.push_back(set_point(0, -1, -1*tau));
	vertices.push_back(set_point(0,  1, -1*tau));

	vertices.push_back(set_point(   tau, 0, -1));
	vertices.push_back(set_point(   tau, 0,  1));
	vertices.push_back(set_point(-1*tau, 0, -1));
	vertices.push_back(set_point(-1*tau, 0,  1));

	//Confine vertices to circle
	for (int i = 0; i < vertices.size(); ++i) {
		GLfloat length = sqrtf((vertices[i].x * vertices[i].x) + (vertices[i].y * vertices[i].y) + (vertices[i].z * vertices[i].z));
		vertices[i].x = ((vertices[i].x / length) * s.radius);
		vertices[i].y = ((vertices[i].y / length) * s.radius);
		vertices[i].z = ((vertices[i].z / length) * s.radius);
	}

	//Now build the base 20 triangles for the icosahedron
	triangles.push_back(set_triangle(vertices[0], vertices[11], vertices[5]));
	triangles.push_back(set_triangle(vertices[0], vertices[5], vertices[1]));
	triangles.push_back(set_triangle(vertices[0], vertices[1], vertices[7]));
	triangles.push_back(set_triangle(vertices[0], vertices[7], vertices[10]));
	triangles.push_back(set_triangle(vertices[0], vertices[10], vertices[11]));

	triangles.push_back(set_triangle(vertices[1], vertices[5], vertices[9]));
	triangles.push_back(set_triangle(vertices[5], vertices[11], vertices[4]));
	triangles.push_back(set_triangle(vertices[11], vertices[10], vertices[2]));
	triangles.push_back(set_triangle(vertices[10], vertices[7], vertices[6]));
	triangles.push_back(set_triangle(vertices[7], vertices[1], vertices[8]));

	triangles.push_back(set_triangle(vertices[3], vertices[9], vertices[4]));
	triangles.push_back(set_triangle(vertices[3], vertices[4], vertices[2]));
	triangles.push_back(set_triangle(vertices[3], vertices[2], vertices[6]));
	triangles.push_back(set_triangle(vertices[3], vertices[6], vertices[8]));
	triangles.push_back(set_triangle(vertices[3], vertices[8], vertices[9]));

	triangles.push_back(set_triangle(vertices[4], vertices[9], vertices[5]));
	triangles.push_back(set_triangle(vertices[2], vertices[4], vertices[11]));
	triangles.push_back(set_triangle(vertices[6], vertices[2], vertices[10]));
	triangles.push_back(set_triangle(vertices[8], vertices[6], vertices[7]));
	triangles.push_back(set_triangle(vertices[9], vertices[8], vertices[1]));

	//Refine the triangles into a sphere
	for (int i = 0; i < RECURSION_LEVEL; ++i) {
		vector<triangle> refined;
		for (int j = 0; j < triangles.size(); ++j) {
			pt a = get_middle_point(triangles[j].one, triangles[j].two, s);
			pt b = get_middle_point(triangles[j].two, triangles[j].three, s);
			pt c = get_middle_point(triangles[j].three, triangles[j].one, s);

			//Add new faces
			refined.push_back(set_triangle(triangles[j].one, a, c));
			refined.push_back(set_triangle(triangles[j].two, b, a));
			refined.push_back(set_triangle(triangles[j].three, c, b));
			refined.push_back(set_triangle(a, b, c));
		}
		triangles = refined;
	}

	//Apply offset for center
	for (int k = 0; k < triangles.size(); ++k) {
		triangles[k].one.x += s.center.x;
		triangles[k].one.y += s.center.y;
		triangles[k].one.z += s.center.z;

		triangles[k].two.x += s.center.x;
		triangles[k].two.y += s.center.y;
		triangles[k].two.z += s.center.z;
	
		triangles[k].three.x += s.center.x;
		triangles[k].three.y += s.center.y;
		triangles[k].three.z += s.center.z;
	}
	return triangles;
}
sphere set_sphere(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLfloat red, GLfloat green, GLfloat blue) {
	sphere s;
	s.center.x = x;
	s.center.y = y;
	s.center.z = z;
	s.radius = radius;
	s.base_color.r = red;
	s.base_color.g = green;
	s.base_color.b = blue;
	s.triangles = build_sphere(s);
	//s.colors = get_phong_intensities(s, )
	return s;
}
void draw_sphere(sphere s) {
	glColor3f(s.base_color.r, s.base_color.g, s.base_color.b);
	for (int i = 0; i < s.triangles.size(); ++i) {
		glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(s.triangles[i].one.x, s.triangles[i].one.y, s.triangles[i].one.z);
		glVertex3f(s.triangles[i].two.x, s.triangles[i].two.y, s.triangles[i].two.z);
		glVertex3f(s.triangles[i].three.x, s.triangles[i].three.y, s.triangles[i].three.z);
		glEnd();
	}
}
pt set_point(GLfloat x, GLfloat y, GLfloat z) {
	pt p;
	p.x = x;
	p.y = y;
	p.z = z;
	return p;
}

triangle set_triangle(pt one, pt two, pt three) {
	triangle t;
	t.one = one;
	t.two = two;
	t.three = three;
	return t;
}

pt get_middle_point(pt v1, pt v2, sphere s) {
	pt middle;
	//Points must be on the circle
	GLfloat x = (v1.x + v2.x) / 2.0;
	GLfloat y = (v1.y + v2.y) / 2.0;
	GLfloat z = (v1.z + v2.z) / 2.0;

	GLfloat length = sqrtf((x * x) + (y * y) + (z * z));
	middle.x = (x / length) * s.radius;
	middle.y = (y / length) * s.radius;
	middle.z = (z / length) * s.radius;

	return middle;
}

pt get_triangle_midpoint(triangle t) {
	pt m;
	m.x = (t.one.x + t.two.x + t.three.x) / 3;
	m.y = (t.one.y + t.two.y + t.three.y) / 3;
	m.z = (t.one.z + t.two.z + t.three.z) / 3;
	return m;
}