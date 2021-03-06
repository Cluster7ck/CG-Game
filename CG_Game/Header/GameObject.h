#ifndef Gameobject_h
#define Gameobject_h

#include <windows.h>
#include <GL/glew.h>
#include <GL/GLUT.h>
#include <GL/GL.h>

#include "vector.h"
#include "color.h"
#include "shaderprogram.h"



class BoundingBox {
public:
	BoundingBox();
	BoundingBox(const Vector& min, const Vector& max);
	BoundingBox operator*(const Matrix& M) const;

	void draw();
	bool collision(const BoundingBox& otherBox);
	float getVolume();
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
	const BoundingBox& getBoundingBox() const;
	void setBoundingBox(BoundingBox bBox);
	ShaderProgram getShaderProgram();
	void loadShaders(const char* VertexShader, const char* FragmentShader);
	void setShaders(ShaderProgram shaderProgram);
	BoundingBox m_BoundingBox;
protected:
	ShaderProgram m_ShaderProgram;
	
};

#endif