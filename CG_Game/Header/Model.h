//
//  Model.h
//  RealtimeRending
//
//  Created by Philipp Lensing on 23.10.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//

#ifndef Model_h
#define Model_h

#include <windows.h>
#include <GL/glew.h>
#include <GL/GLUT.h>
#include <GL/GL.h>

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <algorithm>
#include <assert.h>
#include <math.h>
#include <float.h>

#include <iostream>

#include "utility_len.h"
#include "vector.h"
#include "color.h"
#include "Material.h"
#include "GameObject.h"
#include "GLError.h"

struct Vertex {
	Vertex();
	Vertex(const Vector& p, const Vector& n, float TexS, float TexT);
	Vector Position;
	Vector Normal;
	float  TexcoordS;
	float  TexcoordT;
};

class Model:public GameObject {
public:
    Model();
    ~Model();
	Model(const Model& m);
    bool load( const char* Filename, bool FitSize=true);
    void drawLines() const;
	void draw();
	void drawBuffer();
	void setUseShader(bool use);
protected:
	void createObject(const char* filename, bool fitSize);
	void createMaterials(const char* filename);
	void setShaderUniforms(Vector LightPos, Color LightColor, Color DiffColor, Color SpecColor, Color AmbientColor, float SpecExp);
	
	bool useShader;
	std::vector<Material> m_Materials;
    unsigned int m_MaterialCount;
    std::map<std::string,std::vector<unsigned int>> m_mtlMap;
	std::map<std::string, std::vector<unsigned int>>::iterator m_mtlMap_iter;

	Vertex* m_pVertices;
	unsigned int m_VertexCount;
	unsigned int *Indices;
	unsigned int indicesCount;
	GLuint m_VertexBuffer;
	GLuint m_IndexBuffer;
};

#endif /* defined(__RealtimeRending__Model__) */
