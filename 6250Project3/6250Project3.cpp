/*
*** FILE NAME   : animation_example.c
*** DESCRIPTION : This program is an example program using OpenGL for animation.
*** DATE        : May 2012
*** WRITTEN By  : JKL
*/


#include <stdio.h>               // standard C libraries
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <algorithm>
#include <time.h>
#include <string.h>
#include <vector>
#include <GLUT/glut.h>             // GLUT library
#include "cs_graphics_setup.h"   // Header for CS4250/5250/6250 courses

using namespace std;

//@@***********************************************************************************@@
// Constants
#define WINDOW_XS 200							// Window size
#define WINDOW_YS 200
#define WINDOW_NAME "Project 3 Ray Tracing - Gwen Wahl"	// Window name
#define RECURSION_LEVEL 3
#define ANI_MSEC 10	 // gap between frames

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

typedef struct pixel {
	pt point;
	color value;
};

typedef struct sphere {
	pt center;
	GLfloat radius;
	color base_color;
};

//@@***********************************************************************************@@
// Global Variables
vector<sphere> spheres;
light main_light;
vector<pixel> pixel_buffer;
//main_light.center.x = 0;
//main_light.

//@@***********************************************************************************@@
// Function prototypes
void display_func(void);
void keyboard_func(unsigned char c, int x, int y);
void animation_func(int val);

//void draw_sphere(sphere s);
//pt get_middle_point(pt v1, pt v2, sphere s);
void calculate_rays(pt eye, pt light);
sphere set_sphere(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLfloat red, GLfloat green, GLfloat blue);
pt set_point(GLfloat x, GLfloat y, GLfloat z);
color set_color(GLfloat r, GLfloat g, GLfloat b);
GLfloat dot_product(pt v1, pt v2);
GLfloat max_val(GLfloat x, GLfloat y);
pt get_unit_vector(pt vector);
//triangle set_triangle(pt one, pt two, pt three);
//pt get_triangle_midpoint(triangle t);
//vector< color > get_phong_intensities(sphere s, light l);
//vector < triangle > build_sphere(sphere s);

//@@***********************************************************************************@@
int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	init_setup(WINDOW_XS, WINDOW_YS, WINDOW_NAME);

	//Initialize Spheres
	spheres.push_back(set_sphere(-25, -25, 75, 15, 0.80, 0.00, 0.00));
	spheres.push_back(set_sphere(-25,  25, 75, 15, 0.22, 0.08, 0.69));
	spheres.push_back(set_sphere( 25,  25, 75, 15, 0.95, 0.83, 0.00));
	spheres.push_back(set_sphere( 25, -25, 75, 15, 0.00, 0.80, 0.00));
	spheres.push_back(set_sphere(-25, -25, 25, 24, 0.30, 0.20, 0.70));
	spheres.push_back(set_sphere(-25,  25, 25, 24, 1.00, 0.83, 0.00));
	spheres.push_back(set_sphere( 25,  25, 25, 24, 0.00, 0.80, 0.00));
	spheres.push_back(set_sphere( 25, -25, 25, 24, 0.90, 0.00, 0.00));

	//Initialize pixel buffer
	for (GLfloat x = -50.0; x <= 50.0; x += 0.1) {
		for (GLfloat y = -50.0; y <= 50.0; y += 0.1) {
			pixel p;
			p.point = set_point(x, y, 0);
			p.value = set_color(0.0, 0.0, 0.0);
			pixel_buffer.push_back(p);
		}
	}
	calculate_rays(set_point(0, 0, 10000), set_point(0, 0, 10010));

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

	glBegin(GL_POINTS);
	for (int i = 0; i < pixel_buffer.size(); ++i) {
		glColor3f(pixel_buffer[i].value.r, pixel_buffer[i].value.g, pixel_buffer[i].value.b);
		glVertex2f(pixel_buffer[i].point.x, pixel_buffer[i].point.y);
	}
	glEnd();

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
void calculate_rays(pt eye, pt light) {
	//For each pixel in the scene
	GLfloat dx, dy, dz, a, b, c, d, t, nearest_j, nearest_t, x, y, z;
	for (int i = 0; i < pixel_buffer.size(); ++i) {
		pt p = pixel_buffer[i].point;
		
		dx = p.x - eye.x;
		dy = p.y - eye.y;
		dz = p.z - eye.z;
		nearest_j =   -1;
		nearest_t =  999;

		//Check each sphere, find nearest intersect if any
		for (int j = 0; j < spheres.size(); ++j) {
			pt center = spheres[j].center;
			GLfloat r = spheres[j].radius;

			a = (dx * dx) + (dy * dy) + (dz * dz);
			b = 2 * dx * (eye.x - center.x) + 2 * dy * (eye.y - center.y) + 2 * dz * (eye.z - center.z);
			c = center.x*center.x + center.y*center.y + center.z*center.z + eye.x*eye.x + eye.y*eye.y + eye.z*eye.z + (-2*(center.x*eye.x + center.y*eye.y + center.z*eye.z)) - r*r;

			d = (b*b)-(4*a*c);
			
			t = (-b-sqrt(d))/(2*a);
			
			if (t >= 0 && t <= 1 && t < nearest_t) {
				nearest_t = t;
				nearest_j = j;
			}	
		}
		if (nearest_j >= 0) {
			//Color the pixel accordingly
			//LIGHTING WILL BE HERE
			pt center      = spheres[nearest_j].center;
			GLfloat r 	   = spheres[nearest_j].radius;
			//Find point on circle
			x = eye.x + (nearest_t * dx);
			y = eye.y + (nearest_t * dy);
			z = eye.z + (nearest_t * dz);

			pt normal_vector  = get_unit_vector(set_point((x - center.x) / r, (y - center.y) / r, (z - center.z) / r));
			pt light_vector   = get_unit_vector(set_point(light.x - x, light.y - y, light.z - z));
			pt eye_vector 	  = get_unit_vector(set_point(eye.x - x, eye.y - y, eye.z - z));
			pt reflect_vector = get_unit_vector(set_point((2 * dot_product(light_vector, normal_vector) * normal_vector.x) - light_vector.x,(2 * dot_product(light_vector, normal_vector) * normal_vector.y) - light_vector.y, (2 * dot_product(light_vector, normal_vector) * normal_vector.z) - light_vector.z ));

			GLfloat ambient   = 0.1;
			GLfloat diffuse   = 0.8 * max_val(dot_product(light_vector, normal_vector), 0);
			GLfloat specular  = 0.5 * pow(max_val(dot_product(reflect_vector, eye_vector), 0), 5);
			GLfloat intensity = ambient + diffuse;
			//Adding flat specular for white specularity
			//Set color
			pixel_buffer[i].value.r += (spheres[nearest_j].base_color.r * intensity) + specular;
			pixel_buffer[i].value.g += (spheres[nearest_j].base_color.g * intensity) + specular;
			pixel_buffer[i].value.b += (spheres[nearest_j].base_color.b * intensity) + specular;

		}
	}
}

GLfloat dot_product(pt v1, pt v2) {
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

GLfloat max_val(GLfloat x, GLfloat y) {
	if (x > y) {
		return x;
	} else {
		return y;
	}
}

pt get_unit_vector(pt v) {
	GLfloat m = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	return set_point(v.x / m, v.y / m, v.z / m);
}
/*vector<triangle> build_sphere(sphere s) {
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
} */
	
sphere set_sphere(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLfloat red, GLfloat green, GLfloat blue) {
	sphere s;
	s.center.x = x;
	s.center.y = y;
	s.center.z = z;
	s.radius = radius;
	s.base_color.r = red;
	s.base_color.g = green;
	s.base_color.b = blue;
	//s.triangles = build_sphere(s);
	//s.colors = get_phong_intensities(s, )
	return s;
}
pt set_point(GLfloat x, GLfloat y, GLfloat z) {
	pt p;
	p.x = x;
	p.y = y;
	p.z = z;
	return p;
}
color set_color(GLfloat r, GLfloat g, GLfloat b) {
	color c;
	c.r = r;
	c.g = g;
	c.b = b;
	return c;
}
/*
triangle set_triangle(pt one, pt two, pt three) {
	triangle t;
	t.one = one;
	t.two = two;
	t.three = three;
	return t;
} */
/*
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
} */