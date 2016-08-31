//
//  sceneNode.h
//  CG_P1
//
//  Created by  on 30.04.16.
//  Copyright © 2016 hsos. All rights reserved.
//

#ifndef sceneNode_h
#define sceneNode_h

#include <set>
#include <string>
#include "../Header/vector.h"
#include "../Header/Model.h"
#include "../Header/Matrix.h"

class SceneNode
{
public:
	SceneNode();
	~SceneNode();
	SceneNode(const std::string& Name, const Vector& Translation, const Vector& RotationAxis, const float RotationAngle, const Vector& Scale, SceneNode* pParent, Model* pModel);

	// getter
	const std::string& getName() const;
	const Matrix& getLocalTransform() const;
	Matrix getGlobalTransform() const;
	Matrix getGlobalTransformWithoutScaling() const;
	const Vector& getScaling() const;
	const SceneNode* getParent() const;
	const std::set<SceneNode*>& getChildren() const;
	Model* getModel();

	// setter
	void setName(const std::string& Name);
	void setModel(Model* pModel);
	void setLocalTransform(const Vector& Translation, const Vector& RotationAxis, const float RotationAngle);
	void setLocalTransform(const Matrix& LocalTransform);
	void setScaling(const Vector& Scaling);
	void setParent(SceneNode* pNode);
	void addChild(SceneNode* pChild);
	void removeChild(SceneNode* pChild);
	void draw();


protected:
	void draw(SceneNode* pNode);
	BoundingBox getTransformedBoundingBox();
	std::string m_Name;
	Model* m_pModel;
	SceneNode* m_pParent;
	std::set<SceneNode*> m_Children;
	Matrix m_LocalTransform;
	Vector m_Scaling;
};

#endif /* sceneNode_h */

