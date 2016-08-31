//
//  sceneNode.cpp
//  CG_P1
//
//  Created by  on 30.04.16.
//  Copyright © 2016 hsos. All rights reserved.
//

#include "../Header/sceneNode.h"

SceneNode::SceneNode(){	
}

SceneNode::SceneNode(const std::string & Name, const Vector & Translation, const Vector & RotationAxis, const float RotationAngle, const Vector & Scale, SceneNode * pParent, Model * pModel){
	this->setName(Name);
	this->setLocalTransform(Translation, RotationAxis, RotationAngle);
	this->setScaling(Scale);
	this->setParent(pParent);
	this->setModel(pModel);
}

/* getter */

const std::string& SceneNode::getName() const {
	return this->m_Name;
}

const Matrix& SceneNode::getLocalTransform() const {
	return this->m_LocalTransform;
}

Matrix SceneNode::getGlobalTransform() const {
	Matrix m,sm;
	m = m.identity();
	if (this->getParent() == NULL) {
		return  m;
	}
	sm.scale(this->m_Scaling);

	m = this->getParent()->getGlobalTransformWithoutScaling()*m_LocalTransform*sm;
	return m;
}

Matrix SceneNode::getGlobalTransformWithoutScaling() const {
	Matrix m;
	m = m.identity();
	if (this->getParent() == NULL) {
		return m;
	}

	m = this->getParent()->getGlobalTransformWithoutScaling()*m_LocalTransform;
	return m;
}
const Vector& SceneNode::getScaling() const {
	return this->m_Scaling;
}

const SceneNode* SceneNode::getParent() const {
	return this->m_pParent;
}

const std::set<SceneNode*>& SceneNode::getChildren() const {
	return this->m_Children;
}

Model* SceneNode::getModel(){
	return this->m_pModel;
}

/* setter */

void SceneNode::setName(const std::string& Name) {
	this->m_Name = Name;
}

void SceneNode::setModel(Model* pModel) {
	this->m_pModel = pModel;
}

void SceneNode::setLocalTransform(const Vector& Translation, const Vector& RotationAxis, const float RotationAngle) {
	Matrix mt, mr;
	mt.translation(Translation);
	mr.rotationAxis(RotationAxis, RotationAngle);
	this->m_LocalTransform = mr*mt;
}

void SceneNode::setLocalTransform(const Matrix& LocalTransform) {
	this->m_LocalTransform = LocalTransform;
}

void SceneNode::setScaling(const Vector& Scaling) {
	this->m_Scaling = Scaling;
}

void SceneNode::setParent(SceneNode* pNode) {
	this->m_pParent = pNode;
}

void SceneNode::addChild(SceneNode* pChild) {
	this->m_Children.insert(pChild);
}

void SceneNode::removeChild(SceneNode* pChild) {
	this->m_Children.erase(pChild);
}