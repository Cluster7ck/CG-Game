#ifndef Terrain_h
#define Terrain_h

#include <map>
#include <thread>

#include "TerrainChunk.h"
#include "utility_len.h"
#define CHUNKSIZE 80

struct TerrainOffset {
	int offsetX;
	int offsetY;
	//For map Key
	bool operator<(const TerrainOffset &o) const {
		return offsetX < o.offsetX || (offsetX == o.offsetX && offsetY < o.offsetY);
	}
};

class Terrain:public GameObject {
public:
	Terrain(int chunkCount, PerlinNoise terrainNoise) : chunkCount(chunkCount), terrainNoise(terrainNoise) {};
	~Terrain();
	void initChunks();
	void draw();
	void setShaderUniforms(Vector LightPos, Color LightColor, Color DiffColor, Color SpecColor, Color AmbientColor, float SpecExp, float MinHeight, float MaxHeight);
	void setTerrainCenter(int x, int y);
private:
	void createChunkThread(TerrainOffset ChunkOffset);
	int chunkCount;
	int chunksPerSide;
	int maxOffset;
	TerrainOffset currentCenter;
	PerlinNoise terrainNoise;
	std::map<TerrainOffset, TerrainChunk> terrainMap;
};
#endif /* Terrain_h */
