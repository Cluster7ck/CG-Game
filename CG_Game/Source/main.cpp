//
//  main.cpp
//  RealtimeRending
//
//  Created by Philipp Lensing on 22.10.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//


#include <iostream>
#include <math.h>
#ifdef WIN32
	#include <windows.h>
	#include <GL/glew.h>
	#include <GL/GLUT.h>
	#include <GL/GL.h>
#else
	#include <OpenGL/OpenGL.h>
	#include <GLUT/GLUT.h>
#endif

#include "../Header/Camera.h"
#include "../Header/texture.h"
//#include "../Header/PlaneModel.h"
#include "../Header/Terrain.h"

#define CHUNKS_COUNT 9

// window x and y size
const unsigned int g_WindowWidth=1024;
const unsigned int g_WindowHeight=768;

// light position (point light)
const Vector g_LightPos = Vector( 0,64,0);

//Terrain
struct TerrainChunks {
	Terrain terrainChunks[CHUNKS_COUNT];
	float minHeight;
	float maxHeight;	
	TerrainChunks() :  minHeight(FLT_MAX), maxHeight(FLT_MIN) {}
};
TerrainChunks tChunks;

Camera g_Camera;

int g_MouseButton = 0;
int g_MouseState = 0;

void SetupDefaultGLSettings();
void SetupTerrain();
void DrawScene();
void MouseCallback(int Button, int State, int x, int y);
void MouseMoveCallback(int x, int y);
void KeyboardCallback( unsigned char key, int x, int y);

int main(int argc, char * argv[]) {
    // initialize the glut system and create a window
    glutInitWindowSize(g_WindowWidth, g_WindowHeight);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow("CG Praktikum");
	#ifdef WIN32
		glewInit();
	#endif
    
    SetupDefaultGLSettings();
    
    glutDisplayFunc(DrawScene);
    glutMouseFunc(MouseCallback);
    glutKeyboardFunc(KeyboardCallback);
    glutMotionFunc(MouseMoveCallback);

	SetupTerrain();

    glutMainLoop();
}

void SetupTerrain() {
	/*Create Terrain Chunks*/
	int chunksPerSide = sqrt(CHUNKS_COUNT);
	int maxOffset = (chunksPerSide / 2);

	for (int y = 0; y < chunksPerSide; y++) {
		for (int x = 0; x < chunksPerSide; x++) {

			tChunks.terrainChunks[x + y*chunksPerSide] = *(new Terrain());

			int offsetX = (x % chunksPerSide) - maxOffset;
			int offsetY = (chunksPerSide - 1 - y % chunksPerSide) - maxOffset;
			tChunks.terrainChunks[x + y*chunksPerSide].create("Ressourcen/heightmap.bmp", "Ressourcen/grass.bmp", "Ressourcen/sand.bmp", "Ressourcen/mixmap.bmp", 60, 60, 7, offsetX, offsetY);
		}
	}
	for (int i = 0; i < CHUNKS_COUNT; i++) {
		if (tChunks.terrainChunks[i].getBoundingBox().Min.Y < tChunks.minHeight)
			tChunks.minHeight = tChunks.terrainChunks[i].getBoundingBox().Min.Y;
		if (tChunks.terrainChunks[i].getBoundingBox().Max.Y > tChunks.maxHeight)
			tChunks.maxHeight = tChunks.terrainChunks[i].getBoundingBox().Max.Y;
	}

	std::cout << "\nTerrain Min Height: " << tChunks.minHeight << std::endl;
	std::cout << "\nTerrain Max Height: " << tChunks.maxHeight << std::endl;
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
    g_Camera.mouseInput(x,y,Button,State);
}

void MouseMoveCallback( int x, int y) {
    g_Camera.mouseInput(x,y,g_MouseButton,g_MouseState);
}
 
void KeyboardCallback( unsigned char key, int x, int y) {    
}

void DrawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    g_Camera.apply();

	DrawGroundGrid();
    
    GLfloat lpos[4];
    lpos[0]=g_LightPos.X; lpos[1]=g_LightPos.Y; lpos[2]=g_LightPos.Z; lpos[3]=1;
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);

	for (int i = 0; i < CHUNKS_COUNT; i++) {
		tChunks.terrainChunks[i].draw();
		tChunks.terrainChunks[i].drawBoundingBox();
	}

    glutSwapBuffers();
    glutPostRedisplay();
}
