#include "..\Header\Terrain.h"
float map(float s, float a1, float a2, float b1, float b2);

Terrain::~Terrain() {
	delete[] chunks;
}

void Terrain::initChunks() {
	chunks = new TerrainChunk[chunkCount];
	terrainNoise.Set(0.05, 0.1, 20, 1, 666);
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

			tempChunk.create(255, 255, 7, tempOffset.offsetX, tempOffset.offsetY, terrainNoise);
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

	std::cout << "\nTerrain Min Height: " << m_BoundingBox.Min.Y << std::endl;
	std::cout << "\nTerrain Max Height: " << m_BoundingBox.Max.Y << std::endl;
}

void Terrain::draw() {
	std::map<TerrainOffset, TerrainChunk>::iterator it;
	TerrainOffset tempOffset;
	TerrainChunk tempChunk;

	m_ShaderProgram.activate();
	setShaderUniforms(Vector(0, 64, 0), Color(1, 1, 1), Color(1, 1, 1), Color(0.6, 0, 0), Color(0.2, 0.2, 0.2), 1, m_BoundingBox.Min.Y, m_BoundingBox.Max.Y);

	for (int y = 0; y < chunksPerSide; y++) {
		for (int x = 0; x < chunksPerSide; x++) {
			tempOffset.offsetX = ((x % chunksPerSide) - maxOffset) + currentCenter.offsetX;
			tempOffset.offsetY = ((chunksPerSide - 1 - y % chunksPerSide) - maxOffset) + currentCenter.offsetY;

			it = terrainMap.find(tempOffset);
			if (it != terrainMap.end()) {
				it->second.draw();
				it->second.drawBoundingBox();
			}
			else{
				tempChunk.create(255, 255, 7, tempOffset.offsetX, tempOffset.offsetY, terrainNoise);
				tempChunk.setShaders(this->m_ShaderProgram);
				std::pair<TerrainOffset, TerrainChunk> p(tempOffset, tempChunk);
				terrainMap.insert(p);
				/*
				NEW THREAD
				create Chunk
				return Chunk
				add to map
				*/
			}
		}
	}
	m_ShaderProgram.deactivate();
	
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

float map(float s, float a1, float a2, float b1, float b2)
{
	return b1 + (s - a1)*(b2 - b1) / (a2 - a1);
}