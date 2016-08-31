#include "../Header/sceneNode.h"

SceneNode::SceneNode(){	
}

SceneNode::~SceneNode() {
	std::set<SceneNode *>::iterator it;

	/*for (it = this->getChildren().begin(); it != this->getChildren().end(); ++it) {
		delete it;
	}*/
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

void SceneNode::draw() {
	std::set<SceneNode *>::iterator it;

	for (it = this->getChildren().begin(); it != this->getChildren().end(); ++it) {
		this->draw(*it);
	}
}

void SceneNode::draw(SceneNode *node) {
	if (node != NULL && node->getModel() != NULL) {
		glPushMatrix();
		glMultMatrixf(node->getGlobalTransform());
		node->getModel()->drawBuffer();
		glPopMatrix();

		//node->getTransformedBoundingBox().draw();
	}

	std::set<SceneNode *>::iterator it;
	for (it = node->getChildren().begin(); it != node->getChildren().end(); ++it) {
		this->draw(*it);
	}
}

BoundingBox SceneNode::getTransformedBoundingBox(){
	BoundingBox tempBox = m_pModel->m_BoundingBox;
	tempBox.Min = this->getGlobalTransform() * tempBox.Min;
	tempBox.Max = this->getGlobalTransform() * tempBox.Max;
	/*
	tempBox.Min.X = tempBox.Min.X * m_Scaling.X;
	tempBox.Max.X = tempBox.Max.X * m_Scaling.X;

	tempBox.Min.Z = tempBox.Min.Z * m_Scaling.;
	tempBox.Max.Z = tempBox.Max.Z * m_Scaling.;

	tempBox.Min.Y = tempBox.Min.Y * m_Scaling.;
	tempBox.Max.Y = tempBox.Max.Y * m_Scaling.;*/

	//tempBox.Min = tempBox.Min * ;
	return tempBox;
}