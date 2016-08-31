//
//  scene.h
//  CG_P1
//
//  Created by  on 30.04.16.
//  Copyright © 2016 hsos. All rights reserved.
//

#ifndef scene_h
#define scene_h

#include "../Header/SceneNode.h"
#include "../Header/Model.h"
#include <map>
#include <iostream>
#include <fstream>

class Scene
{
public:
	Scene();
	~Scene();
	bool addSceneFile(const char* scenefile);
	void draw();
protected:
	void draw(SceneNode* pNode);
	SceneNode m_Root;
	std::map<std::string, Model*> m_Models;
};

#endif /* scene_h */
