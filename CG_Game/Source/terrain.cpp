#include "..\Header\Terrain.h"

Terrain::~Terrain() {
}

void Terrain::initChunks() {
	
	currentCenter.offsetX = 0;
	currentCenter.offsetY = 0;

	chunksPerSide = sqrt(chunkCount);
	maxOffset = (chunksPerSide / 2);

	for (int y = 0; y < chunksPerSide; y++) {
		for (int x = 0; x < chunksPerSide; x++) {
			TerrainOffset tempOffset;
			TerrainChunk tempChunk;
			tempOffset.offsetX = (x % chunksPerSide) - maxOffset;
			tempOffset.offsetY = (chunksPerSide - 1 - y % chunksPerSide) - maxOffset;

			tempChunk.create(CHUNKSIZE, CHUNKSIZE, 1, tempOffset.offsetX, tempOffset.offsetY, terrainNoise);
			tempChunk.createObjects(pickups, PICKUPS_PER_CHUNK, 0.5f, terrainNoise);
			tempChunk.bindBuffers();
			tempChunk.setShaders(this->m_ShaderProgram);

			std::pair<TerrainOffset,TerrainChunk> p(tempOffset, tempChunk);
			terrainMap.insert(p);
		}
	}

	m_BoundingBox.Max.X = m_BoundingBox.Max.Y = m_BoundingBox.Max.Z = FLT_MIN;
	m_BoundingBox.Min.X = m_BoundingBox.Min.Y = m_BoundingBox.Min.Z = FLT_MAX;

	typedef std::map<TerrainOffset, TerrainChunk>::iterator it;

	for (it iterator = terrainMap.begin(); iterator != terrainMap.end(); iterator++) {
		if (iterator->second.getBoundingBox().Min.Y < m_BoundingBox.Min.Y)
			m_BoundingBox.Min.Y = iterator->second.getBoundingBox().Min.Y;
		if (iterator->second.getBoundingBox().Max.Y > m_BoundingBox.Max.Y)
			m_BoundingBox.Max.Y = iterator->second.getBoundingBox().Max.Y;

		if (iterator->second.getBoundingBox().Min.X < m_BoundingBox.Min.X)
			m_BoundingBox.Min.X = iterator->second.getBoundingBox().Min.X;
		if (iterator->second.getBoundingBox().Max.X > m_BoundingBox.Max.X)
			m_BoundingBox.Max.X = iterator->second.getBoundingBox().Max.X;

		if (iterator->second.getBoundingBox().Min.Z < m_BoundingBox.Min.Z)
			m_BoundingBox.Min.Z = iterator->second.getBoundingBox().Min.Z;
		if (iterator->second.getBoundingBox().Max.Z > m_BoundingBox.Max.Z)
			m_BoundingBox.Max.Z = iterator->second.getBoundingBox().Max.Z;
	}

	//std::cout << "\nTerrain Min Height: " << m_BoundingBox.Min.Y << std::endl;
	//std::cout << "\nTerrain Max Height: " << m_BoundingBox.Max.Y << std::endl;

	//std::cout << "\nTerrain Min X: " << m_BoundingBox.Min.X << " Max X: " << m_BoundingBox.Max.X << std::endl;
	//std::cout << "\nTerrain Min Z: " << m_BoundingBox.Min.Z << " Max Z: " << m_BoundingBox.Max.Z << std::endl;
}

void Terrain::createChunkThread(TerrainOffset ChunkOffset) {
	TerrainChunk tempChunk;
	tempChunk.create(CHUNKSIZE, CHUNKSIZE, 1, ChunkOffset.offsetX, ChunkOffset.offsetY, terrainNoise);
	tempChunk.createObjects(pickups, PICKUPS_PER_CHUNK, 1.0f, terrainNoise);
	tempChunk.setShaders(this->m_ShaderProgram);
	std::pair<TerrainOffset, TerrainChunk> p(ChunkOffset, tempChunk);
	terrainMap.insert(p);
}

void Terrain::draw() {
	std::map<TerrainOffset, TerrainChunk>::iterator it;
	TerrainOffset tempOffset;
	TerrainChunk tempChunk;


	int sideSize = chunksPerSide;
	for (int y = 0; y < chunksPerSide; y++) {
		for (int x = 0; x < chunksPerSide; x++) {
			tempOffset.offsetX = ((x % chunksPerSide) - maxOffset) + currentCenter.offsetX;
			tempOffset.offsetY = ((chunksPerSide - 1 - y % chunksPerSide) - maxOffset) + currentCenter.offsetY;

			it = terrainMap.find(tempOffset);
			if (it != terrainMap.end()) {
				it->second.bindBuffers();
				if ((x != 0 && x != chunksPerSide - 1 && y != 0 && y != chunksPerSide - 1)) {
					m_ShaderProgram.activate();
					setShaderUniforms(Vector(0.0f, 64.0f, 0.0f), Color(1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f), Color(0.1f, 0.1f, 0.1f), Color(0.2f, 0.2f, 0.2f), 1, m_BoundingBox.Min.Y, m_BoundingBox.Max.Y);
					it->second.draw();
					m_ShaderProgram.deactivate();
					it->second.getObjectsNode()->draw();
					//it->second.drawBoundingBox();
				}
			}
			else{
				std::thread thread1(&Terrain::createChunkThread, this,tempOffset);
				thread1.detach();
			}
		}
	}
	
}

void Terrain::setShaderUniforms(Vector LightPos, Color LightColor, Color DiffColor, Color SpecColor, Color AmbientColor, float SpecExp, float MinHeight, float MaxHeight) {
	GLint paraID = m_ShaderProgram.getParameterID("LightPos");
	m_ShaderProgram.setParameter(m_ShaderProgram.getParameterID("LightPos"), LightPos);
	paraID = m_ShaderProgram.getParameterID("DiffColor");
	m_ShaderProgram.setParameter(m_ShaderProgram.getParameterID("DiffColor"), DiffColor);
	paraID = m_ShaderProgram.getParameterID("SpecColor");
	m_ShaderProgram.setParameter(m_ShaderProgram.getParameterID("SpecColor"), SpecColor);
	paraID = m_ShaderProgram.getParameterID("AmbientColor");
	m_ShaderProgram.setParameter(m_ShaderProgram.getParameterID("AmbientColor"), AmbientColor);
	paraID = m_ShaderProgram.getParameterID("SpecExp");
	m_ShaderProgram.setParameter(m_ShaderProgram.getParameterID("SpecExp"), SpecExp);
	paraID = m_ShaderProgram.getParameterID("MaxHeight");
	m_ShaderProgram.setParameter(m_ShaderProgram.getParameterID("MaxHeight"), MaxHeight);
	paraID = m_ShaderProgram.getParameterID("MinHeight");
	m_ShaderProgram.setParameter(m_ShaderProgram.getParameterID("MinHeight"), MinHeight);
}

void Terrain::setTerrainCenter(int x, int y) {
	currentCenter.offsetX = x;
	currentCenter.offsetY = y;
}

TerrainChunk Terrain::getCenterChunk() {
	std::map<TerrainOffset, TerrainChunk>::iterator it;
	it = terrainMap.find(currentCenter);

	return it->second;
}

void Terrain::setPickups(std::vector<WorldObject>* pickups) {
	this->pickups = pickups;
}

