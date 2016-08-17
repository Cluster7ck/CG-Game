//
//  TerrainChunk.h
//  CG_P1
//
//  Created by  on 25.05.16.
//  Copyright © 2016 hsos. All rights reserved.
//

#ifndef TerrainChunk_h
#define TerrainChunk_h

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
#include "GLError.h"

struct TerrainVertex {
	Vector Pos;
	Vector Normal;
	float u0, v0 = 0;
	float u1, v1 = 0;
};

class TerrainChunk:public GameObject{
public:
	TerrainChunk();
	~TerrainChunk();
	bool load(const char* HeightMap, const char* DetailMap1, const char* DetailMap2, const char* MixMap, float Width, float Depth, float HeightMultiplier);
	bool create(const char* HeightMap, const char* DetailMap1, const char* DetailMap2, const char* MixMap, float Width, float Depth, float HeightMultiplier, float OffsetX, float OffsetY);
	void draw();
	void drawTest();
	void setShaderUniforms(Vector LightPos, Color LightColor, Color DiffColor, Color SpecColor, Color AmbientColor, float SpecExp, float MaxHeight, float MinHeight);
	void drawWithShader();
protected:
	Vector position;
	int terrain_offsetX;
	int terrain_offsetY;
	Texture m_HeightMap;
	Texture m_GrassTex;
	Texture m_SandTex;
	Texture m_MixingRatio;
	GLuint m_VertexBuffer;
	GLuint m_IndexBuffer;
	unsigned int indicesCount;
};

#endif /* TerrainChunk_h */
