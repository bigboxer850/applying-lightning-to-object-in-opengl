#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <math.h>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "OFFReader.c"
#include "file_utils.h"
#include "math_utils.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define DEG2RAD(d) (d * M_PI / 180)
/********************************************************************/
/*   Variables */
Polygo* p;
Vertex* v;
int i, j;
char file[] = "1duk.off";
OffModel* model = readOffFile(file);
const int noVerts = 94933;
const int noPolys = 189866;
Vector3f Vertices[noVerts];
GLfloat Normals[noVerts*3];
//Vector3f poly[noPolys];
const int k = 569598;



char theProgramTitle[] = "Task 3";
int theWindowWidth = 700, theWindowHeight = 700;
int theWindowPositionX = 40, theWindowPositionY = 40;
bool isFullScreen = false;
bool isAnimating = true;
float rotation = DEG2RAD(30);
GLuint VBO, VAO,cubeVAO,cubeVBO;
GLuint gWorldLocation;
GLint curr;
GLuint posID,pos2ID, colorID , normID;
GLuint elementbuffer,cubeelementbuffer;
const int index = 6;
unsigned int indeces[k];

/* Constants */
const int ANIMATION_DELAY = 20; /* milliseconds between rendering */
const char* pVSFileName = "shader.vs";
const char* pFSFileName = "shader.fs";
const char* pGSFileName = "shader.gs";

//char fd[] = "1dukk.off";

//OffModel* md = readOffFile(fd);
/********************************************************************
  Utility functions
 */

 /* post: compute frames per second and display in window's title bar */
void computeFPS() {
	static int frameCount = 0;
	static int lastFrameTime = 0;
	static char* title = NULL;
	int currentTime;

	if (!title)
		title = (char*)malloc((strlen(theProgramTitle) + 20) * sizeof(char));
	frameCount++;
	//currentTime = glutGet((GLenum)(GLUT_ELAPSED_TIME));
	currentTime = GetTickCount()*0.001;
	if (currentTime - lastFrameTime > 1.0f) {
		sprintf(title, "%s [ FPS: %4.4f ]",
			theProgramTitle,
			(currentTime - lastFrameTime)*1.0/ frameCount * 1.0 );
		glutSetWindowTitle(title);
		lastFrameTime = currentTime;
		frameCount = 0;
	}
}
float max_x = 0, max_y = 0, max_z = 0, min_x = 100000, min_y = 100000, min_z = 100000;

static void CreateVertexBuffer() {
	glGenVertexArrays(1, &VAO);
	cout << "VAO: " << VAO << endl;
	glBindVertexArray(VAO);
	//cout << model->vertices[0].x;
	int k = 0;

	for (int i = 0; i < noVerts; i++) {
		Vertices[i] = Vector3f(0.02*(model->vertices[i]).x, 0.02*(model->vertices[i]).y, 0.02*(model->vertices[i]).z);
		max_x = max(max_x, (model->vertices[i]).x);
		max_y = max(max_y, (model->vertices[i]).y);
		max_z = max(max_z, (model->vertices[i]).z);
		min_x = min(min_x, (model->vertices[i]).x);
		min_y = min(min_y, (model->vertices[i]).y);
		min_z = min(min_z, (model->vertices[i]).z);
		Normals[k] = 0.0f; k++;
		Normals[k] = 0.0f; k++;
		Normals[k] = 0.0f; k++;
	}
	max_x = max_x *0.02;
	max_y = max_y * 0.02;
	max_z = max_z * 0.02;
	min_x = min_x * 0.02;
	min_y = min_y * 0.02;
	min_z = min_z * 0.02;

	int p = 0;
	k = 0;
	for (i = 0; i < noPolys; i++) {
		//printf("%d ", (model->polygons[i]).noSides);
		
		indeces[p] = (model->polygons[i]).v[0]; p++;
		indeces[p] = (model->polygons[i]).v[1]; p++;
		indeces[p] = (model->polygons[i]).v[2]; p++; k = k + 3;
		Vector3f U = Vector3f(Vertices[indeces[p-2]][0] -Vertices[indeces[p-3]][0], Vertices[indeces[p - 2]][1] - Vertices[indeces[p - 3]][1], Vertices[indeces[p - 2]][2] - Vertices[indeces[p - 3]][2]);
		Vector3f V = Vector3f(Vertices[indeces[p-1]][0] - Vertices[indeces[p-3]][0], Vertices[indeces[p-1]][1] - Vertices[indeces[p - 3]][1], Vertices[indeces[p-1]][2] - Vertices[indeces[p - 3]][2]);

		//Vector3f U = Vector3f(0.0, 0.0, 0.0);
		//Vector3f V = Vector3f(0.0, 0.0, 0.0);

		Normals[indeces[k - 3]*3] = (Normals[indeces[k - 3]*3] + (U[1] * V[2] - U[2] * V[1]));
		Normals[indeces[k - 3] * 3 + 1] = (Normals[indeces[k - 3] * 3 + 1] + (U[2] * V[0] - U[0] * V[2]));
		Normals[indeces[k - 3]*3+2] = (Normals[indeces[k - 3]*3+2]+ (U[0] * V[1] - U[1] * V[0]));


		Normals[indeces[k - 2] * 3] = (Normals[indeces[k - 2]] + (U[1] * V[2] - U[2] * V[1]));
		Normals[indeces[k - 2]*3+1] = (Normals[indeces[k - 2]] + (U[2] * V[0] - U[0] * V[2]));
		Normals[indeces[k - 2]*3+2] = (Normals[indeces[k - 2]] + (U[0] * V[1] - U[1] * V[0]));


		Normals[indeces[k -1] * 3] = (Normals[indeces[k -1]] + (U[1] * V[2] - U[2] * V[1]));
		Normals[indeces[k -1] * 3 + 1] = (Normals[indeces[k -1]] + (U[2] * V[0] - U[0] * V[2])) ;
		Normals[indeces[k -1] * 3 + 2] = (Normals[indeces[k -1]] + (U[0] * V[1] - U[1] * V[0])) ;

	}
	cout << Normals[0] << " " << Normals[1] << " " << Normals[2] << " ";

	/*Vector3f Vertices[4];
	Vertices[0] = Vector3f(-1.0f, -1.0f, 1.0f);
	Vertices[1] = Vector3f(1.0f, -1.0f, 1.0f);
	Vertices[2] = Vector3f(1.0f, 1.0f, 1.0f);
	Vertices[3] = Vector3f(-1.0f, 1.0f, 1.0f);

	*/

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices) + sizeof(Normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices), sizeof(Normals), Normals);


	glEnableVertexAttribArray(posID);
	glVertexAttribPointer(posID, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(normID);
	glVertexAttribPointer(normID, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET( sizeof(Vertices)));


	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, k * sizeof(unsigned int), indeces, GL_STATIC_DRAW);

	


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &cubeVAO);
	cout << "VAO: " << VAO << endl;
	glBindVertexArray(cubeVAO);



	GLfloat lightVertices[] =
	{ //     COORDINATES     //
		-0.05f, -0.05f,  0.05f,
		-0.05f, -0.05f, -0.05f,
		 0.05f, -0.05f, -0.05f,
		 0.05f, -0.05f,  0.05f,
		-0.05f,  0.05f,  0.05f,
		-0.05f,  0.05f, -0.05f,
		 0.05f,  0.05f, -0.05f,
		 0.05f,  0.05f,  0.05f
	};

	GLuint lightIndices[] =
	{
		0, 1, 2,
		0, 2, 3,
		0, 4, 7,
		0, 7, 3,
		3, 7, 6,
		3, 6, 2,
		2, 6, 5,
		2, 5, 1,
		1, 5, 4,
		1, 4, 0,
		4, 5, 6,
		4, 6, 7
	};


	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), lightVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(pos2ID);
	glVertexAttribPointer(pos2ID, 3, GL_FLOAT, GL_FALSE, 0, 0);


	glGenBuffers(1, &cubeelementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeelementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned int), lightIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType) {
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	glShaderSource(ShaderObj, 1, p, Lengths);
	glCompileShader(ShaderObj);
	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		exit(1);
	}

	glAttachShader(ShaderProgram, ShaderObj);
}

using namespace std;

static void CompileShaders() {
	GLuint ShaderProgram = glCreateProgram();

	if (ShaderProgram == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	string vs, fs, gs;

	if (!ReadFile(pVSFileName, vs)) {
		exit(1);
	}

	if (!ReadFile(pFSFileName, fs)) {
		exit(1);
	}

	

	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);
	//AddShader(ShaderProgram, gs.c_str(), GL_GEOMETRY_SHADER);
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}
	glBindVertexArray(VAO);
	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program1: '%s'\n", ErrorLog);
		exit(1);
	}

	glUseProgram(ShaderProgram);
	gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
	posID = glGetAttribLocation(ShaderProgram, "Position");
	pos2ID= glGetAttribLocation(ShaderProgram, "Pos");
	curr= glGetUniformLocation(ShaderProgram, "curr");
	normID = glGetAttribLocation(ShaderProgram, "normID");
}

/********************************************************************
 Callback Functions
 These functions are registered with the glut window and called
 when certain events occur.
 */

void onInit(int argc, char* argv[])
/* pre:  glut window has been initialized
   post: model has been initialized */ {
   /* by default the back ground color is black */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	CompileShaders();
	CreateVertexBuffer();
	

	/* set to draw in window based on depth  */
	glEnable(GL_DEPTH_TEST);
}

static void onDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPointSize(5);
	
	Matrix4f World;

	World.m[0][0] = 1.0f; World.m[0][1] = 0.0f; World.m[0][2] = 0.0f; World.m[0][3] = -((max_x + min_x) / 2) ;
	World.m[1][0] = 0.0f; World.m[1][1] = 1.0f; World.m[1][2] = 0.0f;  World.m[1][3] = -((max_y + min_y) / 2);
	World.m[2][0] = 0.0f; World.m[2][1] = 0.0f; World.m[2][2] = 1.0f; World.m[2][3] = -((max_z + min_z) / 2) ;
	World.m[3][0] = 0.0f; World.m[3][1] = 0.0f; World.m[3][2] = 0.0f; World.m[3][3] = 1.0f;


	
	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &World.m[0][0]);
	glUniform1i(curr,1);
	//glEnableVertexAttribArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(VAO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawElements(
		GL_TRIANGLES,      // mode
		k,    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glDisableVertexAttribArray(0);

	glBindVertexArray(cubeVAO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	World.m[0][0] = 1.0f; World.m[0][1] = 0.0f; World.m[0][2] = 0.0f; World.m[0][3] = 0.9f;
	World.m[1][0] = 0.0f; World.m[1][1] = cosf(rotation);  World.m[1][2] = sinf(rotation); World.m[1][3] = 0.0f;
	World.m[2][0] = 0.0f;        World.m[2][1] = -sinf(rotation);   World.m[2][2] = cosf(rotation);  World.m[2][3] = 0.0f;
	World.m[3][0] = 0.0f;        World.m[3][1] = 0.0f;         World.m[3][2] = 0.3f; World.m[3][3] = 1.0f;
	Matrix4f R;

	R.m[0][0] = cosf(rotation); R.m[0][1] = -sinf(rotation); R.m[0][2] = 0.0f; R.m[0][3] = 0.0f;
	R.m[1][0] = sinf(rotation); R.m[1][1] = cosf(rotation);  R.m[1][2] = 0.0f; R.m[1][3] = 0.0f;
	R.m[2][0] = 0.0f;        R.m[2][1] = 0.0f;   R.m[2][2] = 1.0f; R.m[2][3] = 0.0f;
	R.m[3][0] = 0.0f;        R.m[3][1] = 0.0f;        R.m[3][2] = 0.0f; R.m[3][3] = 1.0f;

	World = R * World;


	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &World.m[0][0]);
	glUniform1i(curr, 2);
	glDrawElements(
		GL_TRIANGLES,      // mode
		36,    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);





	/* check for any errors when rendering */
	GLenum errorCode = glGetError();
	if (errorCode == GL_NO_ERROR) {
		/* double-buffering - swap the back and front buffers */
		glutSwapBuffers();
	}
	else {
		fprintf(stderr, "OpenGL rendering error %d\n", errorCode);
	}
}

/* pre:  glut window has been resized
 */
static void onReshape(int width, int height) {
	glViewport(0, 0, width, height);
	//glOrtho(0.01, 0.02, 0.0, .5, 0.0, .5);
	if (!isFullScreen) {
		theWindowWidth = width;
		theWindowHeight = height;
	}
	// update scene based on new aspect ratio....
}

/* pre:  glut window is not doing anything else
   post: scene is updated and re-rendered if necessary */
static void onIdle() {
	static int oldTime = 0;
	if (isAnimating) {
		int currentTime = glutGet((GLenum)(GLUT_ELAPSED_TIME));
		/* Ensures fairly constant framerate */
		if (currentTime - oldTime > ANIMATION_DELAY) {
			// do animation....
			rotation += 0.01;

			oldTime = currentTime;
			/* compute the frame rate */
			computeFPS();
			/* notify window it has to be repainted */
			glutPostRedisplay();
		}
	}
}

/* pre:  mouse is dragged (i.e., moved while button is pressed) within glut window
   post: scene is updated and re-rendered  */
static void onMouseMotion(int x, int y) {
	/* notify window that it has to be re-rendered */
	glutPostRedisplay();
}

/* pre:  mouse button has been pressed while within glut window
   post: scene is updated and re-rendered */
static void onMouseButtonPress(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// Left button pressed
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		// Left button un pressed
	}
	/* notify window that it has to be re-rendered */
	glutPostRedisplay();
}

/* pre:  key has been pressed
   post: scene is updated and re-rendered */
static void onAlphaNumericKeyPress(unsigned char key, int x, int y) {
	switch (key) {
		/* toggle animation running */
	case 'a':
		isAnimating = !isAnimating;
		break;
		/* reset */
	case 'r':
		rotation = 0;
		break;
		/* quit! */
	case 'Q':
	case 'q':
	case 27:
		exit(0);
	}

	/* notify window that it has to be re-rendered */
	glutPostRedisplay();
}

/* pre:  arrow or function key has been pressed
   post: scene is updated and re-rendered */
static void onSpecialKeyPress(int key, int x, int y) {
	/* please do not change function of these keys */
	switch (key) {
		/* toggle full screen mode */
	case GLUT_KEY_F1:
		isFullScreen = !isFullScreen;
		if (isFullScreen) {
			theWindowPositionX = glutGet((GLenum)(GLUT_WINDOW_X));
			theWindowPositionY = glutGet((GLenum)(GLUT_WINDOW_Y));
			glutFullScreen();
		}
		else {
			glutReshapeWindow(theWindowWidth, theWindowHeight);
			glutPositionWindow(theWindowPositionX, theWindowPositionY);
		}
		break;
	}

	/* notify window that it has to be re-rendered */
	glutPostRedisplay();
}

/* pre:  glut window has just been iconified or restored
   post: if window is visible, animate model, otherwise don't bother */
static void onVisible(int state) {
	if (state == GLUT_VISIBLE) {
		/* tell glut to show model again */
		glutIdleFunc(onIdle);
	}
	else {
		glutIdleFunc(NULL);
	}
}

static void InitializeGlutCallbacks() {
	/* tell glut how to display model */
	glutDisplayFunc(onDisplay);
	/* tell glutwhat to do when it would otherwise be idle */
	glutIdleFunc(onIdle);
	/* tell glut how to respond to changes in window size */
	glutReshapeFunc(onReshape);
	/* tell glut how to handle changes in window visibility */
	glutVisibilityFunc(onVisible);
	/* tell glut how to handle key presses */
	glutKeyboardFunc(onAlphaNumericKeyPress);
	glutSpecialFunc(onSpecialKeyPress);
	/* tell glut how to handle the mouse */
	glutMotionFunc(onMouseMotion);
	glutMouseFunc(onMouseButtonPress);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);

	/* request initial window size and position on the screen */
	glutInitWindowSize(theWindowWidth, theWindowHeight);
	glutInitWindowPosition(theWindowPositionX, theWindowPositionY);
	/* request full color with double buffering and depth-based rendering */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	/* create window whose title is the name of the executable */
	glutCreateWindow(theProgramTitle);

	InitializeGlutCallbacks();

	// Must be done after glut is initialized!
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	printf("GL version: %s\n", glGetString(GL_VERSION));

	/* initialize model */
	onInit(argc, argv);

	/* give control over to glut to handle rendering and interaction  */
	glutMainLoop();

	/* program should never get here */

	return 0;
}

