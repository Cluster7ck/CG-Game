//
//  Camera.h
//  RealtimeRending
//
//  Created by Philipp Lensing on 22.10.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//

#ifndef __RealtimeRending__Camera__
#define __RealtimeRending__Camera__

#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>

#include "Matrix.h"
#include "vector.h"

extern const unsigned int g_WindowWidth;
extern const unsigned int g_WindowHeight;

class Camera
{
public:
    Camera();
    Camera(Vector& Pos, Vector& Target, Vector& Up);
    
    Vector getPosition();
    Vector getTarget();
    Vector getUp();
    
    void setPosition( const Vector& Pos);
    void setTarget( const Vector& Target);
    void setUp( const Vector& Up);

	void mouseInput(Matrix Object, int x, int y, int Button, int State);
	void mouseWheelInput(int x, int y, int Button, int Dir);

	const Matrix& getViewMatrix() const;
	const Matrix& getProjectionMatrix() const;
	int getWindowWidth();
	int getWindowHeight();
    
    void apply();
protected:
    void pan( float dx, float dy);
    void zoom( float dz);
	void rotateAroundObject(Matrix Object, float x, float y);
    void rotate( float x, float y );
    Vector getVSpherePos( float x, float y);
    Vector rotateAxisAngle( Vector v, Vector n, float a);
    
	Matrix m_ViewMatrix;
	Matrix m_ProjMatrix;
    Vector m_Position;
    Vector m_Target;
    Vector m_Up;
    Vector m_Panning;
    Vector m_Zoom;
    Vector m_Rotation;
    int m_LastMouseX;
    int m_LastMouseY;
};


#endif /* defined(__RealtimeRending__Camera__) */
