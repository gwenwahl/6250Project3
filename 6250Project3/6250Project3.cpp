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
#include <deque>
#include <time.h>
#include <unordered_map>
#include <string>
#include <limits>
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
//Lighting defines
#define AMBIENT 0.1
#define DIFFUSE 0.7
#define SPECULAR 0.5
#define SPECULAR_EXPONENT 3

//@@***********************************************************************************@@
// Structures
typedef struct pt {
	GLdouble x, y, z;
};

typedef struct triangle {
	pt one, two, three;
};

typedef struct color {
	GLdouble r, g, b;
};

typedef struct light {
	pt origin;
};

typedef struct pixel {
	pt point;
	color value;
};

typedef struct sphere {
	pt center;
	GLdouble radius;
	color base_color;
	GLdouble reflectance;
};
typedef struct ray {
	pt origin;
	pt direction;
	color intensitys;
	int total_reflections;
};

//@@***********************************************************************************@@
// Global Variables
vector<sphere> spheres;
light main_light;
unordered_map<string, pixel> pixel_buffer;
deque<ray>   ray_buffer;
pt eye_point;
//main_light.center.x = 0;
//main_light.

//@@***********************************************************************************@@
// Function prototypes
void display_func(void);
void keyboard_func(unsigned char c, int x, int y);
void animation_func(int val);

//void draw_sphere(sphere s);
//pt get_middle_point(pt v1, pt v2, sphere s);
void process_rays(void);
void reflect_ray(pt origin, pt reflect_vector, color value);
sphere set_sphere(GLdouble x, GLdouble y, GLdouble z, GLdouble radius, GLdouble red, GLdouble green, GLdouble blue, GLdouble reflectance);
pt set_point(GLdouble x, GLdouble y, GLdouble z);
color set_color(GLdouble r, GLdouble g, GLdouble b);
GLdouble dot_product(pt v1, pt v2);
GLdouble max_val(GLdouble x, GLdouble y);
pt get_unit_vector(pt vector);
GLdouble roundToTenths(GLdouble x);
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
	spheres.push_back(set_sphere(-25, -25, 75, 15, 0.80, 0.00, 0.00, 0.75));
	spheres.push_back(set_sphere(-25,  25, 75, 15, 0.22, 0.08, 0.69, 0.50));
	spheres.push_back(set_sphere( 25,  25, 75, 15, 0.95, 0.83, 0.00, 0.75));
	spheres.push_back(set_sphere( 25, -25, 75, 15, 0.00, 0.80, 0.00, 0.50));
	spheres.push_back(set_sphere(-25, -25, 25, 24, 0.30, 0.20, 0.70, 0.50));
	spheres.push_back(set_sphere(-25,  25, 25, 24, 1.00, 0.83, 0.00, 0.50));
	spheres.push_back(set_sphere( 25,  25, 25, 24, 0.00, 0.80, 0.00, 0.50));
	spheres.push_back(set_sphere( 25, -25, 25, 24, 0.90, 0.00, 0.00, 0.50));

	main_light.origin = set_point(0, 0, 10010);
	eye_point = set_point(0, 0, 10000);

	//Initialize pixel buffer
	for (GLdouble x = -50.0; x <= 50.0; x += 0.1) {
		for (GLdouble y = -50.0; y <= 50.0; y += 0.1) {
			pixel p;
			p.point = set_point(x, y, 0);
			p.value = set_color(0.0, 0.0, 0.0);

			pixel_buffer[to_string(x) + "," + to_string(y)] = p;
			//Fill initial ray buffer
			ray r;
			r.origin 	 = eye_point;
			r.direction  = set_point(p.point.x - eye_point.x, p.point.y - eye_point.y, p.point.z - eye_point.z);
			r.intensitys = set_color(1, 1, 1);
			r.total_reflections = 0;
			ray_buffer.push_back(r);
		}
	}
	process_rays();


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
	for (GLdouble x = -50.0; x <= 50.0; x += 0.1) {
		for (GLdouble y = -50.0; y <= 50.0; y += 0.1) {
			string key = to_string(x) + "," + to_string(y);
			glColor3f(pixel_buffer[key].value.r, pixel_buffer[key].value.g, pixel_buffer[key].value.b);
			glVertex2f(pixel_buffer[key].point.x, pixel_buffer[key].point.y);
		}
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
void process_rays(void) {
	GLdouble a, b, c, d, t, lowest_t;
	int lowest_i;
	sphere S;

	//While there are rays to process
	while(!ray_buffer.empty()) {
		//Get ray to process
		ray R = ray_buffer.front();
		ray_buffer.pop_front();
		
		//INTERSECT SECTION
		lowest_t = 9999999;
		lowest_i = -1;
		//See if ray intercepts any of the spheres
		for (int i = 0; i < spheres.size(); ++i) {
			S = spheres[i];

			a = dot_product(R.direction, R.direction);
			b = (2 * R.direction.x * (R.origin.x - S.center.x)) + (2 * R.direction.y * (R.origin.y - S.center.y)) + (2 * R.direction.z * (R.origin.z - S.center.z));
			c = dot_product(S.center, S.center) + dot_product(R.origin, R.origin) + (-2 * dot_product(S.center, R.origin)) - S.radius * S.radius;

			d =  (b * b) - (4 * a * c);
			if ( d > 0 ) {
				t = ((-1 * b) - sqrtf(d)) / (2 * a);
				if ( t < lowest_t) {
					lowest_t = t;
					lowest_i = i;
				}
			}
		}

		//LIGHTING SECTION
		if (lowest_i >= 0) {
			S = spheres[lowest_i];
			//Find point on circle
			pt intersection = set_point(R.origin.x + (lowest_t * R.direction.x), R.origin.y + (lowest_t * R.direction.y) ,R.origin.z + (lowest_t * R.direction.z));
			if (R.total_reflections == 0) {
				//Perform Phong shading
				pt normal_vector  = get_unit_vector(set_point((intersection.x - S.center.x) / S.radius, (intersection.y - S.center.y) / S.radius, (intersection.z - S.center.z) / S.radius));
				pt light_vector   = get_unit_vector(set_point(main_light.origin.x - intersection.x, main_light.origin.y - intersection.y, main_light.origin.z - intersection.z));
				pt eye_vector 	  = get_unit_vector(set_point(eye_point.x - intersection.x, eye_point.y - intersection.y, eye_point.z - intersection.z));
				pt reflect_vector = get_unit_vector(set_point((2 * dot_product(light_vector, normal_vector) * normal_vector.x) - light_vector.x,(2 * dot_product(light_vector, normal_vector) * normal_vector.y) - light_vector.y, (2 * dot_product(light_vector, normal_vector) * normal_vector.z) - light_vector.z ));

				GLdouble ambient   = AMBIENT;
				GLdouble diffuse   = DIFFUSE  * max_val(dot_product(light_vector, normal_vector), 0);
				GLdouble specular  = SPECULAR * pow(max_val(dot_product(reflect_vector, eye_vector), 0), SPECULAR_EXPONENT);
				GLdouble intensity = ambient + diffuse;
				//Adding flat specular for white specularity
				//Set color
				string key = to_string(roundToTenths(intersection.x)) + "," + to_string(roundToTenths(intersection.y));
				if (pixel_buffer[key].value.r > 0.0 || pixel_buffer[key].value.g > 0.0 || pixel_buffer[key].value.b > 0.0) {
					//Average colliding color values
					pixel_buffer[key].value.r = (((spheres[lowest_i].base_color.r * intensity) + specular) + pixel_buffer[key].value.r) / 2;
					pixel_buffer[key].value.g = (((spheres[lowest_i].base_color.g * intensity) + specular) + pixel_buffer[key].value.g) / 2;
					pixel_buffer[key].value.b = (((spheres[lowest_i].base_color.b * intensity) + specular) + pixel_buffer[key].value.b) / 2;
				} else {
					//Set initial value
					pixel_buffer[key].value.r += (spheres[lowest_i].base_color.r * intensity) + specular;
					pixel_buffer[key].value.g += (spheres[lowest_i].base_color.g * intensity) + specular;
					pixel_buffer[key].value.b += (spheres[lowest_i].base_color.b * intensity) + specular;
				}
			} else {
				//Add precalculated intensity and reflect
			}
			if (R.total_reflections < RECURSION_LEVEL) {
				//Reflect
			} 
		}
	}
}
/*
void calculate_rays(pt eye, pt light) {
	//For each pixel in the scene
	GLdouble dx, dy, dz, a, b, c, d, t, nearest_j, nearest_t, x, y, z;
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
			GLdouble r = spheres[j].radius;

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
			GLdouble r 	   = spheres[nearest_j].radius;
			//Find point on circle
			x = eye.x + (nearest_t * dx);
			y = eye.y + (nearest_t * dy);
			z = eye.z + (nearest_t * dz);

			pt normal_vector  = get_unit_vector(set_point((x - center.x) / r, (y - center.y) / r, (z - center.z) / r));
			pt light_vector   = get_unit_vector(set_point(light.x - x, light.y - y, light.z - z));
			pt eye_vector 	  = get_unit_vector(set_point(eye.x - x, eye.y - y, eye.z - z));
			pt reflect_vector = get_unit_vector(set_point((2 * dot_product(light_vector, normal_vector) * normal_vector.x) - light_vector.x,(2 * dot_product(light_vector, normal_vector) * normal_vector.y) - light_vector.y, (2 * dot_product(light_vector, normal_vector) * normal_vector.z) - light_vector.z ));

			GLdouble ambient   = 0.1;
			GLdouble diffuse   = 0.8 * max_val(dot_product(light_vector, normal_vector), 0);
			GLdouble specular  = 0.5 * pow(max_val(dot_product(reflect_vector, eye_vector), 0), 5);
			GLdouble intensity = ambient + diffuse;
			//Adding flat specular for white specularity
			//Set color
			pixel_buffer[i].value.r += (spheres[nearest_j].base_color.r * intensity) + specular;
			pixel_buffer[i].value.g += (spheres[nearest_j].base_color.g * intensity) + specular;
			pixel_buffer[i].value.b += (spheres[nearest_j].base_color.b * intensity) + specular;

			//Now reflect 
			//reflect_ray(set_point(x, y, z), reflect_vector, set_color(pixel_buffer[i].value.r * spheres[nearest_j].reflectance, pixel_buffer[i].value.g * spheres[nearest_j].reflectance, pixel_buffer[i].value.b * spheres[nearest_j].reflectance));
		}
	}
} */
	/*
void reflect_ray(pt origin, pt reflect_vector, color value) {
	GLdouble a, b, c, d, t, nearest_j, nearest_t, x, y, z;
	//Find nearest intersect if any of the reflect vector to a sphere
	for (int j = 0; j < spheres.size(); ++j) {
		pt center = spheres[j].center;
		GLdouble r = spheres[j].radius;

		a = (reflect_vector.x * reflect_vector.x) + (reflect_vector.y * reflect_vector.y) + (reflect_vector.z * reflect_vector.z);
		b = 2 * reflect_vector.x * (origin.x - center.x) + 2 * reflect_vector.y * (origin.y - center.y) + 2 * reflect_vector.z * (origin.z - center.z);
		c = center.x*center.x + center.y*center.y + center.z*center.z + origin.x*origin.x + origin.y*origin.y + origin.z*origin.z + (-2*(center.x*origin.x + center.y*origin.y + center.z*origin.z)) - r*r;
		d = (b*b)-(4*a*c);
			
		t = (-b-sqrt(d))/(2*a);
			
		if (t >= 0 && t <= 1 && t < nearest_t) {
			nearest_t = t;
			nearest_j = j;
		}	
	}
	if (nearest_j >= 0) {
		pt center      = spheres[nearest_j].center;
		GLdouble r 	   = spheres[nearest_j].radius;
		
		//Find point on circle
		x = roundToTenths(origin.x + (nearest_t * reflect_vector.x));
		y = roundToTenths(origin.y + (nearest_t * reflect_vector.y));
		z = roundToTenths(origin.z + (nearest_t * reflect_vector.z));

		for (int i = 0; i < pixel_buffer.size(); ++i) {
	
			if (pixel_buffer[i].point.x == x && pixel_buffer[i].point.y == y && pixel_buffer[i].point.z == z) {
				//For testing
				pixel_buffer[i].value = set_color(1, 1, 0);
			}
		}
	}
}
*/
GLdouble roundToTenths(GLdouble x) {
	return GLdouble(int(x * 10)) / 10;
}

GLdouble dot_product(pt v1, pt v2) {
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

GLdouble max_val(GLdouble x, GLdouble y) {
	if (x > y) {
		return x;
	} else {
		return y;
	}
}

pt get_unit_vector(pt v) {
	GLdouble m = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
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
		GLdouble length = sqrtf((vertices[i].x * vertices[i].x) + (vertices[i].y * vertices[i].y) + (vertices[i].z * vertices[i].z));
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
	
sphere set_sphere(GLdouble x, GLdouble y, GLdouble z, GLdouble radius, GLdouble red, GLdouble green, GLdouble blue, GLdouble reflectance) {
	sphere s;
	s.center.x = x;
	s.center.y = y;
	s.center.z = z;
	s.radius = radius;
	s.base_color.r = red;
	s.base_color.g = green;
	s.base_color.b = blue;
	s.reflectance = reflectance;
	//s.triangles = build_sphere(s);
	//s.colors = get_phong_intensities(s, )
	return s;
}
pt set_point(GLdouble x, GLdouble y, GLdouble z) {
	pt p;
	p.x = x;
	p.y = y;
	p.z = z;
	return p;
}
color set_color(GLdouble r, GLdouble g, GLdouble b) {
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
	GLdouble x = (v1.x + v2.x) / 2.0;
	GLdouble y = (v1.y + v2.y) / 2.0;
	GLdouble z = (v1.z + v2.z) / 2.0;

	GLdouble length = sqrtf((x * x) + (y * y) + (z * z));
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