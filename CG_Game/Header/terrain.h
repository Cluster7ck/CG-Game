#ifndef Terrain_h
#define Terrain_h

#include "TerrainChunk.h"

class Terrain:public GameObject {
public:
	Terrain(int chunkCount) : minHeight(), maxHeight(), chunkCount(chunkCount) {};
	~Terrain();
	void initChunks();
	void draw();
	void setShaderUniforms(Vector LightPos, Color LightColor, Color DiffColor, Color SpecColor, Color AmbientColor, float SpecExp, float MaxHeight, float MinHeight);
	TerrainChunk* chunks;
private:
	float chunkCount;
	float minHeight;
	float maxHeight;
};
#endif /* Terrain_h */
