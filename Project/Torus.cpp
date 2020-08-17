#include "glSetup.h"
#include <Windows.h>
#include "include/GLFW/glfw3.h"
#include "include/GL/gl.h"
#include "include/GL/glut.h"

#include "include/glm/glm.hpp"				// OpenGL Mathematics
#include "include/glm/gtc/type_ptr.hpp"		// glm::value_ptr()
#include "include/glm/gtx/string_cast.hpp"
using namespace glm;

#pragma comment(lib, "lib/glfw3.lib")
#pragma comment(lib, "lib/opengl32.lib")
#pragma comment(lib, "lib/glut32.lib")

#include <iostream>
using namespace std;

#include <fstream>

// Camera configuration
vec3 eye(3.5, 4.5, 3.5);
vec3 center(0, 0, 0);
vec3 up(0, 1, 0);

// Light configuration
vec4 light(1.0, 1.5, 0.5, 0);	// Light position

// Global coordinate frame
float AXIS_LENGTH = 3;
float AXIS_LINE_WIDTH = 1;

// Colors
GLfloat bgColor[4] = { 1, 1, 1, 1 };

// Selected example
int selection = 0;

vec3 p[36][18];			// Points
vec3 qcenter[36][18];	// Quads' center points
vec3 normal[36][18];	// Quads' normal vectors

void setupLight()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat ambient[4] = { 0.1, 0.1, 0.1, 1 };
	GLfloat diffuse[4] = { 1.0, 1.0, 1.0, 1 };
	GLfloat specular[4] = { 1.0, 1.0, 1.0, 1 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(light));
}

// Material
void setupMaterial()
{
	// Make it possible to change a subset of material parameters
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	// Material
	GLfloat mat_ambient[4] = { 0.1, 0.1, 0.1, 1 };
	GLfloat mat_specular[4] = { 0.5, 0.5, 0.5, 1 };
	GLfloat mat_shininess = 25;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

// Texture
void loadTexture(const char* filename)
{
	GLuint texture;
	unsigned char* data;
	FILE* file;
	
	file = fopen(filename, "rb"); // open file
	if (file == NULL) return;

	data = (unsigned char*)malloc(512 * 512 * 3);
	fread(data, 512 * 512 * 3, 1, file); // read file
	fclose(file); // close file
	
	glGenTextures(1, &texture);
	glBindTexture(1, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glEnable(GL_TEXTURE_2D);
	free(data); // free the texture
}

void init()
{
	// Prepare points p[36][18]
	float angle1 = 0.0f, angle2 = 0.0f;
	vec3 start(0.5f, 0.0f, 0.0f);
	vec3 axisy(0.0f, 1.0f, 0.0f);
	vec3 axisz(0.0f, 0.0f, 1.0f);
	mat4 TR = translate(mat4(1.0), vec3(1.0f, 1.0f, 0.0f));

	// Set p[j][i]
	for (int i = 0; i < 18; i++)
	{
		mat4 R1 = rotate(mat4(1.0), angle1, axisz);
		for (int j = 0; j < 36; j++)
		{
			mat4 R2 = rotate(mat4(1.0), angle2, axisy);
			vec4 V = R2 * TR * R1 * vec4(start, 1);
			p[j][i] = vec3(V.x, V.y, V.z);
			angle2 += (pi<float>() / 18.0f);
		}
		angle1 -= (pi<float>() / 9.0f);
	}

	// Texture
	loadTexture("Material/marble.raw");

	// Keyboard
	cout << "Set the s-t coordinates as in the figure" << endl;
	cout << "'1', '2', '3' keys to select textures" << endl;
}

void drawTorus() // Draw a Torus model
{
	glShadeModel(GL_SMOOTH);
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
	for (int i = 0; i < 18; i++)
	{
		for (int j = 0; j < 36; j++)
		{
			glTexCoord2f((float)j / 36, (float)i / 18); // s,t
			glNormal3fv(value_ptr(p[j][i]));
			glVertex3fv(value_ptr(p[j][i]));

			glTexCoord2f((float)j / 36, (float)(i + 1) / 18); // s,t
			glNormal3fv(value_ptr(p[j][(i + 1) % 18]));
			glVertex3fv(value_ptr(p[j][(i + 1) % 18]));

			glTexCoord2f((float)(j + 1) / 36, (float)(i + 1) / 18); // s,t
			glNormal3fv(value_ptr(p[(j + 1) % 36][(i + 1) % 18]));
			glVertex3fv(value_ptr(p[(j + 1) % 36][(i + 1) % 18]));

			glTexCoord2f((float)(j + 1) / 36, (float)i / 18); // s,t
			glNormal3fv(value_ptr(p[(j + 1) % 36][i]));
			glVertex3fv(value_ptr(p[(j + 1) % 36][i]));
		}
	}
	glEnd();
}

void render(GLFWwindow* window)
{
	// Background color
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Modelview matrix
	glMatrixMode(GL_MODELVIEW);;
	glLoadIdentity();

	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

	// Axes
	glDisable(GL_LIGHTING);
	drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH*dpiScaling);

	// Lighting
	setupLight();

	// Material
	setupMaterial();

	// Draw Torus
	glEnable(GL_TEXTURE_2D);
	drawTorus();
	glDisable(GL_TEXTURE_2D);
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
			// Quit
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;

			// 3 textures
		case GLFW_KEY_1: loadTexture("Material/marble.raw");	break;
		case GLFW_KEY_2: loadTexture("Material/wood.raw");	break;
		case GLFW_KEY_3: loadTexture("Material/check.raw");	break;
		}
	}
}

int main(int argc, char* argv[])
{
	// Initialize the OpenGL system
	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL)	return -1;

	// Callbacks
	glfwSetKeyCallback(window, keyboard);

	// Depth test
	glEnable(GL_DEPTH_TEST);

	// Normal vectors are normalized after transformation
	glEnable(GL_NORMALIZE);

	// Viewport and perspective setting
	reshape(window, windowW, windowH);

	// Initialization
	init();

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();		// Events
		render(window);			// Draw one frame
		glfwSwapBuffers(window);	// Swap buffers
	}

	// Terminate the glfw system
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}