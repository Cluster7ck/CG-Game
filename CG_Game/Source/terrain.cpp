#include "..\Header\Terrain.h"

Terrain::~Terrain() {
	delete[] chunks;
}

void Terrain::initChunks() {
	chunks = new TerrainChunk[chunkCount];

	int chunksPerSide = sqrt(chunkCount);
	int maxOffset = (chunksPerSide / 2);
	PerlinNoise pn(0.05, 0.1, 20, 1, 666);
	for (int y = 0; y < chunksPerSide; y++) {
		for (int x = 0; x < chunksPerSide; x++) {

			chunks[x + y*chunksPerSide] = *(new TerrainChunk());

			int offsetX = (x % chunksPerSide) - maxOffset;
			int offsetY = (chunksPerSide - 1 - y % chunksPerSide) - maxOffset;
			chunks[x + y*chunksPerSide].create(255, 255, 7, offsetX, offsetY, pn);
			chunks[x + y*chunksPerSide].setShaders(this->m_ShaderProgram);
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
	m_ShaderProgram.activate();
	setShaderUniforms(Vector(0, 64, 0), Color(1, 1, 1), Color(0.5, 0.2, 0.3), Color(0.6, 0, 0), Color(0.2, 0.2, 0.2), 1, maxHeight, minHeight);
	for (int i = 0; i < chunkCount; i++) {
		chunks[i].draw();
	}
	m_ShaderProgram.deactivate();
}

void Terrain::setShaderUniforms(Vector LightPos, Color LightColor, Color DiffColor, Color SpecColor, Color AmbientColor, float SpecExp, float MaxHeight, float MinHeight) {
	GLint paraID = m_ShaderProgram.getParameterID("LightPos");
	m_ShaderProgram.setParameter(m_ShaderProgram.getParameterID("LightPos"), LightPos);
	//paraID = m_ShaderProgram.getParameterID("litColor");
	//m_ShaderProgram.setParameter(m_ShaderProgram.getParameterID("litColor"), LightColor);
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