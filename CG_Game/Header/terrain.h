#ifndef Terrain_h
#define Terrain_h

#include "TerrainChunk.h"

class Terrain {
public:
	Terrain(int chunkCount) : minHeight(), maxHeight(), chunkCount(chunkCount) {};
	~Terrain();
	void initChunks();
	void draw();
	TerrainChunk* chunks;
private:
	
	float chunkCount;
	float minHeight;
	float maxHeight;
};
#endif /* Terrain_h */
