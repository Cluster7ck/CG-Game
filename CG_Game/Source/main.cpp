//
//  main.cpp
//  RealtimeRending
//
//  Created by Philipp Lensing on 22.10.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//


#include <iostream>
#include <math.h>
#include <windows.h>
#include <GL/glew.h>
#include <GL/GLUT.h>
#include <GL/GL.h>
#include <GL/freeglut_ext.h>

#include "../Header/Camera.h"
#include "../Header/texture.h"
#include "../Header/Ball.h"
#include "../Header/Terrain.h"

#define CHUNKS_COUNT 49

// window x and y size
const unsigned int g_WindowWidth=1400;
const unsigned int g_WindowHeight=900;

// light position (point light)
const Vector g_LightPos = Vector( 0,64,0);

float deltaTime = 0;
int elapsedTimeLastFrame = 0;

//Terrain
PerlinNoise noise(0.5, 0.042, 40, 1, 1400);
Terrain terrain(CHUNKS_COUNT, noise);
Camera g_Camera;
Ball ball(15,noise);

float keyStore[4];

int g_MouseButton = 0;
int g_MouseState = 0;
int g_MouseDir = 0;

void SetupDefaultGLSettings();
void DrawScene();
void MouseCallback(int Button, int State, int x, int y);
void MouseMoveCallback(int x, int y);
void mouseWheel(int, int, int, int);
void KeyboardCallback( unsigned char key, int x, int y);
void KeyboardUpCallback(unsigned char key, int x, int y);
void SpecialKeyboardCallback(int key, int x, int y);
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
	glutMouseWheelFunc(mouseWheel);
    glutKeyboardFunc(KeyboardCallback);
	glutKeyboardUpFunc(KeyboardUpCallback);
    glutMotionFunc(MouseMoveCallback);
	glutSpecialFunc(SpecialKeyboardCallback);

	terrain.loadShaders("Shader/vertexshader.glsl", "Shader/dumb_shader.glsl");
	terrain.initChunks();
	ball.load("Ressourcen/ball.obj", Vector(0, 0.5, 0));
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

void DrawGroundGrid() {
    const float GridSize=10.0f;
    const unsigned int GridSegments=20;
    const float GridStep=GridSize/(float)GridSegments;
    const float GridOrigin=-GridSize*0.5f;
    
    glDisable( GL_LIGHTING);
    glBegin(GL_LINES);
        glColor3f(1.0f, 1.0f, 1.0f);
        for( unsigned int i=0; i<GridSegments+1; i++) {
            float itpos=GridOrigin + GridStep*(float)i;
            glVertex3f(itpos, 0, GridOrigin);
            glVertex3f(itpos, 0, GridOrigin+GridSize);
        
            glVertex3f(GridOrigin, 0, itpos);
            glVertex3f(GridOrigin+GridSize, 0, itpos);

        }
    glEnd();
    glEnable( GL_LIGHTING);
}

void MouseCallback(int Button, int State, int x, int y) {
	g_MouseButton = Button;
	g_MouseState = State;
	g_Camera.mouseInput(ball.m_Ball, x, y, Button, State);
}

void MouseMoveCallback(int x, int y) {
	g_Camera.mouseInput(ball.m_Ball, x, y, g_MouseButton, g_MouseState);
}

void mouseWheel(int Button, int Dir, int x, int y) {
	g_MouseButton = Button;
	g_MouseDir = Dir;
	g_Camera.mouseWheelInput(x, y, Button, Dir);
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
	ball.steer(keyStore[0] + keyStore[1], keyStore[2] + keyStore[3]);
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
	ball.steer(keyStore[0] + keyStore[1], keyStore[2] + keyStore[3]);
}

void SpecialKeyboardCallback(int key, int x, int y) {
	// function is called if a special keyboard button is pressed (e. g. Up-arrow-Key)
	static int offsetX = 0;
	static int offsetY = 0;
	switch (key) {
	//up
	case 101:
		offsetX++;
		break;
	//down
	case 103:
		offsetX--;
		break;
	//left
	case 100:
		offsetY--;
		break;
	//right
	case 102:
		offsetY++;
		break;
	default:
		x = y = 0;
		break;
	}
	terrain.setTerrainCenter(offsetX, offsetY);
}
void DrawScene() {
	deltaTime = (glutGet(GLUT_ELAPSED_TIME) - elapsedTimeLastFrame) / 1000.0f;
	elapsedTimeLastFrame = glutGet(GLUT_ELAPSED_TIME);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    g_Camera.apply();

	//DrawGroundGrid();
    
    GLfloat lpos[4];
    lpos[0]=g_LightPos.X; lpos[1]=g_LightPos.Y; lpos[2]=g_LightPos.Z; lpos[3]=1;
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	
	ball.update(deltaTime);
	ball.drawAxis();
	//Ball Coordiantes to terrain offset. switches when at center
	float offsetX = ball.m_Ball.translation().X >= 0 ?  floor(ball.m_Ball.translation().X / (CHUNKSIZE - 1)) : ceil(ball.m_Ball.translation().X / (CHUNKSIZE - 1));
	float offsetZ = ball.m_Ball.translation().Z >= 0 ? floor(ball.m_Ball.translation().Z / (CHUNKSIZE - 1)) : ceil(ball.m_Ball.translation().Z / (CHUNKSIZE - 1));
	terrain.setTerrainCenter(offsetX, offsetZ);
	terrain.draw();
	terrain.drawBoundingBox();


    glutSwapBuffers();
    glutPostRedisplay();
}
