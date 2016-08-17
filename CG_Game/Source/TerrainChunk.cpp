//
//  terrain.cpp
//  CG_P1
//
//  Created by  on 25.05.16.
//  Copyright © 2016 hsos. All rights reserved.
//

#include "../Header/TerrainChunk.h"

#define CHUNKSIZE 60
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
Vector triangleNormal(Vector a, Vector b, Vector c);
float map(float s, float a1, float a2, float b1, float b2);
TerrainChunk::TerrainChunk() {
}

TerrainChunk::~TerrainChunk() {

}

bool TerrainChunk::load(const char* HeightMap, const char* DetailMap1, const char* DetailMap2, const char* MixMap, float Width, float Depth, float HeightMultiplier) {
	//static Texture imgTexture;

	if (!m_HeightMap.LoadFromBMP(HeightMap)) {
		std::cout << "Fehler beim Lesen der HeightMap-Datei." << std::endl;
		return false;
	}
	
	// Aufgabe 2
	if (!m_GrassTex.LoadFromBMP(DetailMap1)) {
		std::cout << "Fehler beim Lesen der DetailMap1-Datei." << std::endl;
		return false;
	}

	if (!m_SandTex.LoadFromBMP(DetailMap2)) {
		std::cout << "Fehler beim Lesen der DetailMap2-Datei." << std::endl;
		return false;
	}

	if (!m_MixingRatio.LoadFromBMP(MixMap)) {
		std::cout << "Fehler beim Lesen der MixMap-Datei." << std::endl;
		return false;
	}

	RGBImage img = *m_HeightMap.getRGBImage();
	RGBImage mixMap = *m_MixingRatio.getRGBImage();
	int imgWidth = img.width();
	int imgHeight = img.height();

	// Vertices
	TerrainVertex* Vertices = new TerrainVertex[imgWidth*imgHeight];

	unsigned int k = 60;

	for (int x = 0; x < imgWidth; x++) {
		for (int y = 0; y < imgHeight; y++) {
			Color currentColor = img.getPixelColor(x, y);
			Vertices[x * imgWidth + y].Pos.X = x/(imgWidth*1.0f)*Width - (Width / 2);
			Vertices[x * imgWidth + y].Pos.Z = y/(imgHeight*1.0f)*Depth - (Depth / 2);
			Vertices[x * imgHeight + y].Pos.Y = currentColor.R * HeightMultiplier;
			Vertices[x * imgHeight + y].Normal = Vector();
			//Für Mixmap
			Vertices[x * imgHeight + y].u0 = x / (imgWidth * 1.0f);
			Vertices[x * imgHeight + y].v0 = y / (imgHeight * 1.0f);
			//Für Detailsmap
			Vertices[x * imgHeight + y].u1 = x / (imgWidth * 1.0f) * k;
			Vertices[x * imgHeight + y].v1 = y / (imgHeight * 1.0f) * k;
		}
	}

	position = Vertices[0].Pos;

	// Indices
	indicesCount = (imgWidth * imgHeight) * 6;
	unsigned int *Indices = new unsigned int[indicesCount];
	unsigned int vertexIndex = 0;

	for (int x = 0; x < imgWidth; x++) {
		for (int y = 0; y < imgHeight; y++) {
			if (x < imgWidth - 1 && y < imgHeight - 1) {
				Indices[vertexIndex] = x * imgWidth + y;
				Indices[vertexIndex + 1] = (x + 1)* imgWidth + (y + 1);
				Indices[vertexIndex + 2] = (x + 1)* imgWidth + y;
				
				Indices[vertexIndex + 3] = (x)* imgWidth + y;
				Indices[vertexIndex + 4] = (x)* imgWidth + (y + 1);
				Indices[vertexIndex + 5] = (x + 1)* imgWidth + (y + 1);
				vertexIndex += 6;
			}
		}
	}

	// Calc normals
	for (int x = 0; x < imgWidth; x++) {
		for (int y = 0; y < imgHeight; y++) {
			Vector a, b, c;
			Vector vertexNormal, normalTri1, normalTri2, normalTri3, normalTri4, normalTri5, normalTri6;
			
			if(x == 0 && y == 0){
				a = Vertices[x * imgWidth + y].Pos;
				b = Vertices[(x + 1)* imgWidth + y].Pos;
				c = Vertices[(x + 1)* imgWidth + (y + 1)].Pos;
				normalTri1 = triangleNormal(a,b,c);

				a = Vertices[x * imgWidth + y].Pos;
				b = Vertices[(x + 1)* imgWidth + (y + 1)].Pos;
				c = Vertices[x * imgWidth + (y + 1)].Pos;
				normalTri2 = triangleNormal(a, b, c);
				
				vertexNormal = ((normalTri1 + normalTri2)).normalize();
			}
			else if (y == 0) {
				if (x == imgWidth - 1) {
					a = Vertices[x * imgWidth + y].Pos;
					b = Vertices[x * imgWidth + (y + 1)].Pos;
					c = Vertices[(x - 1)* imgWidth + y].Pos;
					vertexNormal = triangleNormal(a, b, c);
				}
				else {
					a = Vertices[x * imgWidth + y].Pos;
					b = Vertices[(x + 1)* imgWidth + (y + 1)].Pos;
					c = Vertices[x * imgWidth + (y + 1)].Pos;
					normalTri1 = triangleNormal(a, b, c);

					a = Vertices[x * imgWidth + y].Pos;
					b = Vertices[(x + 1)* imgWidth + y].Pos;
					c = Vertices[(x + 1)* imgWidth + (y + 1)].Pos;
					normalTri2 = triangleNormal(a, b, c);

					a = Vertices[x * imgWidth + y].Pos;
					b = Vertices[x * imgWidth + (y + 1)].Pos;
					c = Vertices[(x-1) * imgWidth + y].Pos;
					normalTri3 = triangleNormal(a, b, c);

					vertexNormal = ((normalTri1 + normalTri2 + normalTri3)).normalize();
				}
			}
			else if (x == 0) {
				if (y == imgHeight - 1) {
					a = Vertices[x * imgWidth + (y)].Pos;
					b = Vertices[x * imgWidth + (y-1)].Pos;
					c = Vertices[(x + 1)* imgWidth + y].Pos;
					vertexNormal = triangleNormal(a, b, c);
				}
				else{
					a = Vertices[x * imgWidth + y].Pos;
					b = Vertices[x * imgWidth + (y-1)].Pos;
					c = Vertices[(x + 1)* imgWidth + y].Pos;
					normalTri1 = triangleNormal(a, b, c);

					a = Vertices[x * imgWidth + y].Pos;
					b = Vertices[(x + 1)* imgWidth + y].Pos;
					c = Vertices[(x + 1)* imgWidth + (y + 1)].Pos;
					normalTri2 = triangleNormal(a, b, c);

					a = Vertices[x * imgWidth + y].Pos;
					b = Vertices[(x + 1)* imgWidth + (y + 1)].Pos;
					c = Vertices[x * imgWidth + (y + 1)].Pos;
					normalTri3 = triangleNormal(a, b, c);

					vertexNormal = ((normalTri1 + normalTri2 + normalTri3)).normalize();
				}
			}
			else if (x == imgWidth - 1) {
				if (y == imgHeight - 1) {
					a = Vertices[x* imgWidth + y].Pos;
					b = Vertices[(x-1) * imgWidth + y].Pos;
					c = Vertices[(x - 1)* imgWidth + (y-1)].Pos;
					normalTri1 = triangleNormal(a, b, c);

					a = Vertices[x* imgWidth + y].Pos;
					b = Vertices[(x-1) * imgWidth + (y - 1)].Pos;
					c = Vertices[x * imgWidth + (y-1)].Pos;
					normalTri2 = triangleNormal(a, b, c);

					vertexNormal = ((normalTri1 + normalTri2)).normalize();
				}
				else {
					a = Vertices[x* imgWidth + y].Pos;
					b = Vertices[(x - 1) * imgWidth + y].Pos;
					c = Vertices[(x - 1)* imgWidth + (y - 1)].Pos;
					normalTri1 = triangleNormal(a, b, c);

					a = Vertices[x* imgWidth + y].Pos;
					b = Vertices[(x - 1) * imgWidth + (y - 1)].Pos;
					c = Vertices[x * imgWidth + (y - 1)].Pos;
					normalTri2 = triangleNormal(a, b, c);

					a = Vertices[x* imgWidth + y].Pos;
					b = Vertices[x * imgWidth + (y+1)].Pos;
					c = Vertices[(x-1) * imgWidth + y].Pos;
					normalTri3 = triangleNormal(a, b, c);

					vertexNormal = ((normalTri1 + normalTri2 + normalTri3)).normalize();
				}
			}
			else if(x < imgWidth - 1 && y < imgHeight - 1) {

				a = Vertices[x* imgWidth + y].Pos;
				b = Vertices[(x - 1) * imgWidth + y].Pos;
				c = Vertices[(x - 1)* imgWidth + (y - 1)].Pos;
				normalTri1 = triangleNormal(a, b, c);

				a = Vertices[x* imgWidth + y].Pos;
				b = Vertices[(x - 1) * imgWidth + (y - 1)].Pos;
				c = Vertices[x * imgWidth + (y - 1)].Pos;
				normalTri2 = triangleNormal(a, b, c);

				a = Vertices[x* imgWidth + y].Pos;
				b = Vertices[x * imgWidth + (y + 1)].Pos;
				c = Vertices[(x - 1) * imgWidth + y].Pos;
				normalTri3 = triangleNormal(a, b, c);
				
				a = Vertices[x * imgWidth + y].Pos;
				b = Vertices[x * imgWidth + (y - 1)].Pos;
				c = Vertices[(x + 1)* imgWidth + y].Pos;
				normalTri4 = triangleNormal(a, b, c);

				a = Vertices[x * imgWidth + y].Pos;
				b = Vertices[(x + 1)* imgWidth + y].Pos;
				c = Vertices[(x + 1)* imgWidth + (y + 1)].Pos;
				normalTri5 = triangleNormal(a, b, c);

				a = Vertices[x * imgWidth + y].Pos;
				b = Vertices[(x + 1)* imgWidth + (y + 1)].Pos;
				c = Vertices[x * imgWidth + (y + 1)].Pos;
				normalTri6 = triangleNormal(a, b, c);

				vertexNormal = ((normalTri1 + normalTri2 + normalTri3 + normalTri4 + normalTri5 + normalTri6)).normalize();
			}
			Vertices[x * imgWidth + y].Normal = vertexNormal * -1;
		}
	}
	
	// create gpu buffer for vertices
	glGenBuffers(1, &m_VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TerrainVertex) * (imgWidth * imgHeight), Vertices, GL_STATIC_DRAW);

	// create gpu buffer for indices
	glGenBuffers(1, &m_IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicesCount, Indices, GL_STATIC_DRAW);

	return true;
}

bool TerrainChunk::create(const char* HeightMap, const char* DetailMap1, const char* DetailMap2, const char* MixMap, float Width, float Depth, float HeightMultiplier, float OffsetX, float OffsetY) {
	//cant have more than one
	PerlinNoise pn(0.1, 0.12, 15, 1, 666);
	terrain_offsetX = OffsetX;
	terrain_offsetY = OffsetY;

	if (!m_HeightMap.LoadFromBMP(HeightMap)) {
		std::cout << "Fehler beim Lesen der HeightMap-Datei." << std::endl;
		return false;
	}

	// Aufgabe 2
	if (!m_GrassTex.LoadFromBMP(DetailMap1)) {
		std::cout << "Fehler beim Lesen der DetailMap1-Datei." << std::endl;
		return false;
	}

	if (!m_SandTex.LoadFromBMP(DetailMap2)) {
		std::cout << "Fehler beim Lesen der DetailMap2-Datei." << std::endl;
		return false;
	}

	if (!m_MixingRatio.LoadFromBMP(MixMap)) {
		std::cout << "Fehler beim Lesen der MixMap-Datei." << std::endl;
		return false;
	}

	RGBImage img = *m_HeightMap.getRGBImage();
	RGBImage mixMap = *m_MixingRatio.getRGBImage();

	// Vertices
	TerrainVertex* Vertices = new TerrainVertex[CHUNKSIZE*CHUNKSIZE];

	unsigned int k = 8;
	HeightMultiplier = 1;
	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			Color currentColor = img.getPixelColor(x, y);
			int currentIndex = x * CHUNKSIZE + y;
			Vertices[currentIndex].Pos.X = (x / (CHUNKSIZE*1.0f)*Width - (Width / 2)) + OffsetX*(Width-1);
			Vertices[currentIndex].Pos.Z = (y / (CHUNKSIZE*1.0f)*Depth - (Depth / 2)) + OffsetY*(Depth-1);
			//Height from Perlin Noise
			Vertices[currentIndex].Pos.Y = pn.GetHeight(x + (CHUNKSIZE -1) * OffsetX, y + (CHUNKSIZE -1) * OffsetY) * HeightMultiplier;
			//std::cout << "PN Height: " << pn.GetHeight(x + (CHUNKSIZE - 1) * OffsetX, y + (CHUNKSIZE - 1) * OffsetY) * HeightMultiplier << std::endl;
			//std::cout << "Remapped: " << map(pn.GetHeight(x + (CHUNKSIZE - 1) * OffsetX, y + (CHUNKSIZE - 1) * OffsetY) * HeightMultiplier, -5.88486f, 5.47452f, 0, 1) << std::endl;
			
			Vertices[currentIndex].Normal = Vector();
			//Für Mixmap
			Vertices[currentIndex].u0 = x / (CHUNKSIZE * 1.0f);
			Vertices[currentIndex].v0 = y / (CHUNKSIZE * 1.0f);
			//Für Detailsmap
			Vertices[currentIndex].u1 = x / (CHUNKSIZE * 1.0f) * k;
			Vertices[currentIndex].v1 = y / (CHUNKSIZE * 1.0f) * k;
		}
	}

	m_BoundingBox.Max.X = m_BoundingBox.Min.X = Vertices[0].Pos.X;
	m_BoundingBox.Max.Y = m_BoundingBox.Min.Y = Vertices[0].Pos.Y;
	m_BoundingBox.Max.Z = m_BoundingBox.Min.Z = Vertices[0].Pos.Z;
	
	for (unsigned int i = 0; i < CHUNKSIZE*CHUNKSIZE; i++) {
		Vector vertex = Vertices[i].Pos;

		// Box kleiner
		if (m_BoundingBox.Min.X > vertex.X) {
			m_BoundingBox.Min.X = vertex.X;
		}
		if (m_BoundingBox.Min.Y > vertex.Y) {
			m_BoundingBox.Min.Y = vertex.Y;
		}
		if (m_BoundingBox.Min.Z > vertex.Z) {
			m_BoundingBox.Min.Z = vertex.Z;
		}
		// Box größer
		if (m_BoundingBox.Max.X < vertex.X) {
			m_BoundingBox.Max.X = vertex.X;
		}
		if (m_BoundingBox.Max.Y < vertex.Y) {
			m_BoundingBox.Max.Y = vertex.Y;
		}
		if (m_BoundingBox.Max.Z < vertex.Z) {
			m_BoundingBox.Max.Z = vertex.Z;
		}
	}

	std::cout << "BBox X -> Min: " << m_BoundingBox.Min.X << " Max: " << m_BoundingBox.Max.X << " Dist: " << m_BoundingBox.Max.X - m_BoundingBox.Min.X << std::endl;
	std::cout << "BBox Y -> Min: " << m_BoundingBox.Min.Y << " Max: " << m_BoundingBox.Max.Y << " Dist: " << m_BoundingBox.Max.Y - m_BoundingBox.Min.Y << std::endl;
	std::cout << "BBox Z -> Min: " << m_BoundingBox.Min.Z << " Max: " << m_BoundingBox.Max.Z << " Dist: " << m_BoundingBox.Max.Z - m_BoundingBox.Min.Z << std::endl;

	// Indices
	indicesCount = (CHUNKSIZE * CHUNKSIZE) * 6;
	unsigned int *Indices = new unsigned int[indicesCount];
	unsigned int vertexIndex = 0;

	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			if (x < CHUNKSIZE - 1 && y < CHUNKSIZE - 1) {
				Indices[vertexIndex] = x * CHUNKSIZE + y;
				Indices[vertexIndex + 1] = (x + 1)* CHUNKSIZE + (y + 1);
				Indices[vertexIndex + 2] = (x + 1)* CHUNKSIZE + y;

				Indices[vertexIndex + 3] = (x)* CHUNKSIZE + y;
				Indices[vertexIndex + 4] = (x)* CHUNKSIZE + (y + 1);
				Indices[vertexIndex + 5] = (x + 1)* CHUNKSIZE + (y + 1);
				vertexIndex += 6;
			}
		}
	}

	// Calc normals
	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			Vector a, b, c;
			Vector vertexNormal, normalTri1, normalTri2, normalTri3, normalTri4, normalTri5, normalTri6;

			if (x == 0 && y == 0) {
				a = Vertices[x * CHUNKSIZE + y].Pos;
				b = Vertices[(x + 1)* CHUNKSIZE + y].Pos;
				c = Vertices[(x + 1)* CHUNKSIZE + (y + 1)].Pos;
				normalTri1 = triangleNormal(a, b, c);

				a = Vertices[x * CHUNKSIZE + y].Pos;
				b = Vertices[(x + 1)* CHUNKSIZE + (y + 1)].Pos;
				c = Vertices[x * CHUNKSIZE + (y + 1)].Pos;
				normalTri2 = triangleNormal(a, b, c);

				vertexNormal = ((normalTri1 + normalTri2)).normalize();
			}
			else if (y == 0) {
				if (x == CHUNKSIZE - 1) {
					a = Vertices[x * CHUNKSIZE + y].Pos;
					b = Vertices[x * CHUNKSIZE + (y + 1)].Pos;
					c = Vertices[(x - 1)* CHUNKSIZE + y].Pos;
					vertexNormal = triangleNormal(a, b, c);
				}
				else {
					a = Vertices[x * CHUNKSIZE + y].Pos;
					b = Vertices[(x + 1)* CHUNKSIZE + (y + 1)].Pos;
					c = Vertices[x * CHUNKSIZE + (y + 1)].Pos;
					normalTri1 = triangleNormal(a, b, c);

					a = Vertices[x * CHUNKSIZE + y].Pos;
					b = Vertices[(x + 1)* CHUNKSIZE + y].Pos;
					c = Vertices[(x + 1)* CHUNKSIZE + (y + 1)].Pos;
					normalTri2 = triangleNormal(a, b, c);

					a = Vertices[x * CHUNKSIZE + y].Pos;
					b = Vertices[x * CHUNKSIZE + (y + 1)].Pos;
					c = Vertices[(x - 1) * CHUNKSIZE + y].Pos;
					normalTri3 = triangleNormal(a, b, c);

					vertexNormal = ((normalTri1 + normalTri2 + normalTri3)).normalize();
				}
			}
			else if (x == 0) {
				if (y == CHUNKSIZE - 1) {
					a = Vertices[x * CHUNKSIZE + (y)].Pos;
					b = Vertices[x * CHUNKSIZE + (y - 1)].Pos;
					c = Vertices[(x + 1)* CHUNKSIZE + y].Pos;
					vertexNormal = triangleNormal(a, b, c);
				}
				else {
					a = Vertices[x * CHUNKSIZE + y].Pos;
					b = Vertices[x * CHUNKSIZE + (y - 1)].Pos;
					c = Vertices[(x + 1)* CHUNKSIZE + y].Pos;
					normalTri1 = triangleNormal(a, b, c);

					a = Vertices[x * CHUNKSIZE + y].Pos;
					b = Vertices[(x + 1)* CHUNKSIZE + y].Pos;
					c = Vertices[(x + 1)* CHUNKSIZE + (y + 1)].Pos;
					normalTri2 = triangleNormal(a, b, c);

					a = Vertices[x * CHUNKSIZE + y].Pos;
					b = Vertices[(x + 1)* CHUNKSIZE + (y + 1)].Pos;
					c = Vertices[x * CHUNKSIZE + (y + 1)].Pos;
					normalTri3 = triangleNormal(a, b, c);

					vertexNormal = ((normalTri1 + normalTri2 + normalTri3)).normalize();
				}
			}
			else if (x == CHUNKSIZE - 1) {
				if (y == CHUNKSIZE - 1) {
					a = Vertices[x* CHUNKSIZE + y].Pos;
					b = Vertices[(x - 1) * CHUNKSIZE + y].Pos;
					c = Vertices[(x - 1)* CHUNKSIZE + (y - 1)].Pos;
					normalTri1 = triangleNormal(a, b, c);

					a = Vertices[x* CHUNKSIZE + y].Pos;
					b = Vertices[(x - 1) * CHUNKSIZE + (y - 1)].Pos;
					c = Vertices[x * CHUNKSIZE + (y - 1)].Pos;
					normalTri2 = triangleNormal(a, b, c);

					vertexNormal = ((normalTri1 + normalTri2)).normalize();
				}
				else {
					a = Vertices[x* CHUNKSIZE + y].Pos;
					b = Vertices[(x - 1) * CHUNKSIZE + y].Pos;
					c = Vertices[(x - 1)* CHUNKSIZE + (y - 1)].Pos;
					normalTri1 = triangleNormal(a, b, c);

					a = Vertices[x* CHUNKSIZE + y].Pos;
					b = Vertices[(x - 1) * CHUNKSIZE + (y - 1)].Pos;
					c = Vertices[x * CHUNKSIZE + (y - 1)].Pos;
					normalTri2 = triangleNormal(a, b, c);

					a = Vertices[x* CHUNKSIZE + y].Pos;
					b = Vertices[x * CHUNKSIZE + (y + 1)].Pos;
					c = Vertices[(x - 1) * CHUNKSIZE + y].Pos;
					normalTri3 = triangleNormal(a, b, c);

					vertexNormal = ((normalTri1 + normalTri2 + normalTri3)).normalize();
				}
			}
			else if (x < CHUNKSIZE - 1 && y < CHUNKSIZE - 1) {

				a = Vertices[x* CHUNKSIZE + y].Pos;
				b = Vertices[(x - 1) * CHUNKSIZE + y].Pos;
				c = Vertices[(x - 1)* CHUNKSIZE + (y - 1)].Pos;
				normalTri1 = triangleNormal(a, b, c);

				a = Vertices[x* CHUNKSIZE + y].Pos;
				b = Vertices[(x - 1) * CHUNKSIZE + (y - 1)].Pos;
				c = Vertices[x * CHUNKSIZE + (y - 1)].Pos;
				normalTri2 = triangleNormal(a, b, c);

				a = Vertices[x* CHUNKSIZE + y].Pos;
				b = Vertices[x * CHUNKSIZE + (y + 1)].Pos;
				c = Vertices[(x - 1) * CHUNKSIZE + y].Pos;
				normalTri3 = triangleNormal(a, b, c);

				a = Vertices[x * CHUNKSIZE + y].Pos;
				b = Vertices[x * CHUNKSIZE + (y - 1)].Pos;
				c = Vertices[(x + 1)* CHUNKSIZE + y].Pos;
				normalTri4 = triangleNormal(a, b, c);

				a = Vertices[x * CHUNKSIZE + y].Pos;
				b = Vertices[(x + 1)* CHUNKSIZE + y].Pos;
				c = Vertices[(x + 1)* CHUNKSIZE + (y + 1)].Pos;
				normalTri5 = triangleNormal(a, b, c);

				a = Vertices[x * CHUNKSIZE + y].Pos;
				b = Vertices[(x + 1)* CHUNKSIZE + (y + 1)].Pos;
				c = Vertices[x * CHUNKSIZE + (y + 1)].Pos;
				normalTri6 = triangleNormal(a, b, c);

				vertexNormal = ((normalTri1 + normalTri2 + normalTri3 + normalTri4 + normalTri5 + normalTri6)).normalize();
			}
			Vertices[x * CHUNKSIZE + y].Normal = vertexNormal * -1;
		}
	}

	// create gpu buffer for vertices
	glGenBuffers(1, &m_VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TerrainVertex) * (CHUNKSIZE * CHUNKSIZE), Vertices, GL_STATIC_DRAW);

	// create gpu buffer for indices
	glGenBuffers(1, &m_IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicesCount, Indices, GL_STATIC_DRAW);

	return true;
}

void TerrainChunk::draw() {
	//setShaderUniforms(Vector(0, 64, 0), Color(1, 1, 1), Color(1.0, 1.0, 1.0), Color(0.6, 0, 0), Color(0.2, 0.2, 0.2), 4);

	// inform the client that we want to use array buffers
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// setup position & normal pointers
	glVertexPointer(3, GL_FLOAT, sizeof(TerrainVertex), BUFFER_OFFSET(0));
	glNormalPointer(GL_FLOAT, sizeof(TerrainVertex), BUFFER_OFFSET(12));

	// setup mixtexture
	glActiveTexture(GL_TEXTURE0);
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, sizeof(TerrainVertex), BUFFER_OFFSET(24));
	m_MixingRatio.apply();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	// setup texture-environment-unit 0 => grass
	glActiveTexture(GL_TEXTURE1);
	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, sizeof(TerrainVertex), BUFFER_OFFSET(32)); // first uv-set
	m_GrassTex.apply();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// setup texture-environment-unit 1 => sand
	glActiveTexture(GL_TEXTURE2);
	glClientActiveTexture(GL_TEXTURE2);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, sizeof(TerrainVertex), BUFFER_OFFSET(32)); // second uv-set
	m_SandTex.apply();

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);

	//Interpolate between grass and sand with factor from mixmap
	glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE1);
	glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE2);
	glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_TEXTURE0);
	glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
	glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_ONE_MINUS_SRC_COLOR);

	//save current state in texture so we can multiply with the shadow
	glActiveTexture(GL_TEXTURE3);
	glClientActiveTexture(GL_TEXTURE3);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, sizeof(TerrainVertex), BUFFER_OFFSET(32));
	m_GrassTex.apply();

	//multiply
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
	glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PREVIOUS);
	glTexEnvf(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PRIMARY_COLOR);
	glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);


	// we draw our terrain
	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
	
	/*
	glColor3d(1.0, 0.0, 0.0);
	char* bla = "bla";
	glRasterPos2f(500, 500);
	int len = (int)strlen(bla);
	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, bla[i]);
	}
	*/
	// disable states in reverse order
	glDisable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE1);
	glClientActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);
	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
}

void TerrainChunk::drawTest() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	#ifdef GL_DEBUG
		check_gl_error();
	#endif // DEBUG
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	
	// setup position & normal pointers
	glVertexPointer(3, GL_FLOAT, sizeof(TerrainVertex), BUFFER_OFFSET(0));
	glNormalPointer(GL_FLOAT, sizeof(TerrainVertex), BUFFER_OFFSET(12));
	
	m_ShaderProgram.activate();
	setShaderUniforms(Vector(0, 64, 0), Color(1, 1, 1), Color(1.0, 1.0, 1.0), Color(0.6, 0, 0), Color(0.2, 0.2, 0.2), 1, 5.478, -5.9f);
	
	#ifdef GL_DEBUG
		check_gl_error();
	#endif // DEBUG
	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	m_ShaderProgram.deactivate();
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

}

void TerrainChunk::setShaderUniforms(Vector LightPos, Color LightColor, Color DiffColor, Color SpecColor, Color AmbientColor, float SpecExp, float MaxHeight, float MinHeight) {
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

Vector triangleNormal(Vector a, Vector b, Vector c) {
	return (b - a).cross(c - a);
}

float map(float s, float a1, float a2, float b1, float b2)
{
	return b1 + (s - a1)*(b2 - b1) / (a2 - a1);
}

void TerrainChunk::drawWithShader() {
	m_ShaderProgram.activate();
	draw();
	m_ShaderProgram.deactivate();
}