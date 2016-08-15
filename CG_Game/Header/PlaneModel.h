//
//  PlaneModel.h
//  RealtimeRending
//
//  Created by Philipp Lensing on 25.11.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//

#ifndef __RealtimeRending__PlaneModel__
#define __RealtimeRending__PlaneModel__

#include <iostream>

#include <windows.h>
#include <GL/glew.h>
#include <GL/GLUT.h>
#include <GL/GL.h>

#include "texture.h"
#include "vector.h"

struct PlaneVertex
{
    Vector Pos;
    Vector Normal;
    float u0,v0;
    float u1,v1;
};

class PlaneModel
{
public:
    PlaneModel();
    ~PlaneModel();
    bool load(const char* tex1, const char* tex2);
    void draw();
protected:
    Texture m_GrassTex;
    Texture m_SandTex;
    GLuint m_VertexBuffer;
    GLuint m_IndexBuffer;
};

#endif /* defined(__RealtimeRending__PlaneModel__) */
