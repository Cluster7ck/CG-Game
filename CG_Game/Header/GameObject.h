#ifndef GAMEOBJECT_H
#define _GAMEOBJECT_H

#include <windows.h>
#include <GL/glew.h>
#include <GL/GLUT.h>
#include <GL/GL.h>

#include "vector.h"
#include "color.h"
#include "shaderprogram.h"

struct Vertex {
	Vertex();
	Vertex(const Vector& p, const Vector& n, float TexS, float TexT);
	Vector Position;
	Vector Normal;
	float  TexcoordS;
	float  TexcoordT;
};

class BoundingBox {
public:
	BoundingBox();
	BoundingBox(const Vector& min, const Vector& max);
	Vector Min;
	Vector Max;
};

class GameObject
{
public:
	GameObject();
	virtual ~GameObject() = 0;
	virtual void draw() = 0;
	void drawBoundingBox();
	BoundingBox getBoundingBox();
	ShaderProgram getShaderProgram();
protected:
	ShaderProgram m_ShaderProgram;
	BoundingBox m_BoundingBox;
};

#endif