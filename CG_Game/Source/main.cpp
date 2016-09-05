#include <iostream>
#include <math.h>
#include <vector>
#include <string>
#include <windows.h>
#include <GL/glew.h>
#include <GL/GLUT.h>
#include <GL/GL.h>
#include <GL/freeglut_ext.h>

#include "../Header/Camera.h"
#include "../Header/texture.h"
#include "../Header/Ball.h"
#include "../Header/Terrain.h"
#include "../Header/SkyBox.h"
#include "../Header//GLError.h"

//CHUNKS_COUNT is sqaure of grid side -> has to be odd
#define CHUNKS_COUNT 49
#define PICKUP_COUNT 6

// window x and y size
const unsigned int g_WindowWidth=1400;
const unsigned int g_WindowHeight=900;

// light position (point light)
const Vector g_LightPos = Vector( 0,64,0);

float deltaTime = 0;
int elapsedTimeLastFrame = 0;

//World Objects
struct ModelToLoad {
	std::string Filename;
	float minScale;
	float maxScale;
};

ModelToLoad objects[PICKUP_COUNT] = {
	{
		"Ressourcen/tree1.obj",
		1.0f,
		4.0f
	},
	{
		"Ressourcen/tree2.obj",
		2.5f,
		4.0f
	},
	{
		"Ressourcen/tree3.obj",
		3.5f,
		6.0f
	},
	{
		"Ressourcen/mushroom1.obj",
		0.2f,
		3.0f
	},
	{
		"Ressourcen/mushroom2.obj",
		0.5f,
		1.5f
	},
	{
		"Ressourcen/mushroom3.obj",
		0.5f,
		3.0f
	}
};

//Terrain
PerlinNoise noise(0.5, 0.042, 40, 1, 666);
std::vector<WorldObject> worldObjects;
Terrain terrain(CHUNKS_COUNT, noise);

Camera g_Camera;
SkyBox skybox;
Ball ball(18,noise);

float keyStore[4];

int g_mouseInput = 0;
int g_MouseButton = 0;
int g_MouseState = 0;
int g_MouseDir = 0;

void SetupDefaultGLSettings();
void DrawScene();
void MouseCallback(int Button, int State, int x, int y);
void MouseMoveCallback(int x, int y);
void KeyboardCallback( unsigned char key, int x, int y);
void KeyboardUpCallback(unsigned char key, int x, int y);

Vector getOffsets(Vector pos);
int main(int argc, char * argv[]) {
    // initialize the glut system and create a window
    glutInitWindowSize(g_WindowWidth, g_WindowHeight);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow("CG Praktikum");
	
	glewInit();
    
    SetupDefaultGLSettings();
    
    glutDisplayFunc(DrawScene);
	glutMouseFunc(MouseCallback);
    glutKeyboardFunc(KeyboardCallback);
	glutKeyboardUpFunc(KeyboardUpCallback);
    glutMotionFunc(MouseMoveCallback);

	skybox = SkyBox("Shader/skybox_vert.glsl", "Shader/skybox_frag.glsl");
	worldObjects.reserve(PICKUP_COUNT);
	for (int i = 0; i < PICKUP_COUNT; i++) {
		WorldObject tempWorldObj = *(new WorldObject());
		tempWorldObj.minScale = objects[i].minScale;
		tempWorldObj.maxScale = objects[i].maxScale;
		worldObjects.push_back(tempWorldObj);
		worldObjects.at(i).model.load(objects[i].Filename.c_str(), true);
		worldObjects.at(i).model.loadShaders("Shader/vertexshader.glsl", "Shader/blinn_phong_fragmentshader.glsl");
		worldObjects.at(i).model.setUseShader(true);
	}
	terrain.setPickups(&worldObjects);
	terrain.loadShaders("Shader/vertexshader.glsl", "Shader/terrain_fragmentshader.glsl");
	terrain.initChunks();
	ball.load("Ressourcen/player_ball.obj", Vector(0, 0.5, 0));
	ball.g_Model_ball.loadShaders("Shader/vertexshader.glsl", "Shader/blinn_phong_fragmentshader.glsl");
	ball.g_Model_ball.setUseShader(true);
    glutMainLoop();
}


void SetupDefaultGLSettings() {
    glClearColor(0, 0, 0, 255);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65, (double)g_WindowWidth/(double)g_WindowHeight, 0.045f, 1000.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
 
    // Setup Light Color
    GLfloat ambientLight[] = { 0.5f, 0.5f, 0.5f, 0.0f };
    GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

    // Setup default material
    float diff[4] = {1,1,1,1};
    float amb[4]  = {0.2f,0.2f,0.2f,1};
    float spec[4] = {0.5f,0.5f,0.5f,1};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMateriali(GL_FRONT, GL_SHININESS, 30);
    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void MouseCallback(int Button, int State, int x, int y) {
	g_MouseButton = Button;
	g_MouseState = State;
	g_Camera.mouseInput(ball.m_Ball, x, y, Button, State);
}

void MouseMoveCallback(int x, int y) {
	g_Camera.mouseInput(ball.m_Ball, x, y, g_MouseButton, g_MouseState);
}
 
void KeyboardCallback(unsigned char key, int x, int y) {
	// function is called if a regular keyboard button is pressed
	switch (key) {
	case 'w':
		keyStore[0] = 1;
		break;
	case 's':
		keyStore[1] = -1;
		break;
	case 'a':
		keyStore[2] = -1;
		break;
	case 'd':
		keyStore[3] = 1;
		break;
	default:
		x = y = 0;
		break;
	}
	ball.steer(keyStore[0] + keyStore[1], keyStore[2] + keyStore[3],false);
}

void KeyboardUpCallback(unsigned char key, int x, int y) {
	// function is called if a special keyboard button is pressed (e. g. Up-arrow-Key)
	switch (key) {
	case 'w':
		keyStore[0] = 0;
		break;
	case 's':
		keyStore[1] = 0;
		break;
	case 'a':
		keyStore[2] = 0;
		break;
	case 'd':
		keyStore[3] = 0;
		break;
	default:
		x = y = 0;
		break;
	}
	ball.steer(keyStore[0] + keyStore[1], keyStore[2] + keyStore[3],true);
}

void DrawScene() {
	deltaTime = (glutGet(GLUT_ELAPSED_TIME) - elapsedTimeLastFrame) / 1000.0f;
	elapsedTimeLastFrame = glutGet(GLUT_ELAPSED_TIME);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    g_Camera.apply();
	
    GLfloat lpos[4];
    lpos[0]=g_LightPos.X; lpos[1]=g_LightPos.Y; lpos[2]=g_LightPos.Z; lpos[3]=1;
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);

	skybox.draw(g_Camera.getProjectionMatrix(), g_Camera.getViewMatrix(), g_Camera.getPosition());
	ball.update(deltaTime,terrain.getCenterChunk().getObjectsNode());
	//ball.drawAxis();
	//ball.drawBoundingBox();

	//Ball Coordiantes to terrain offset.
	Vector offsets = getOffsets(ball.m_Ball.translation());
	terrain.setTerrainCenter(offsets.X, offsets.Z);
	terrain.draw();

    glutSwapBuffers();
    glutPostRedisplay();
}

Vector getOffsets(Vector pos) {
	Vector offs(0,0,0);
	int xSign = sgn(pos.X);
	int zSign = sgn(pos.Z);
	if (abs(pos.X) > CHUNKSIZE / 2) {
		offs.X = (floor((abs(pos.X) - CHUNKSIZE / 2) / CHUNKSIZE) + 1)*xSign;
	}

	if(abs(pos.Z) > CHUNKSIZE / 2) {
		offs.Z = ((floor(abs(pos.Z) - CHUNKSIZE / 2) / CHUNKSIZE) + 1)*zSign;

	}
	return offs;
}
