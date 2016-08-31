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
#include <random>
#include <windows.h>
#include <GL/glew.h>
#include <GL/GLUT.h>
#include <GL/GL.h>

#include "vector.h"
#include "texture.h"
#include "RGBImage.h"
#include "PerlinNoise.h"
#include "GLError.h"
#include "GameObject.h"
#include "sceneNode.h"

#define CHUNKSIZE 80

struct TerrainVertex {
	Vector Pos;
	Vector Normal;
};

class TerrainChunk:public GameObject{
public:
	TerrainChunk();
	~TerrainChunk();
	bool create(float Width, float Depth, float HeightMultiplier, float OffsetX, float OffsetY, PerlinNoise pn);
	bool createObjects(Model* ModelArray, int ModelArraySize, int ObjectCount, float Variance, PerlinNoise Pn);
	void bindBuffers();
	void draw();
protected:
	SceneNode objects;
	Vector position;
	TerrainVertex* Vertices;
	unsigned int *Indices;
	int terrain_offsetX;
	int terrain_offsetY;
	bool isBound;
	Texture m_HeightMap;
	Texture m_GrassTex;
	Texture m_SandTex;
	Texture m_MixingRatio;
	GLuint m_VertexBuffer;
	GLuint m_IndexBuffer;
	unsigned int indicesCount;
};

#endif /* TerrainChunk_h */
