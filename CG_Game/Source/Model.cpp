//
//  Model.cpp
//  RealtimeRending
//
//  Created by Philipp Lensing on 23.10.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//

#include <Windows.h>
#include <GL/glew.h>
#include <GL/GLUT.h>
#include <GL/GL.h>

#include "../Header/Model.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
void setMaterial(Material m);

Model::Model() : m_pVertices(NULL), m_pMaterials(NULL), m_MaterialCount(0), m_VertexCount(0) {

}

Model::~Model() {
    if( m_pVertices)
        delete [] m_pVertices;
    if(m_pMaterials)
        delete [] m_pMaterials;
}

bool Model::load(const char* Filename, bool FitSize) {
	createObject(Filename, FitSize);
    return true;
}

void Model::createObject(const char* Filename, bool FitSize) {
	std::ifstream fileStream(Filename);
	if (!fileStream) {
		std::cout << "Die Datei \"" << Filename << "\" kann nicht geoeffnet werden." << std::endl;
		exit(-1);
	}

	std::string line = "";
	int fileLength = 1;
	int numVertices = 0;
	int numTexCoord = 0;
	int numFaces = 0;

	//Anzahl von v f vt
	while (std::getline(fileStream, line)) {

		if (!line.empty() && line[0] != '\0' && line[0] != '#') {
			char firstChar = line[0];
			char secondChar = line[1];

			if (firstChar == 'f') {
				numFaces++;
			}
			else if (firstChar == 'v') {
				if (secondChar == 't') {
					numTexCoord++;
				}
				else {
					numVertices++;
				}

			}
		}
		fileLength++;
	}

	fileStream.clear();
	fileStream.seekg(0, fileStream.beg);

	struct Face {
		//Position Index, Textur Index
		unsigned int pidx[3], tidx[3];
	};

	std::vector<Face> f;
	f.reserve(numFaces);

	std::vector<Vector> v;
	v.reserve(numVertices);

	struct Texcoord {
		float s, t;
	};

	std::vector<Texcoord> vt;
	vt.reserve(numTexCoord);

	std::string lastMtl = "";

	// Ganze Zahl, die das nächste zu lesende Zeichen darstellt, oder -1, wenn keine zu lesenden Zeichen vorhanden sind.
	while (fileStream.peek() != -1) {

		std::getline(fileStream, line);

		const char *charPointer = line.c_str();

		// Ueberspringe Endzeichen, Kommentare oder leere Zeilen
		if (line[0] == '\0' || line[0] == '#' || line.empty()) {
			continue;
		}

		// Textur
		if (charPointer[0] == 'f') {
			charPointer += 2;

			Face firstFace;
			Face secondFace;
			unsigned int posidx[4], texidx[4];

			int converted = sscanf(charPointer, "%d/%d %d/%d %d/%d %d/%d", &posidx[0], &texidx[0], &posidx[1], &texidx[1], &posidx[2], &texidx[2], &posidx[3], &texidx[3]);

			firstFace.pidx[0] = posidx[0];
			firstFace.pidx[1] = posidx[1];
			firstFace.pidx[2] = posidx[2];
			firstFace.tidx[0] = texidx[0];
			firstFace.tidx[1] = texidx[1];
			firstFace.tidx[2] = texidx[2];
			f.push_back(firstFace);

			if (converted == 8) {
				secondFace.pidx[0] = posidx[0];
				secondFace.pidx[1] = posidx[2];
				secondFace.pidx[2] = posidx[3];
				secondFace.tidx[0] = texidx[0];
				secondFace.tidx[1] = texidx[2];
				secondFace.tidx[2] = texidx[3];
				f.push_back(secondFace);
			}
		}
		else if (charPointer[0] == 'v') {
			// Texturkoordinaten
			if (charPointer[1] == 't') {
				charPointer += 3;

				Texcoord tex;

				sscanf(charPointer, "%f %f", &tex.s, &tex.t);

				vt.push_back(tex);

				// Eckpunkt
			}
			else {
				charPointer += 2;

				Vector vertex;

				sscanf(charPointer, "%f %f %f", &vertex.X, &vertex.Y, &vertex.Z);

				v.push_back(vertex);
			}
		}
		else if (strncmp(charPointer, "usemtl", 6) == 0) {
			charPointer += 7;
			std::string mtlName(charPointer);

			m_mtlMap_iter = m_mtlMap.find(lastMtl);
			if (mtlName.compare(lastMtl) != 0) {
				if (m_mtlMap_iter != m_mtlMap.end()) {
					//f.size() - 1 : last index of material before
					m_mtlMap_iter->second.push_back(f.size() - 1);
				}
			}
			else {
				m_mtlMap_iter->second.pop_back();
			}

			m_mtlMap_iter = m_mtlMap.find(mtlName);

			if (m_mtlMap_iter == m_mtlMap.end()) {
				std::vector<unsigned int> faceIndex;
				faceIndex.push_back(f.size());
				m_mtlMap.emplace(mtlName, faceIndex);
			}
			else {
				m_mtlMap_iter->second.push_back(f.size());
			}
			lastMtl = mtlName;
		}
		else if (strncmp(charPointer, "mtllib", 6) == 0) {
			charPointer += 7;

			char mtlFilename[256];
			replaceFilename(Filename, charPointer, mtlFilename);
			createMaterials(mtlFilename);
		}
	}

	//Close out last material
	m_mtlMap_iter = m_mtlMap.find(lastMtl);
	if (m_mtlMap_iter != m_mtlMap.end()) {
		//f.size() - 1 : last index of material before
		m_mtlMap_iter->second.push_back(f.size() - 1);
	}

	// BoundingBox 
	m_BoundingBox.Max.X = m_BoundingBox.Min.X = v[0].X;
	m_BoundingBox.Max.Y = m_BoundingBox.Min.Y = v[0].Y;
	m_BoundingBox.Max.Z = m_BoundingBox.Min.Z = v[0].Z;

	for (unsigned int i = 0; i < v.size(); i++) {
		Vector vertex = v[i];

		// Box kleiner
		if (m_BoundingBox.Min.X < vertex.X) {
			m_BoundingBox.Min.X = vertex.X;
		}
		if (m_BoundingBox.Min.Y < vertex.Y) {
			m_BoundingBox.Min.Y = vertex.Y;
		}
		if (m_BoundingBox.Min.Z < vertex.Z) {
			m_BoundingBox.Min.Z = vertex.Z;
		}
		// Box größer
		if (m_BoundingBox.Max.X > vertex.X) {
			m_BoundingBox.Max.X = vertex.X;
		}
		if (m_BoundingBox.Max.Y > vertex.Y) {
			m_BoundingBox.Max.Y = vertex.Y;
		}
		if (m_BoundingBox.Max.Z > vertex.Z) {
			m_BoundingBox.Max.Z = vertex.Z;
		}
	}

	// Objekt skalieren
	if (FitSize) {
		float scale = 7 / ((m_BoundingBox.Min - m_BoundingBox.Max).length() * 2);
		for (unsigned int i = 0; i < v.size(); i++) {
			v[i] = v[i] * scale;
		}
	}

	for (unsigned int i = 0; i < v.size(); i++) {
		Vector vertex = v[i];

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

	// Eckpunkte und Materialien zusammenstellen
	unsigned int faceCount = (unsigned int)f.size();
	m_pVertices = new Vertex[v.size()];
	m_VertexCount = v.size();

	indicesCount = faceCount * 3;
	Indices = new unsigned int[indicesCount];
	unsigned int vertexIndex = 0;
	
	for (unsigned int i = 0; i < v.size(); i++) {
		m_pVertices[i].Position = v[i];
	}
	
	for (unsigned int i = 0; i < faceCount; i++) {
		unsigned int PosIdx0 = f[i].pidx[0] - 1;
		unsigned int PosIdx1 = f[i].pidx[1] - 1;
		unsigned int PosIdx2 = f[i].pidx[2] - 1;

		unsigned int TexIdx0 = f[i].tidx[0] - 1;
		unsigned int TexIdx1 = f[i].tidx[1] - 1;
		unsigned int TexIdx2 = f[i].tidx[2] - 1;

		Indices[vertexIndex] = PosIdx0;
		Indices[vertexIndex + 1] = PosIdx1;
		Indices[vertexIndex + 2] = PosIdx2;

		m_pVertices[PosIdx0].TexcoordS = vt[TexIdx0].s;
		m_pVertices[PosIdx1].TexcoordS = vt[TexIdx1].s;
		m_pVertices[PosIdx2].TexcoordS = vt[TexIdx2].s;

		m_pVertices[PosIdx0].TexcoordT = vt[TexIdx0].t;
		m_pVertices[PosIdx1].TexcoordT = vt[TexIdx1].t;
		m_pVertices[PosIdx2].TexcoordT = vt[TexIdx2].t;

		Vector a = v[PosIdx0];
		Vector b = v[PosIdx1];
		Vector c = v[PosIdx2];
		Vector normal = (b - a).cross(c - a);
		if (normal.length() == 0) {
			//std::cout << "fehlerfall" <<std::endl;
			normal = Vector(1, 0, 0);
		}
		else {
			normal.normalize();
		}
		m_pVertices[PosIdx0].Normal = m_pVertices[PosIdx1].Normal = m_pVertices[PosIdx2].Normal = normal;

		vertexIndex += 3;
	}
	
	/*
	//Write vertices to indexbuffer
	for (unsigned int i = 0; i < faceCount; i++) {
		unsigned int PosIdx0 = f[i].pidx[0] - 1;
		unsigned int PosIdx1 = f[i].pidx[1] - 1;
		unsigned int PosIdx2 = f[i].pidx[2] - 1;

		unsigned int TexIdx0 = f[i].tidx[0] - 1;
		unsigned int TexIdx1 = f[i].tidx[1] - 1;
		unsigned int TexIdx2 = f[i].tidx[2] - 1;

		Vector a = m_pVertices[i * 3].Position = v[PosIdx0];
		Vector b = m_pVertices[i * 3 + 1].Position = v[PosIdx1];
		Vector c = m_pVertices[i * 3 + 2].Position = v[PosIdx2];

		m_pVertices[i * 3].TexcoordS = vt[TexIdx0].s;
		m_pVertices[i * 3 + 1].TexcoordS = vt[TexIdx1].s;
		m_pVertices[i * 3 + 2].TexcoordS = vt[TexIdx2].s;

		m_pVertices[i * 3].TexcoordT = vt[TexIdx0].t;
		m_pVertices[i * 3 + 1].TexcoordT = vt[TexIdx1].t;
		m_pVertices[i * 3 + 2].TexcoordT = vt[TexIdx2].t;
		//break at i == 12272 xy same normal = (0,0,0)  "f 23745/23745 23746/23746 23747/23747 "
		Vector normal = (b - a).cross(c - a);

		if (normal.length() == 0) {
			//std::cout << "fehlerfall" <<std::endl;
			normal = Vector(1, 0, 0);
		}
		else {
			normal.normalize();
		}
		m_pVertices[i * 3].Normal =
			m_pVertices[i * 3 + 1].Normal =
			m_pVertices[i * 3 + 2].Normal = normal;
	}*/
	fileStream.close();

	glGenBuffers(1, &m_VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_VertexCount, m_pVertices, GL_STATIC_DRAW);

	// create gpu buffer for indices
	glGenBuffers(1, &m_IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicesCount, Indices, GL_STATIC_DRAW);
}

void Model::createMaterials(const char* Filename) {
	std::ifstream fileStream(Filename);
	if (!fileStream) {
		std::cout << "Die Datei \"" << Filename << "\" kann nicht geoeffnet werden." << std::endl;

	}

	std::string line = "";
	int mtlCount = 0;
	while (std::getline(fileStream, line)) {
		const char* charPointer = line.c_str();
		if (strncmp(charPointer, "newmtl", 6) == 0) {
			mtlCount++;
		}
	}
	fileStream.clear();
	fileStream.seekg(0, fileStream.beg);

	m_pMaterials = new Material[mtlCount];

	while (std::getline(fileStream, line)) {
		//remove tabs
		line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());

		const char* charPointer = line.c_str();

		if (line[0] == '\0' || line[0] == '#' || line.empty()) {
			continue;
		}

		if (strncmp(charPointer, "newmtl", 6) == 0) {
			charPointer += 7;
			m_pMaterials[m_MaterialCount].setName(charPointer);
			m_MaterialCount++;
		}
		else if (strncmp(charPointer,"Kd",2) == 0) {
			charPointer += 2;
			Color diffColor;
			sscanf(charPointer, "%f %f %f", &diffColor.R, &diffColor.G, &diffColor.B);
			m_pMaterials[m_MaterialCount - 1].setDiffuseColor(diffColor);
		}
		else if (strncmp(charPointer, "Ks", 2) == 0) {
			charPointer += 2;
			Color specColor;
			sscanf(charPointer, "%f %f %f", &specColor.R, &specColor.G, &specColor.B);
			m_pMaterials[m_MaterialCount - 1].setSpecularColor(specColor);
		}
		else if (strncmp(charPointer, "Ns", 2) == 0) {
			charPointer += 2;
			float specExp;
			sscanf(charPointer, "%f", &specExp);
			m_pMaterials[m_MaterialCount - 1].setSpecularExponent(specExp);
		}
		else if (strncmp(charPointer, "Ka", 2) == 0) {
			charPointer += 2;
			Color ambColor;
			sscanf(charPointer, "%f %f %f", &ambColor.R, &ambColor.G, &ambColor.B);
			m_pMaterials[m_MaterialCount - 1].setAmbientColor(ambColor);
		}
		else if (strncmp(charPointer, "map_Kd", 6) == 0) {
			charPointer += 7;
			char textureFilename[256];
			replaceFilename(Filename, charPointer, textureFilename);
			m_pMaterials[m_MaterialCount - 1].setDiffuseTexture(textureFilename);
		}

	}
	fileStream.close();
}

void Model::drawLines() const {
	glBegin(GL_LINES);
	glColor3f(0.60, 0.20, 0.60);		//rgb(60%,20%,60%) = violette

	for (unsigned int i = 0; i < m_VertexCount / 3; i++) {
		for (int j = 0; j < 3; j++) {
			glVertex3f(m_pVertices[i * 3 + j].Position.X, m_pVertices[i * 3 + j].Position.Y, m_pVertices[i * 3 + j].Position.Z);
			if (j < 2) {
				glVertex3f(m_pVertices[i * 3 + j + 1].Position.X, m_pVertices[i * 3 + j + 1].Position.Y, m_pVertices[i * 3 + j + 1].Position.Z);
			} else {
				glVertex3f(m_pVertices[i * 3].Position.X, m_pVertices[i * 3].Position.Y, m_pVertices[i * 3].Position.Z);
			}
		}
	}

	glEnd();
}

void Model::draw(){
	//Draw Triangles for every Material
	if (m_mtlMap.empty() && !useShader) {
		glBegin(GL_TRIANGLES);

		for (unsigned int i = 0; i < m_VertexCount / 3; i++) {
			for (int j = 0; j < 3; j++) {
				glNormal3f(m_pVertices[i * 3 + j].Normal.X, m_pVertices[i * 3 + j].Normal.Y, m_pVertices[i * 3 + j].Normal.Z);
				glVertex3f(m_pVertices[i * 3 + j].Position.X, m_pVertices[i * 3 + j].Position.Y, m_pVertices[i * 3 + j].Position.Z);
			}
		}
		glEnd();
	}
	else {
		for (auto const &itMap : m_mtlMap) {

			Material currentMaterial;
			for (unsigned int k = 0; k < m_MaterialCount; k++) {
				if (itMap.first.compare(m_pMaterials[k].getName()) == 0) {
					currentMaterial = m_pMaterials[k];
				}
			}
			setMaterial(currentMaterial);

			//itMap.second is vector of face indices || indeces are in pairs x --- y => i=i+2
			glBegin(GL_TRIANGLES);
			for (unsigned int n = 0; n < itMap.second.size(); n += 2) {
				for (unsigned int i = itMap.second[n]; i <= itMap.second[n + 1]; i++) {
					for (int j = 0; j < 3; j++) {
						glNormal3f(m_pVertices[i * 3 + j].Normal.X, m_pVertices[i * 3 + j].Normal.Y, m_pVertices[i * 3 + j].Normal.Z);
						glTexCoord2f(m_pVertices[i * 3 + j].TexcoordS, m_pVertices[i * 3 + j].TexcoordT);
						glVertex3f(m_pVertices[i * 3 + j].Position.X, m_pVertices[i * 3 + j].Position.Y, m_pVertices[i * 3 + j].Position.Z);
					}
				}
			}
			glEnd();
		}
	}
}

void Model::drawBuffer() {

	m_ShaderProgram.activate();
	setShaderUniforms(Vector(0, 64, 0), Color(1, 1, 1), Color(1, 1, 1), Color(0.1, 0.1, 0.1), Color(0.2, 0.2, 0.2), 1, m_BoundingBox.Min.Y, m_BoundingBox.Max.Y);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

	#ifdef GL_DEBUG
		check_gl_error();
	#endif // DEBUG

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	// setup position & normal pointers
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));
	glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(12));


	#ifdef GL_DEBUG
		check_gl_error();
	#endif // DEBUG

	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	m_ShaderProgram.deactivate();
}

void Model::replaceFilename(const char* Filename,const char* replacer,char* destination) {
	char charPointer;
	strcpy(destination, Filename);
	//Last occurence of slash
	char* backslashPointer = strrchr(destination, '/');
	//copy material filename behind slash
	strcpy(backslashPointer + 1, replacer);
}

void Model::setUseShader(bool useShader) {
	this->useShader = useShader;
}

void setMaterial(Material mtl) {
	
	float diff[4];
	float spec[4];
	float amb[4];
	int specExp = ((int)mtl.getSpecularExponent() >= 0) ? (int)mtl.getSpecularExponent() : 0;
	mtl.getDiffuseColor().convert(diff);
	mtl.getSpecularColor().convert(spec);
	mtl.getAmbientColor().convert(amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	glMateriali(GL_FRONT, GL_SHININESS, specExp);
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);

	mtl.getTexture().apply();
}

void Model::setShaderUniforms(Vector LightPos, Color LightColor, Color DiffColor, Color SpecColor, Color AmbientColor, float SpecExp, float MinHeight, float MaxHeight) {
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
