#include "..\Header\Terrain.h"

void Terrain::initChunks() {
	chunks = new TerrainChunk[chunkCount];

	int chunksPerSide = sqrt(chunkCount);
	int maxOffset = (chunksPerSide / 2);

	for (int y = 0; y < chunksPerSide; y++) {
		for (int x = 0; x < chunksPerSide; x++) {

			chunks[x + y*chunksPerSide] = *(new TerrainChunk());

			int offsetX = (x % chunksPerSide) - maxOffset;
			int offsetY = (chunksPerSide - 1 - y % chunksPerSide) - maxOffset;
			chunks[x + y*chunksPerSide].create("Ressourcen/heightmap.bmp", "Ressourcen/grass.bmp", "Ressourcen/sand.bmp", "Ressourcen/mixmap.bmp", 60, 60, 7, offsetX, offsetY);
			chunks[x + y*chunksPerSide].loadShaders("Shader/vertexshader.glsl", "Shader/dumb_shader.glsl");
		}
	}
	for (int i = 0; i < chunkCount; i++) {
		if (chunks[i].getBoundingBox().Min.Y < minHeight)
			minHeight = chunks[i].getBoundingBox().Min.Y;
		if (chunks[i].getBoundingBox().Max.Y > maxHeight)
			maxHeight = chunks[i].getBoundingBox().Max.Y;
	}

	std::cout << "\nTerrain Min Height: " << minHeight << std::endl;
	std::cout << "\nTerrain Max Height: " << maxHeight << std::endl;
}

void Terrain::draw() {
	for (int i = 0; i < chunkCount; i++) {
		
		chunks[i].drawTest();
	}
}

Terrain::~Terrain() {
	delete[] chunks;
}