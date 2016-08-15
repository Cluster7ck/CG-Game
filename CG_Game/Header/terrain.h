//
//  terrain.h
//  CG_P1
//
//  Created by  on 25.05.16.
//  Copyright © 2016 hsos. All rights reserved.
//

#ifndef terrain_h
#define terrain_h

#include <iostream>
#include <windows.h>
#include <GL/glew.h>
#include <GL/GLUT.h>
#include <GL/GL.h>

#include "GameObject.h"
#include "vector.h"
#include "texture.h"
#include "RGBImage.h"
#include "PerlinNoise.h"

struct TerrainVertex {
	Vector Pos;
	Vector Normal;
	float u0, v0 = 0;
	float u1, v1 = 0;
};

class Terrain:public GameObject{
public:
	Terrain();
	~Terrain();
	bool load(const char* HeightMap, const char* DetailMap1, const char* DetailMap2, const char* MixMap, float Width, float Depth, float HeightMultiplier);
	bool create(const char* HeightMap, const char* DetailMap1, const char* DetailMap2, const char* MixMap, float Width, float Depth, float HeightMultiplier, float OffsetX, float OffsetY);
	void draw();
protected:
	Vector position;
	Texture m_HeightMap;
	Texture m_GrassTex;
	Texture m_SandTex;
	Texture m_MixingRatio;
	GLuint m_VertexBuffer;
	GLuint m_IndexBuffer;
	unsigned int indicesCount;
};

#endif /* terrain_h */
