//
//  Model.h
//  RealtimeRending
//
//  Created by Philipp Lensing on 23.10.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//

#ifndef __RealtimeRending__Model__
#define __RealtimeRending__Model__

#include <iostream>
#include "../Header/vector.h"
#include "../Header/color.h"

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <algorithm>

#include <assert.h>
#include <math.h>
#include <float.h>
#include "material.h"


struct Vertex {
    Vertex();
    Vertex( const Vector& p, const Vector& n, float TexS, float TexT);
    Vector Position;
    Vector Normal;
    float  TexcoordS;
    float  TexcoordT;
};

class BoundingBox {
public:
    BoundingBox();
    BoundingBox( const Vector& min, const Vector& max);
    Vector Min;
    Vector Max;
};

class Model {
public:
    Model();
    ~Model();
    const BoundingBox& boundingBox() const;
    bool load( const char* Filename, bool FitSize=true);
    void drawLines() const;
	void drawTriangles() const;

protected:
    void createCube();
	void createObject(const char* filename, bool fitSize);
	void createMaterials(const char* filename);
	void replaceFilename(const char* Filename, const char* replacer, char* destination);

    Material* m_pMaterials;
    unsigned int m_MaterialCount;
    Vertex* m_pVertices;
    unsigned int m_VertexCount;
    std::map<std::string,std::vector<unsigned int>> m_mtlMap;
	std::map<std::string, std::vector<unsigned int>>::iterator m_mtlMap_iter;
    BoundingBox m_Box;
};

#endif /* defined(__RealtimeRending__Model__) */
