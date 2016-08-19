#ifndef Terrain_h
#define Terrain_h
#include <map>
#include "TerrainChunk.h"

struct TerrainOffset {
	int offsetX;
	int offsetY;

	bool operator<(const TerrainOffset &o) const {
		return offsetX < o.offsetX || (offsetX == o.offsetX && offsetY < o.offsetY);
	}
};
class Terrain:public GameObject {
public:
	Terrain(int chunkCount) : chunkCount(chunkCount) {};
	~Terrain();
	void initChunks();
	void draw();
	void setShaderUniforms(Vector LightPos, Color LightColor, Color DiffColor, Color SpecColor, Color AmbientColor, float SpecExp, float MinHeight, float MaxHeight);
	TerrainChunk* chunks;
	void setTerrainCenter(int x, int y);
private:
	int chunkCount;
	int chunksPerSide;
	int maxOffset;
	TerrainOffset currentCenter;
	PerlinNoise terrainNoise;
	std::map<TerrainOffset, TerrainChunk> terrainMap;
};
#endif /* Terrain_h */
