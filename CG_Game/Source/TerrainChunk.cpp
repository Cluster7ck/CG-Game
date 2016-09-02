#include "../Header/TerrainChunk.h"


#define BUFFER_OFFSET(i) ((char *)NULL + (i))
Vector triangleNormal(Vector a, Vector b, Vector c);

TerrainChunk::TerrainChunk() {
	isBound = false;
	objects = new SceneNode();
	objects->setLocalTransform(Vector(0, 0, 0), Vector(0, 1, 0), 0);
	objects->setName(std::string("Root"));
	objects->setParent(NULL);
}

TerrainChunk::~TerrainChunk() {
}

bool TerrainChunk::create(float Width, float Depth, float HeightMultiplier, float OffsetX, float OffsetY, PerlinNoise Pn) {
	terrainOffsetX = OffsetX;
	terrainOffsetZ = OffsetY;

	Vertices = new TerrainVertex[CHUNKSIZE*CHUNKSIZE];

	unsigned int k = 8;
	for (int x = 0; x < CHUNKSIZE; x++) {
		for (int y = 0; y < CHUNKSIZE; y++) {
			int currentIndex = x * CHUNKSIZE + y;
			Vertices[currentIndex].Pos.X = (x / (CHUNKSIZE*1.0f)*Width - (Width / 2)) + OffsetX*(Width-1);
			Vertices[currentIndex].Pos.Z = (y / (CHUNKSIZE*1.0f)*Depth - (Depth / 2)) + OffsetY*(Depth-1);
			//Height from Perlin Noise
			Vertices[currentIndex].Pos.Y = Pn.GetHeight(Vertices[currentIndex].Pos.X, Vertices[currentIndex].Pos.Z) * HeightMultiplier;

			Vertices[currentIndex].Normal = Vector();
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
	std::cout << "\nChunk offX: " << OffsetX << " offY: " << OffsetY << std::endl;
	std::cout << std::endl << "\nBBox X -> Min: " << m_BoundingBox.Min.X << " Max: " << m_BoundingBox.Max.X << " Dist: " << m_BoundingBox.Max.X - m_BoundingBox.Min.X << std::endl;
	std::cout << "BBox Y -> Min: " << m_BoundingBox.Min.Y << " Max: " << m_BoundingBox.Max.Y << " Dist: " << m_BoundingBox.Max.Y - m_BoundingBox.Min.Y << std::endl;
	std::cout << "BBox Z -> Min: " << m_BoundingBox.Min.Z << " Max: " << m_BoundingBox.Max.Z << " Dist: " << m_BoundingBox.Max.Z - m_BoundingBox.Min.Z << std::endl;
	
	
	// Indices
	indicesCount = (CHUNKSIZE * CHUNKSIZE) * 6;
	Indices = new unsigned int[indicesCount];
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

	return true;
}

bool TerrainChunk::createObjects(std::vector<Model>* ModelVector, int ObjectCount, float Variance, PerlinNoise Pn) {
	int objCountDeviation = floor(ObjectCount * (1-Variance));
	srand(Pn.GetHeight(terrainOffsetX,terrainOffsetZ));
	int floor = ObjectCount - objCountDeviation, ceiling = ObjectCount + objCountDeviation, range = (ceiling - floor);
	ObjectCount = floor + int((range * rand()) / (RAND_MAX + 1.0));

	for (int i = 0; i < ObjectCount; i++) {
		float x, z, scale, rotation;
		int modelNr;

		std::stringstream ss;
		ss << " ObjNr. " << i << " of TerrainChunk (" << this->terrainOffsetX << " , " << this->terrainOffsetZ << ") ";
		std::string name = ss.str();

		scale = randomFloat(0.5f, 4.0f);
		x = randomFloat(m_BoundingBox.Min.X, m_BoundingBox.Max.X);
		z = randomFloat(m_BoundingBox.Min.Z, m_BoundingBox.Max.Z);
		//6.28319 radian  = 360 Deg
		rotation = 0;//randomFloat(0, 6.28319f);
		modelNr = rand() % ModelVector->size();
		
		
		SceneNode* objectNode = new SceneNode(	name, 
												Vector(x, Pn.GetHeight(x,z) ,z),
												Vector(0,1,0),
												rotation,
												Vector(scale,scale,scale), 
												objects, 
												&(ModelVector->at(modelNr)));
		//std::cout << "Node: " << objectNode->getName() << " y: " << objectNode->getLocalTransform().translation().Y << std::endl;
		objects->addChild(objectNode);
	}

	return true;
}
//OpenGL calls hard to do in thread...
void TerrainChunk::bindBuffers() {
	if (!isBound) {
		glGenBuffers(1, &m_VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(TerrainVertex) * (CHUNKSIZE * CHUNKSIZE), Vertices, GL_STATIC_DRAW);

		// create gpu buffer for indices
		glGenBuffers(1, &m_IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicesCount, Indices, GL_STATIC_DRAW);

		isBound = true;
	}
}

void TerrainChunk::draw() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

	#ifdef GL_DEBUG
		check_gl_error();
	#endif // DEBUG

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	// setup position & normal pointers
	glVertexPointer(3, GL_FLOAT, sizeof(TerrainVertex), BUFFER_OFFSET(0));
	glNormalPointer(GL_FLOAT, sizeof(TerrainVertex), BUFFER_OFFSET(12));


	#ifdef GL_DEBUG
		check_gl_error();
	#endif // DEBUG

	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

Vector triangleNormal(Vector a, Vector b, Vector c) {
	return (b - a).cross(c - a);
}