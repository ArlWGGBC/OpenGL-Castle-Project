//***************************************************************************
// GAME2012_ZingelKarl_FinalAssignment.cpp by Karl Zingel - 101339946
//
// GAME2012_ZingelKarl_FinalAssignment
//
// Description:
//	Click run to see the results.
//*****************************************************************************

////http://glew.sourceforge.net/
//The OpenGL Extension Wrangler Library (GLEW) is a cross-platform open-source C/C++ extension loading library. 
//GLEW provides efficient run-time mechanisms for determining which OpenGL extensions are supported on the target
//platform. OpenGL core and extension functionality is exposed in a single header file. GLEW has been tested on a 
//variety of operating systems, including Windows, Linux, Mac OS X, FreeBSD, Irix, and Solaris.
//
//http://freeglut.sourceforge.net/
//The OpenGL Utility Toolkit(GLUT) is a library of utilities for OpenGL programs, which primarily perform system - level I / O with the host operating system.
//Functions performed include window definition, window control, and monitoring of keyboardand mouse input.
//Routines for drawing a number of geometric primitives(both in solid and wireframe mode) are also provided, including cubes, spheresand the Utah teapot.
//GLUT also has some limited support for creating pop - up menus..

//OpenGL functions are in a single library named GL (or OpenGL in Windows). Function names begin with the letters glSomeFunction*();
//Shaders are written in the OpenGL Shading Language(GLSL)
//To interface with the window system and to get input from external devices into our programs, we need another library. For the XWindow System, this library is called GLX, for Windows, it is wgl,
//and for the Macintosh, it is agl. Rather than using a different library for each system,
//we use two readily available libraries, the OpenGL Extension Wrangler(GLEW) and the OpenGLUtilityToolkit(GLUT).
//GLEW removes operating system dependencies. GLUT provides the minimum functionality that should be expected in any modern window system.
//OpenGL makes heavy use of defined constants to increase code readability and avoid the use of magic numbers.Thus, strings such as GL_FILL and GL_POINTS are defined in header(#include <GL/glut.h>)

//https://glm.g-truc.net/0.9.9/index.html
////OpenGL Mathematics (GLM) is a header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications.
///////////////////////////////////////////////////////////////////////

using namespace std;

#include "stdlib.h"
#include "time.h"
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include "prepShader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <iostream>
#include "Shape.h"
#include "Light.h"

#define BUFFER_OFFSET(x)  ((const void*) (x))
#define FPS 60
#define MOVESPEED 0.5f
#define TURNSPEED 0.1f
#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)
#define XY_AXIS glm::vec3(1,0.9,0)
#define YZ_AXIS glm::vec3(0,1,1)
#define XZ_AXIS glm::vec3(1,0,1)
#define SPEED 3.f

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

enum keyMasks {
	KEY_FORWARD = 0b00000001,		// 0x01 or   1	or   01
	KEY_BACKWARD = 0b00000010,		// 0x02 or   2	or   02
	KEY_LEFT = 0b00000100,
	KEY_RIGHT = 0b00001000,
	KEY_UP = 0b00010000,
	KEY_DOWN = 0b00100000,
	KEY_MOUSECLICKED = 0b01000000

	// Any other keys you want to add.
};

static unsigned int
program,
vertexShaderId,
fragmentShaderId;

GLuint modelID, viewID, projID;
glm::mat4 View, Projection;

bool drawOnce = true;
// Our bitflag variable. 1 byte for up to 8 key states.
unsigned char keys = 0; // Initialized to 0 or 0b00000000.

// Texture variables.
GLuint blankID, firstID, secondID, hedgeTexture, wallTexture
, roomTexture, gateTexture, groundTexture, roofTexture, towerTexture, castlewallTexture1, castlewallTexture2, roadTexture, intTex;
GLint width, height, bitDepth;

// Light objects. Now OOP.
AmbientLight aLight(
	glm::vec3(1.0f, 1.0f, 1.0f),    // Diffuse colour.
	0.4f);// Diffuse strength.
//Directional light
DirectionalLight dLight(
	glm::vec3(1.0f, 0.0f, 0.0f),    // Origin.
	glm::vec3(1.0f, 0.0f, 0.0f),    // Diffuse colour.
	0.0f);                            // Diffuse strength.
//Point light
PointLight pLights(
	glm::vec3(25.7f, 6.5f, -25.75f),    // Position.
	15.0f,                            // Range.
	1.0f, 4.5f, 75.0f,                // Constant, Linear, Quadratic.
	glm::vec3(1.0f / 255.0f * 255.0f, 1.0f / 255.0f * 255.0f, 1.0f / 255.0f * 0.0f),    // Diffuse colour.
    20.0f);                    // Diffuse strength.

//Point light2
PointLight pLights2(
	glm::vec3(24.7f, 10.0f, -7.6f),    // Position.
	35.0f,                            // Range.
	1.0f, 4.5f, 75.0f,                // Constant, Linear, Quadratic.
	glm::vec3(1.0f / 255.0f * 255.0f, 1.0f / 255.0f * 0.0f, 1.0f / 255.0f * 200.0f),   // Diffuse colour.
	15.0f);                    // Diffuse strength.
//Point light3
PointLight pLights3(
	glm::vec3(24.3f, 10.0f, -36.4f),    // Position.
	20.0f,                            // Range.
	1.0f, 4.5f, 75.0f,                // Constant, Linear, Quadratic.
	glm::vec3(1.0f / 255.0f * 255.0f, 1.0f / 255.0f * 0.0f, 1.0f / 255.0f * 200.0f),    // Diffuse colour.
	15.0f);                    // Diffuse strength.
			
// Camera and transform variables.


// Camera and transform variables.
float scale = 1.0f, angle = 0.0f;
glm::vec3 position, frontVec, worldUp, upVec, rightVec; // Set by function
GLfloat pitch, yaw;
int lastX, lastY;

// Geometry data.
Grid g_grid(50);

Cube g_cube;
Prism g_prism(10);
Cone g_cone(18);
Prism g_c(30);
Cone g_d(8);
Plane g_plane;


struct Shapes
{
	Shape shape;

};
void timer(int); // Prototype.

void resetView()
{
	position = glm::vec3(10.0f, 5.0f, 25.0f); // Super pulled back because of grid size.
	frontVec = glm::vec3(0.0f, 0.0f, -1.0f);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	pitch = 0.0f;
	yaw = -90.0f;
	// View will now get set only in transformObject
}

void setupLights()
{
	
}



void loadTextures()
{


	// Image loading.
	stbi_set_flip_vertically_on_load(true);

	// Load first image.
	unsigned char* image = stbi_load("blank.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &blankID);
	glBindTexture(GL_TEXTURE_2D, blankID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End first image.
	
	// 1
	// roofTexture, towerTexture, castlewallTexture1, castlewallTexture2;
	image = stbi_load("tower_texture3.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &towerTexture);
	glBindTexture(GL_TEXTURE_2D, towerTexture);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End 1

	
	// 2
	// roofTexture, towerTexture, castlewallTexture1, castlewallTexture2;
	image = stbi_load("roof_texture.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &roofTexture);
	glBindTexture(GL_TEXTURE_2D, roofTexture);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End 2

	// 3
	// roofTexture, towerTexture, castlewallTexture1, castlewallTexture2;
	image = stbi_load("wall_tex.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &wallTexture);
	glBindTexture(GL_TEXTURE_2D, wallTexture);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End 3


	// Load third.
	image = stbi_load("dirt.png", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &firstID);
	glBindTexture(GL_TEXTURE_2D, firstID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End third

	// Load fourth image.
	image = stbi_load("chainmail.png", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &secondID);
	glBindTexture(GL_TEXTURE_2D, secondID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End fourth.

	// Load fifth image.
	image = stbi_load("waterfinal.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &groundTexture);
	glBindTexture(GL_TEXTURE_2D, groundTexture);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End fifth image.

	
	// Load sixth image.
	image = stbi_load("hedge.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &hedgeTexture);
	glBindTexture(GL_TEXTURE_2D, hedgeTexture);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End sixth image.
	//
	
	// Load sixth image.
	image = stbi_load("grasstex2.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &roadTexture);
	glBindTexture(GL_TEXTURE_2D, roadTexture);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End sixth image.
	//
	//
	// Load seventh image.
	image = stbi_load("gate.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &gateTexture);
	glBindTexture(GL_TEXTURE_2D, gateTexture);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End seventh image.

	// Load seventh image.
	image = stbi_load("darkstone.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &intTex);
	glBindTexture(GL_TEXTURE_2D, intTex);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End seventh image.
}


void init(void)
{
	srand((unsigned)time(NULL));
	// Create shader program executable.
	vertexShaderId = setShader((char*)"vertex", (char*)"cube.vert");
	fragmentShaderId = setShader((char*)"fragment", (char*)"cube.frag");
	program = glCreateProgram();
	glAttachShader(program, vertexShaderId);
	glAttachShader(program, fragmentShaderId);
	glLinkProgram(program);
	glUseProgram(program);

	modelID = glGetUniformLocation(program, "model");
	viewID = glGetUniformLocation(program, "view");
	projID = glGetUniformLocation(program, "projection");
	
	// Projection matrix : 45∞ Field of View, 1:1 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), 1.0f / 1.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	// Projection = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	resetView();

	
	loadTextures();
	

	glUniform1i(glGetUniformLocation(program, "texture0"), 0);

	// Setting ambient light.
	glUniform3f(glGetUniformLocation(program, "aLight.base.diffuseColour"), aLight.diffuseColour.x, aLight.diffuseColour.y, aLight.diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "aLight.base.diffuseStrength"), aLight.diffuseStrength);


	// Setting point lights.
	glUniform3f(glGetUniformLocation(program, "pLight.base.diffuseColour"), pLights.diffuseColour.x, pLights.diffuseColour.y, pLights.diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLight.base.diffuseStrength"), pLights.diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLight.position"), pLights.position.x, pLights.position.y, pLights.position.z);
	glUniform1f(glGetUniformLocation(program, "pLight.constant"), pLights.constant);
	glUniform1f(glGetUniformLocation(program, "pLight.linear"), pLights.linear);
	glUniform1f(glGetUniformLocation(program, "pLight.quadratic"), pLights.quadratic);

	glUniform3f(glGetUniformLocation(program, "pLight2.base.diffuseColour"), pLights2.diffuseColour.x, pLights2.diffuseColour.y, pLights2.diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLight2.base.diffuseStrength"), pLights2.diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLight2.position"), pLights2.position.x, pLights2.position.y, pLights2.position.z);
	glUniform1f(glGetUniformLocation(program, "pLight2.constant"), pLights2.constant);
	glUniform1f(glGetUniformLocation(program, "pLight2.linear"), pLights2.linear);
	glUniform1f(glGetUniformLocation(program, "pLight2.quadratic"), pLights2.quadratic);


	glUniform3f(glGetUniformLocation(program, "pLight3.base.diffuseColour"), pLights3.diffuseColour.x, pLights3.diffuseColour.y, pLights3.diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLight3.base.diffuseStrength"), pLights3.diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLight3.position"), pLights3.position.x, pLights3.position.y, pLights3.position.z);
	glUniform1f(glGetUniformLocation(program, "pLight3.constant"), pLights3.constant);
	glUniform1f(glGetUniformLocation(program, "pLight3.linear"), pLights3.linear);
	glUniform1f(glGetUniformLocation(program, "pLight3.quadratic"), pLights3.quadratic);




	// All VAO/VBO data now in Shape.h! But we still need to do this AFTER OpenGL is initialized.
	// Enable depth test and blend.
	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBlendEquation(GL_FUNC_ADD);

	// Enable smoothing.
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	// Enable face culling.
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	//glCullFace(GL_BACK);

	g_grid.BufferShape();

	g_cube.BufferShape();

	g_prism.BufferShape();

	g_cone.BufferShape();
	g_plane.BufferShape();
	g_d.BufferShape();
	g_c.BufferShape();


	timer(0); // Setup my recursive 'fixed' timestep/framerate.
}

//---------------------------------------------------------------------
//
// calculateView
//
void calculateView()
{
	frontVec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontVec.y = sin(glm::radians(pitch));
	frontVec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontVec = glm::normalize(frontVec);
	rightVec = glm::normalize(glm::cross(frontVec, worldUp));
	upVec = glm::normalize(glm::cross(rightVec, frontVec));

	View = glm::lookAt(
		position, // Camera position
		position + frontVec, // Look target
		upVec); // Up vector
}

//---------------------------------------------------------------------
//
// transformModel
//
void transformObject(glm::vec3 scale, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 translation) {
	glm::mat4 Model;
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, translation);
	Model = glm::rotate(Model, glm::radians(rotationAngle), rotationAxis);
	Model = glm::scale(Model, scale);
	
	// We must now update the View.
	calculateView();

	glUniformMatrix4fv(modelID, 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(projID, 1, GL_FALSE, &Projection[0][0]);
}


void createShape(Shape shape, GLuint texture, float iteration, float rotationangle, glm::vec3 scale, glm::vec3 position, float xsc, float ysc, float zsc)
{
	glBindTexture(GL_TEXTURE_2D, texture);

		for (int i = 0; i < iteration; i++)
		{
			
			shape.BufferShape();
			transformObject(scale, X_AXIS, rotationangle, position);
			shape.DrawShape(GL_TRIANGLES);
			position.x += xsc;
			position.y += ysc;
			position.z += zsc;
		}
}

void createMerlon(Shape shape, GLuint texture, float iteration, float rotationangle, glm::vec3 scale, glm::vec3 position, float xsc, float ysc, float zsc)
{
	glBindTexture(GL_TEXTURE_2D, texture);

		for (int i = 0; i < iteration; i++)
		{
			if(i % 2 == 0)
			{
			shape.BufferShape();
			transformObject(scale, X_AXIS, rotationangle, position);
			shape.DrawShape(GL_TRIANGLES);
			position.x += xsc;
			position.y += ysc;
			position.z += zsc;
			}
			else
			{
				
			shape.BufferShape();
			/*transformObject(scale, X_AXIS, rotationangle, position);*/
			transformObject(scale, X_AXIS, rotationangle, glm::vec3(position.x + xsc, position.y + ysc, position.z + zsc));
			shape.DrawShape(GL_TRIANGLES);
			
			}
		}
}	//-36.4205 X : 26.3699
bool isPickedUp = false;
bool isOnPlace = false;
glm::vec3 position1(23.5f, 5.0f, -36.9f); //top crystal
glm::vec3 position2 = glm::vec3(position1.x + 2.0f, position1.y, position1.z); //down of crystal
glm::vec3 position3(25.7f, 2.0f, -26.75f); //cyllinder pos
void drawShapes()
{

		//To connect, add scale to initial starting position - 1f

		//Iteration can be its own number not influenced by scales or position.

		//Scale and scale multiplier ( scale & xSc, ySc, zSc ) must be the same.
		glBindTexture(GL_TEXTURE_2D, firstID);


		//Exterior ground

		createShape(g_cube, roadTexture, 1, 0, glm::vec3(54, 1, -64), glm::vec3(-2.5, -1.0f, 2), 0, 0, 0);

		//Castle ground

		createShape(g_cube, intTex, 1, 0, glm::vec3(47, 2, -46), glm::vec3(1, -1.0f, -4), 0, 0, 0);

		//Interior ground

		createShape(g_cube, firstID, 1, 0, glm::vec3(42, 1, -40), glm::vec3(4, 1.005f, -7), 0, 0, 0);
		//Create Ground
		//Plane

		g_grid.RecolorShape(1.0f, 1.0f, 1.0f);
		createShape(g_grid, groundTexture, 1, 90, glm::vec3(1,1,1),glm::vec3(-0.5, 0.005, -52), 0,0,0);
		//Create Exterior Walls

		//Front walls
		createShape(g_cube, wallTexture, 3, 0, glm::vec3(6, 9, 1), glm::vec3(4, 1.0f, -7), 6, 0, 0);
		createShape(g_cube, wallTexture, 3, 0, glm::vec3(-6, 9, 1), glm::vec3(45, 1.0f, -7), -6, 0, 0);
		//Side Walls
		
		//right wall
		createShape(g_cube, wallTexture, 7, 0, glm::vec3(1, 9, 6), glm::vec3(45.5, 1.0f, -12), 0, 0, -6);
		//left wall
		createShape(g_cube, wallTexture, 7, 0, glm::vec3(1, 9, 6), glm::vec3(3, 1.0f, -12), 0, 0, -6);
		//Back Wall
		createShape(g_cube, wallTexture, 3, 0, glm::vec3(6.15, 9, 1), glm::vec3(4.5, 1.0f, -48), 6.15, 0, 0);
		createShape(g_cube, wallTexture, 3, 0, glm::vec3(-6.15, 9, 1), glm::vec3(45.5, 1.0f, -48), -6.15, 0, 0);
		/*createShape(g_cube, wallTexture, 21, 0, glm::vec3(2, 9, 1), glm::vec3(4, 1.0f, -48), 2, 0, 0);*/

		//Battlements
		//Front walls
		createShape(g_cube, intTex, 3, 0, glm::vec3(6, 1, 2), glm::vec3(4, 10.0f, -7.5), 6, 0, 0);
		createShape(g_cube, intTex, 3, 0, glm::vec3(-6, 1, 2), glm::vec3(45, 10.0f, -7.5), -6, 0, 0);
		//Side Walls
		//right wall
		createShape(g_cube, intTex, 7, 0, glm::vec3(2, 1, 6), glm::vec3(45, 10.0f, -12), 0, 0, -6);
		//left wall
		createShape(g_cube, intTex, 7, 0, glm::vec3(2, 1, 6), glm::vec3(2.5, 10.0f, -12), 0, 0, -6);
		//Back Walls
		createShape(g_cube, intTex, 7, 0, glm::vec3(6, 1, 2), glm::vec3(4, 10.0f, -48.5), 6, 0, 0);

		//Front Gate
		createShape(g_cube, gateTexture, 1, 0, glm::vec3(4, 8, 1), glm::vec3(23, 2.0f, -7),0,0,0 );
		//Back Gate
		createShape(g_cube, gateTexture, 1, 0, glm::vec3(4.15, 8, 1), glm::vec3(22.9, 2.0f, -48),0,0,0 );


		/////////////////////Towers/////////////////////////
		
		//Front Left and Front Right
		createShape(g_prism, towerTexture, 1, 0, glm::vec3(4, 12, 4), glm::vec3(1, 1.0f, -8),0,0,0	);
		createShape(g_prism, towerTexture	, 1, 0, glm::vec3(4, 12, 4), glm::vec3(44, 1.0f, -8),0,0,0);
		//Front Left Middle and Front Right Middle
		createShape(g_prism, towerTexture, 1, 0, glm::vec3(3, 12, 3), glm::vec3(26, 1.0f, -8),0,0,0);
		createShape(g_prism, towerTexture, 1, 0, glm::vec3(3, 12, 3), glm::vec3(20, 1.0f, -8),0,0,0);
		//Back Left and Back Right
		createShape(g_prism, towerTexture, 1, 0, glm::vec3(4, 12, 4), glm::vec3(1, 1.0f, -50),0,0,0);
		createShape(g_prism, towerTexture, 1, 0, glm::vec3(4, 12, 4), glm::vec3(44, 1.0f, -50),0,0,0);

		///////////////////Tower Tops//////////////////
		//Front Left
		createShape(g_cone, roofTexture, 1, 0,glm::vec3(6, 3, 6), glm::vec3(0, 13, -9),0,0,0 );
		//Front Right
		createShape(g_cone, roofTexture, 1, 0, glm::vec3(6, 3, 6), glm::vec3(43, 13, -9),0,0,0);
		//Front Left Middle and Front Right Middle
		createShape(g_cone, roofTexture, 1, 0,glm::vec3(5, 3, 5), glm::vec3(25, 13, -9),0,0,0 );
		createShape(g_cone, roofTexture, 1, 0, glm::vec3(5, 3, 5), glm::vec3(19, 13, -9),0,0,0);
		//Back Left
		createShape(g_cone, roofTexture, 1, 0,glm::vec3(6, 3, 6), glm::vec3(0, 13, -51),0,0,0 );
		//Back Right
		createShape(g_cone, roofTexture, 1, 0, glm::vec3(6, 3, 6), glm::vec3(43, 13, -51),0,0,0);
		
		//PARAPET (TODO)

		
		//Merlons/Crenels (TODO) : INCREASE MERLON/CRENEL HEIGHT TO PARAPET HEIGHT
		//Right Wall
		createMerlon(g_cube, wallTexture, 14, 0, glm::vec3(0.5, 1, 2), glm::vec3(46, 11.0f, -12),0,0,-4);
		//Left Wall
		createMerlon(g_cube, wallTexture, 14, 0, glm::vec3(0.5, 1, 2), glm::vec3(3, 11.0f, -12),0,0,-4);
		//Back wall
		createMerlon(g_cube, wallTexture, 16, 0, glm::vec3(2, 1, 0.5), glm::vec3(4, 11.0f, -48),4,0,0);
		//Front Wall
		//Left
		createMerlon(g_cube, wallTexture, 5, 0, glm::vec3(2, 1, 0.5), glm::vec3(6, 11.0f, -6.5),4,0,0);
		//Right
		createMerlon(g_cube, wallTexture, 5, 0, glm::vec3(2, 1, 0.5), glm::vec3(29, 11.0f, -6.5),4,0,0);

		

		//Steps
		//Front ramp
		createShape(g_cube, wallTexture, 1, 20, glm::vec3(4,0.25, 8), glm::vec3(22.5, 2.25f, -7),0,0,0 );
		//Back ramp
		createShape(g_cube, wallTexture, 1, -17.5, glm::vec3(4.15,0.25, 7), glm::vec3(23, 0.0f, -54),0,0,0 );
		//Inside steps
		createShape(g_cube, wallTexture, 1, 0, glm::vec3(4,0.5, -0.5), glm::vec3(22.5, 2.25f, -7),0,0,0 );
		createShape(g_cube, wallTexture, 1, 0, glm::vec3(4,0.5, -0.5), glm::vec3(22.5, 1.806f, -7.5),0,0,0 );
		createShape(g_cube, wallTexture, 1, 0, glm::vec3(4,0.5, -0.5), glm::vec3(22.5, 1.362f, -8),0,0,0 );
		createShape(g_cube, wallTexture, 1, 0, glm::vec3(4,0.5, -0.5), glm::vec3(22.5, 0.918f, -8.5),0,0,0 );

		///////////////////////////////////////MAZE/////////////////////////////////
		//6
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(1, 3.5, -16.5), glm::vec3(6, 1.f, -11),0,0,-16.5 );
		//3
		createShape(g_cube, hedgeTexture, 34, 0,glm::vec3(1, 3.5, 1), glm::vec3(43, 1.f, -11),0,0,-1 );
		//1
		createShape(g_cube, hedgeTexture, 16, 0,glm::vec3(1, 3.5, 1), glm::vec3(6, 1.f, -11),1,0,0 );
		//2
		createShape(g_cube, hedgeTexture, 16, 0,glm::vec3(1, 3.5, 1), glm::vec3(43, 1.f, -11),-1,0,0 );
		//5
		createShape(g_cube, hedgeTexture, 25, 0,glm::vec3(1, 3.5, 1), glm::vec3(6, 1.f, -44),1,0,0 );
		//4
		createShape(g_cube, hedgeTexture, 11, 0,glm::vec3(1, 3.5, 1), glm::vec3(43, 1.f, -44),-1,0,0 );
		//7
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(1, 3.5, -5.5), glm::vec3(9, 1.f, -14),0,0,-5.5 );
		//8
		createShape(g_cube, hedgeTexture, 32, 0,glm::vec3(1, 3.5, 1), glm::vec3(9, 1.f, -14),1,0,0 );
		//9
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(1, 3.5, -6), glm::vec3(40, 1.f, -14),0,0,-6 );
		//10
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(-3, 3.5, 1), glm::vec3(40, 1.f, -26),-3,0,0 );
		//15
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(1, 3.5, 5.5), glm::vec3(12, 1.f, -27),0,0,5.5 );
		//16
		//REDUNDANT/REMOVED
		//17
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(3, 3.5, 1), glm::vec3(6, 1.f, -28),3,0,0 );
		//18
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(1, 3.5, 5.5), glm::vec3(9, 1.f, -41),0,0,5.5 );
		//19
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(-10.5, 3.5, 1), glm::vec3(30, 1.f, -41),-10.5,0,0 );
		//20
		createShape(g_cube, hedgeTexture, 1, 0,glm::vec3(1, 3.5, -2.5), glm::vec3(30, 1.f, -41),0,0,-2.5 );
		//21
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(1, 3.5, -3.5), glm::vec3(27, 1.f, -34),0,0,-3.5 );
		//22
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(-6, 3.5, 1), glm::vec3(27, 1.f, -38),-6,0,0 );
		//23
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(1, 3.5, 11), glm::vec3(15, 1.f, -38),0,0,11 );
		//24
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(11, 3.5, 1), glm::vec3(16, 1.f, -17),11,0,0 );
		//25
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(1, 3.5, -3), glm::vec3(37, 1.f, -17),0,0,-3 );
		//addition
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(1, 3.5, -3), glm::vec3(34, 1.f, -17),0,0,-3 );
		//26
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(1, 3.5, 7.5), glm::vec3(18, 1.f, -35), 0, 0,7.5 );
		//addition1
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(7, 3.5, 1), glm::vec3(18, 1.f, -20), 7, 0,0);
		//27
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(-4.5, 3.5, 1), glm::vec3(27, 1.f, -35),-4.5,0,0 );
		//28
		createShape(g_cube, hedgeTexture, 1, 0,glm::vec3(-4, 3.5, 1), glm::vec3(34, 1.f, -35),0,0,0 );
		//29
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(1, 3.5, -3), glm::vec3(34, 1.f, -29),0,0,-3 );
		//30
		createShape(g_cube, hedgeTexture, 1, 0,glm::vec3(-3, 3.5, 1), glm::vec3(37, 1.f, -29),-3,0,0 );
		//31
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(1, 3.5, -6.5), glm::vec3(37, 1.f, -28),0,0,-6.5 );
		//32
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(-3.5, 3.5, 1), glm::vec3(37, 1.f, -38),-3.5,0,0 );
		//33
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(1, 3.5, -3), glm::vec3(40, 1.f, -28),0,0,-3 );
		//34
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(1, 3.5, -4), glm::vec3(40, 1.f, -36),0,0,-4 );
		//35
		createShape(g_cube, hedgeTexture, 1, 0,glm::vec3(1, 3.5, -2), glm::vec3(33, 1.f, -41),0,0,-2 );
		//36
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(-2, 3.5, 1), glm::vec3(37, 1.f, -41),-2,0,0 );
		//37
		//REDUNDANT/REMOVED
		//38
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(1, 3.5, 1), glm::vec3(42, 1.f, -29),-1,0,0 );
		//39
		createShape(g_cube, hedgeTexture, 2, 0,glm::vec3(1, 3.5, 5.5), glm::vec3(12, 1.f, -38),0,0,5.5 );
		//Create the special item
		if(!isPickedUp)
		{
			
			glBindTexture(GL_TEXTURE_2D, blankID);									   //
			transformObject(glm::vec3(2.0f, 2.0f, 2.0f), Z_AXIS, 0.0f, position1);		   //
			g_d.RecolorShape(1.0f / 255.0f * 255.0f, 1.0f / 255.0f * 0.0f, 1.0f / 255.0f * 200.0f);																			   //
			g_d.DrawShape(GL_TRIANGLES);												   //    drawing the special item 2 (2 cones)
																						   //
			glBindTexture(GL_TEXTURE_2D, blankID);									   //
			transformObject(glm::vec3(2.0f, 2.0f, 2.0f), Z_AXIS, -180.0f, position2);	   //
			g_d.DrawShape(GL_TRIANGLES);

		  
			if ((position1.x - position.x <= 2.0f && position1.x - position.x >= -2.0f &&			 //
				position.y - position1.y <= 4.0f && position.y - position1.y >= -4.0f &&			 //
				position.z - position1.z <= 2.0f && position.z - position1.z >= -2.0f) || 			 //   the condition for deleting the special item
				(position2.x - position.x <= 2.0f && position2.x - position.x >= -2.0f &&			 //
					position.y - position2.y <= 4.0f && position.y - position2.y >= -4.0f &&		 //
					position.z - position2.z <= 2.0f && position.z - position2.z >= -2.0f) && !isOnPlace)			 //
			{
					cout << "You picked up the object" << endl;										 //   delete the item when we set the "isPickedUp" to true
					isPickedUp = true;																 //
			}
		}
		//create cyllinder at centre 
		g_c.RecolorShape(1.0f / 255.0f * 133.0f, 1.0f / 255.0f * 100.0f, 1.0f / 255.0f * 255.0f);
		createShape(g_c, blankID, 1, 0, glm::vec3(0.7f, 1.5f, 0.7f), position3, 0, 0, 5.5);
}
	//Z : -48.1252 X : 24.8632
glm::vec3 doorPos(25, 5.0f, -48);
bool fullWayUp = false;
void checkTheDoor()
{
	if ((doorPos.x - position.x <= 3.0f && doorPos.x - position.x >= -3.0f &&			 //
		position.y - doorPos.y <= 3.0f && position.y - doorPos.y >= -3.0f &&			 //
		position.z - doorPos.z <= 3.0f && position.z - doorPos.z >= -3.0f))
	{
		if (!isOnPlace)
			cout << "You can't pass yet !! \n";
				else
		{
			cout << "You passing the door! \n";
		}
	}
}
void putTheCrystal()
{
	if ((position3.x - position.x <= 2.0f && position3.x - position.x >= -2.0f &&			 //
		position.y - position3.y <= 3.0f && position.y - position3.y >= -3.0f &&			 //
		position.z - position3.z <= 2.0f && position.z - position3.z >= -2.0f) && isPickedUp && !isOnPlace)
	{
		position1 = glm::vec3(position3.x-0.65f, position3.y + 3.5f, position3.z -0.675f);
		position2 = glm::vec3(position1.x+2.0f, position3.y + 3.5f, position1.z);
		if(!isOnPlace)
		cout << "You put the crystal on it's place, now you can pass the door! \n";
		isOnPlace = true;
		
	}
	if (isOnPlace)
	{
		glBindTexture(GL_TEXTURE_2D, blankID);									   //
		transformObject(glm::vec3(2.0f, 2.0f, 2.0f), Z_AXIS, 0.0f, position1);		   //
		g_d.RecolorShape(1.0f / 255.0f * 255.0f, 1.0f / 255.0f * 0.0f, 1.0f / 255.0f * 200.0f);																			   //
		g_d.DrawShape(GL_TRIANGLES);												   //    drawing the special item 2 (2 cones)
																					   //
		glBindTexture(GL_TEXTURE_2D, blankID);									   //
		transformObject(glm::vec3(2.0f, 2.0f, 2.0f), Z_AXIS, -180.0f, position2);	   //
		g_d.DrawShape(GL_TRIANGLES);
		if (position1.y <= (position3.y + 8.0f) && !fullWayUp)
		{
			position1.y += 0.05f;
			position2.y += 0.05f;
			if (position1.y <= (position3.y + 7.9f) && position1.y >= (position3.y + 7.0f))
			{
				fullWayUp = true;
			}
		}
		if(position1.y >= (position3.y + 3.6f)  && fullWayUp/*&& position1.y >= (position3.y + 4.0f) && fullWayUp*/)
		{
			position1.y -= 0.05f;
			position2.y -= 0.05f;
			if (position1.y <= (position3.y + 3.7f))
				fullWayUp = false;
		}
	}
}
//---------------------------------------------------------------------
//
// display
//
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// glBindTexture(GL_TEXTURE_2D, blankID); // Use this texture for all shapes.
	//cout << position1.y << "  Y3 :" << position3.y + 4.5f << endl;
	drawShapes();

	checkTheDoor();
	putTheCrystal();
	
	/*cout << "Z : " << position.z << " X : " << position.x << endl;*/

	glBindVertexArray(0); // Done writing.
	glutSwapBuffers(); // Now for a potentially smoother render.
}

void idle() // Not even called.
{
	glutPostRedisplay();
}

void parseKeys()
{
	if (keys & KEY_FORWARD)
		position += frontVec * MOVESPEED;
	if (keys & KEY_BACKWARD)
		position -= frontVec * MOVESPEED;
	if (keys & KEY_LEFT)
		position -= rightVec * MOVESPEED;
	if (keys & KEY_RIGHT)
		position += rightVec * MOVESPEED;
	if (keys & KEY_UP)
		position += upVec * MOVESPEED;
	if (keys & KEY_DOWN)
		position -= upVec * MOVESPEED;
}

void timer(int) { // Tick of the frame.
	// Get first timestamp
	int start = glutGet(GLUT_ELAPSED_TIME);
	// Update call
	parseKeys();
	// Display call
	glutPostRedisplay();
	// Calling next tick
	int end = glutGet(GLUT_ELAPSED_TIME);
	glutTimerFunc((1000 / FPS) - (end-start), timer, 0);
}

// Keyboard input processing routine.
// Keyboard input processing routine.
void keyDown(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 'w':
		if (!(keys & KEY_FORWARD))
			keys |= KEY_FORWARD; // keys = keys | KEY_FORWARD
		break;
	case 's':
		if (!(keys & KEY_BACKWARD))
			keys |= KEY_BACKWARD;
		break;
	case 'a':
		if (!(keys & KEY_LEFT))
			keys |= KEY_LEFT;
		break;
	case 'd':
		if (!(keys & KEY_RIGHT))
			keys |= KEY_RIGHT;
		break;
	case 'r':
		if (!(keys & KEY_UP))
			keys |= KEY_UP;
		break;
	case 'f':
		if (!(keys & KEY_DOWN))
			keys |= KEY_DOWN;
		break;
	default:
		break;
	}
}

void keyDownSpec(int key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case GLUT_KEY_UP: // Up arrow.
		if (!(keys & KEY_FORWARD))
			keys |= KEY_FORWARD; // keys = keys | KEY_FORWARD
		break;
	case GLUT_KEY_DOWN: // Down arrow.
		if (!(keys & KEY_BACKWARD))
			keys |= KEY_BACKWARD;
		break;
	default:
		break;
	}
}

void keyUp(unsigned char key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case 'w':
		keys &= ~KEY_FORWARD; // keys = keys & ~KEY_FORWARD. ~ is bitwise NOT.
		break;
	case 's':
		keys &= ~KEY_BACKWARD;
		break;
	case 'a':
		keys &= ~KEY_LEFT;
		break;
	case 'd':
		keys &= ~KEY_RIGHT;
		break;
	case 'r':
		keys &= ~KEY_UP;
		break;
	case 'f':
		keys &= ~KEY_DOWN;
		break;
	case ' ':
		resetView();
		break;
	default:
		break;
	}
}

void keyUpSpec(int key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case GLUT_KEY_UP:
		keys &= ~KEY_FORWARD; // keys = keys & ~KEY_FORWARD
		break;
	case GLUT_KEY_DOWN:
		keys &= ~KEY_BACKWARD;
		break;
	default:
		break;
	}
}

void mouseMove(int x, int y)
{
	if (keys & KEY_MOUSECLICKED)
	{
		pitch += (GLfloat)((y - lastY) * TURNSPEED);
		yaw -= (GLfloat)((x - lastX) * TURNSPEED);
		lastY = y;
		lastX = x;
	}
}

void mouseClick(int btn, int state, int x, int y)
{
	if (state == 0)
	{
		lastX = x;
		lastY = y;
		keys |= KEY_MOUSECLICKED; // Flip flag to true
		glutSetCursor(GLUT_CURSOR_NONE);
		//cout << "Mouse clicked." << endl;
	}
	else
	{
		keys &= ~KEY_MOUSECLICKED; // Reset flag to false
		glutSetCursor(GLUT_CURSOR_INHERIT);
		//cout << "Mouse released." << endl;
	}
}

//---------------------------------------------------------------------
//
// clean
//
void clean()
{
	cout << "Cleaning up!" << endl;
	glDeleteTextures(1, &firstID);
	glDeleteTextures(1, &secondID);
	glDeleteTextures(1, &blankID);
}

//---------------------------------------------------------------------
//
// main
//
int main(int argc, char** argv)
{
	//Before we can open a window, theremust be interaction between the windowing systemand OpenGL.In GLUT, this interaction is initiated by the following function call :
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
	glutSetOption(GLUT_MULTISAMPLE, 8);

	//if you comment out this line, a window is created with a default size
	glutInitWindowSize(1024, 1024);

	//the top-left corner of the display
	glutInitWindowPosition(0, 0);

	glutCreateWindow("Week 13 - 01_Blending");

	glewInit();	//Initializes the glew and prepares the drawing pipeline.

	init(); // Our own custom function.

	glutDisplayFunc(display);
	glutKeyboardFunc(keyDown);
	glutSpecialFunc(keyDownSpec);
	glutKeyboardUpFunc(keyUp);
	glutSpecialUpFunc(keyUpSpec);

	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove); // Requires click to register.

	atexit(clean); // This useful GLUT function calls specified function before exiting program. 
	glutMainLoop();

	return 0;
}