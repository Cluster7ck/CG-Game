//
//  Model.h
//  RealtimeRending
//
//  Created by Philipp Lensing on 23.10.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//

#ifndef Model_h
#define Model_h

#include <iostream>
#include "vector.h"
#include "color.h"
#include "Material.h"
#include "GameObject.h"

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <algorithm>
#include <assert.h>
#include <math.h>
#include <float.h>

class Model:public GameObject {
public:
    Model();
    ~Model();
    bool load( const char* Filename, bool FitSize=true);
    void drawLines() const;
	void draw();

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
};

#endif /* defined(__RealtimeRending__Model__) */
