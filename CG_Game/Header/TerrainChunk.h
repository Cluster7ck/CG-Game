#ifndef TerrainChunk_h
#define TerrainChunk_h

#include <iostream>
#include <sstream>
#include <string>
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

#define CHUNKSIZE 300

struct TerrainVertex {
	Vector Pos;
	Vector Normal;
};

struct WorldObject {
	Model model;
	float minScale;
	float maxScale;
};

class TerrainChunk:public GameObject{
public:
	TerrainChunk();
	~TerrainChunk();
	bool create(float Width, float Depth, float HeightMultiplier, float OffsetX, float OffsetY, PerlinNoise pn);
	bool createObjects(std::vector<WorldObject>* ObjectVector, int ObjectCount, float Variance, PerlinNoise Pn);
	void bindBuffers();
	void draw();
	SceneNode* getObjectsNode() { return objects; };
protected:
	SceneNode* objects;
	Vector position;
	TerrainVertex* Vertices;
	unsigned int *Indices;
	int terrainOffsetX;
	int terrainOffsetZ;
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
