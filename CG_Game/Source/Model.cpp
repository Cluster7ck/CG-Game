//
//  Model.cpp
//  RealtimeRending
//
//  Created by Philipp Lensing on 23.10.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//

#ifdef WIN32
	#include <Windows.h>
	#include <GL/glew.h>
	#include <GL/GLUT.h>
	#include <GL/GL.h>
#else
	#include <OpenGL/OpenGL.h>
	#include <GLUT/GLUT.h>
#endif

#include "../Header/Model.h"

void setMaterial(Material m);

Vertex::Vertex() {
}

Vertex::Vertex( const Vector& p, const Vector& n, float TexS, float TexT) {
    Position = p;
    Normal = n;
    TexcoordS = TexS;
    TexcoordT = TexT;
}

BoundingBox::BoundingBox() {
}

BoundingBox::BoundingBox( const Vector& min, const Vector& max) : Min(min), Max(max) {
}

Model::Model() : m_pVertices(NULL), m_pMaterials(NULL), m_MaterialCount(0), m_VertexCount(0) {

}

Model::~Model() {
    if( m_pVertices)
        delete [] m_pVertices;
    if(m_pMaterials)
        delete [] m_pMaterials;
}

bool Model::load(const char* Filename, bool FitSize) {
	// Blatt 3 Aufgabe 1
	//createCube();		

	// Blatt 3 Aufgabe 2
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

	std::string lastMtl="";

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
			} else {
				charPointer += 2;

				Vector vertex;

				sscanf(charPointer, "%f %f %f", &vertex.X, &vertex.Y, &vertex.Z);

				v.push_back(vertex);
			}
		}
        else if(strncmp(charPointer,"usemtl",6) == 0){
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
	if(m_mtlMap_iter != m_mtlMap.end()) {
		//f.size() - 1 : last index of material before
		m_mtlMap_iter->second.push_back(f.size() - 1);
	}

	// BoundingBox 
	m_Box.Max.X = m_Box.Min.X = v[0].X;
	m_Box.Max.Y = m_Box.Min.Y = v[0].Y;
	m_Box.Max.Z = m_Box.Min.Z = v[0].Z;

	for (unsigned int i = 0; i < v.size(); i++) {
		Vector vertex = v[i];

		// Box kleiner
		if (m_Box.Min.X < vertex.X) {
			m_Box.Min.X = vertex.X;
		}
		if (m_Box.Min.Y < vertex.Y) {
			m_Box.Min.Y = vertex.Y;
		}
		if (m_Box.Min.Z < vertex.Z) {
			m_Box.Min.Z = vertex.Z;
		}
		// Box größer
		if (m_Box.Max.X > vertex.X) {
			m_Box.Max.X = vertex.X;
		}
		if (m_Box.Max.Y > vertex.Y) {
			m_Box.Max.Y = vertex.Y;
		}
		if (m_Box.Max.Z > vertex.Z) {
			m_Box.Max.Z = vertex.Z;
		}
	}

	// Objekt skalieren
	if (FitSize) {
		float scale = 7 / ((m_Box.Min - m_Box.Max).length()*2);
		for (unsigned int i = 0; i < v.size(); i++) {
			v[i] = v[i] * scale;
		}
	}

	// Eckpunkte und Materialien zusammenstellen
	unsigned int faceCount = (unsigned int)f.size();
	m_pVertices = new Vertex[faceCount * 3];
	m_VertexCount = faceCount * 3;

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
        
        if(normal.length() == 0){
            //std::cout << "fehlerfall" <<std::endl;
            normal = Vector(1,0,0);
        } else{
            normal.normalize(); 
        }
		m_pVertices[i * 3].Normal =
			m_pVertices[i * 3 + 1].Normal =
			m_pVertices[i * 3 + 2].Normal = normal;
	}

	//print mtl with index
	/*for (auto const &itMap : m_mtlMap) {
		std::cout << " name: " << itMap.first <<std::endl;
		std::cout << "{";
		for (unsigned int i = 0; i < itMap.second.size(); i++) {
			std::cout << itMap.second[i] << " ";
		}
		std::cout << "}" << std::endl;
	}
	printf("Count: %d", m_VertexCount);

	printf("Vertices:\n");
	for (unsigned int i = 0; i<m_VertexCount; i++) {
		printf("%2i: ", i);
		printf("p(%2.1f, %2.1f, %2.1f) ", m_pVertices[i].Position.X, m_pVertices[i].Position.Y, m_pVertices[i].Position.Z);
		printf("n(%2.1f, %2.1f, %2.1f) ", m_pVertices[i].Normal.X, m_pVertices[i].Normal.Y, m_pVertices[i].Normal.Z);
		printf("t(%2.1f, %2.1f)\n", m_pVertices[i].TexcoordS, m_pVertices[i].TexcoordT);
	}*/
    fileStream.close();
	
}

void Model::createCube() {
    Vector PositionsFromFile[8] = {
        Vector(-1, -1, 1),
        Vector(-1,  1, 1),
        Vector( 1, -1, 1),
        Vector( 1,  1, 1),
        Vector( 1, -1, -1),
        Vector( 1,  1, -1),
        Vector(-1, -1, -1),
        Vector(-1, 1, -1)
    };
    struct Texcoord {
        float s,t;
    };
    Texcoord TexcoordFromFile[20] = {
        { 0, 1 },
        { 1, 0 },
        { 0, 0 },
        { 0, 0 },
        { 1, 1 },
        { 0, 1 },
        { 1, 1 },
        { 0, 0 },
        { 1, 0 },
        { 1, 0 },
        { 0, 1 },
        { 1, 1 },
        { 1, 0 },
        { 0, 0 },
        { 1, 1 },
        { 0, 1 },
        { 0, 0 },
        { 1, 0 },
        { 0, 1 },
        { 1, 1 }
    };
    struct Face {
        unsigned int pidx[3], tidx[3];
    };
    Face FacesFromFile[12] {
        //face 1
        {3,4,2,   9, 12, 6},
        {3,2,1,   9, 6, 3},
        //face 2
        {5,6,4,   13,15,11},
        {5,4,3,   13,11,8},
        //face 3
        {7,8,6,   18,20,16},
        {7,6,5,   18,16,14},
        //face 4
        {1,2,8,   2,5,19},
        {1,8,7,   2,19,17},
        //face 5
        {4,6,8,   10,15,19},
        {4,8,2,   10,19,4},
        //face 6
        {5,3,1,   13,7,1},
        {5,1,7,   13,1,17} 
    };
    
    m_pVertices = new Vertex[12*3];
    m_VertexCount = 12*3;
    for(int i=0; i<12; i++ ) {
        unsigned int PosIdx0 = FacesFromFile[i].pidx[0]-1;
        unsigned int PosIdx1 = FacesFromFile[i].pidx[1]-1;
        unsigned int PosIdx2 = FacesFromFile[i].pidx[2]-1;
        
        unsigned int TexIdx0 = FacesFromFile[i].tidx[0]-1;
        unsigned int TexIdx1 = FacesFromFile[i].tidx[1]-1;
        unsigned int TexIdx2 = FacesFromFile[i].tidx[2]-1;
        
        Vector a = m_pVertices[i*3].Position = PositionsFromFile[ PosIdx0];
        Vector b = m_pVertices[i*3+1].Position = PositionsFromFile[ PosIdx1];
        Vector c = m_pVertices[i*3+2].Position = PositionsFromFile[ PosIdx2];
        
        m_pVertices[i*3].TexcoordS = TexcoordFromFile[TexIdx0].s;
        m_pVertices[i*3+1].TexcoordS = TexcoordFromFile[TexIdx1].s;
        m_pVertices[i*3+2].TexcoordS = TexcoordFromFile[TexIdx2].s;
        
        m_pVertices[i*3].TexcoordT = TexcoordFromFile[TexIdx0].t;
        m_pVertices[i*3+1].TexcoordT = TexcoordFromFile[TexIdx1].t;
        m_pVertices[i*3+2].TexcoordT = TexcoordFromFile[TexIdx2].t;
        
        Vector normal = (b-a).cross(c-a);
        normal.normalize();
        
        m_pVertices[i*3].Normal =
        m_pVertices[i*3+1].Normal =
        m_pVertices[i*3+2].Normal = normal;
    }
    /*
    printf( "Vertices:\n");
    for( unsigned int i=0; i<m_VertexCount; i++) {
        printf( "%2i: ", i);
        printf( "p(%2.1f, %2.1f, %2.1f) ", m_pVertices[i].Position.X, m_pVertices[i].Position.Y, m_pVertices[i].Position.Z );
        printf( "n(%2.1f, %2.1f, %2.1f) ", m_pVertices[i].Normal.X, m_pVertices[i].Normal.Y, m_pVertices[i].Normal.Z );
        printf( "t(%2.1f, %2.1f)\n", m_pVertices[i].TexcoordS, m_pVertices[i].TexcoordT );  
    }*/
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

const BoundingBox& Model::boundingBox() const {
    return m_Box;
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

void Model::drawTriangles() const {
	//Draw Triangles for every Material
	if (m_mtlMap.empty()) {
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

void Model::replaceFilename(const char* Filename,const char* replacer,char* destination) {
	char charPointer;
	strcpy(destination, Filename);
	//Last occurence of slash
	char* backslashPointer = strrchr(destination, '/');
	//copy material filename behind slash
	strcpy(backslashPointer + 1, replacer);
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