#include "..\Header\GameObject.h"

Vertex::Vertex() {
}

Vertex::Vertex(const Vector& p, const Vector& n, float TexS, float TexT) {
	Position = p;
	Normal = n;
	TexcoordS = TexS;
	TexcoordT = TexT;
}

BoundingBox::BoundingBox() {
}

BoundingBox::BoundingBox(const Vector& min, const Vector& max) : Min(min), Max(max) {
}

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

BoundingBox GameObject::getBoundingBox() {
	return m_BoundingBox;
}

ShaderProgram GameObject::getShaderProgram() {
	return m_ShaderProgram;
}

void GameObject::drawBoundingBox() {
	glBegin(GL_LINES);
	glColor3f(1, 1, 0);		//rgb(60%,20%,60%) = violette

	//Back side
	glVertex3f(m_BoundingBox.Min.X, m_BoundingBox.Min.Y, m_BoundingBox.Min.Z);
	glVertex3f(m_BoundingBox.Max.X, m_BoundingBox.Min.Y, m_BoundingBox.Min.Z);

	glVertex3f(m_BoundingBox.Max.X, m_BoundingBox.Min.Y, m_BoundingBox.Min.Z);
	glVertex3f(m_BoundingBox.Max.X, m_BoundingBox.Max.Y, m_BoundingBox.Min.Z);

	glVertex3f(m_BoundingBox.Max.X, m_BoundingBox.Max.Y, m_BoundingBox.Min.Z);
	glVertex3f(m_BoundingBox.Min.X, m_BoundingBox.Max.Y, m_BoundingBox.Min.Z);

	glVertex3f(m_BoundingBox.Min.X, m_BoundingBox.Max.Y, m_BoundingBox.Min.Z);
	glVertex3f(m_BoundingBox.Min.X, m_BoundingBox.Min.Y, m_BoundingBox.Min.Z);

	//Left side
	glVertex3f(m_BoundingBox.Min.X, m_BoundingBox.Min.Y, m_BoundingBox.Min.Z);
	glVertex3f(m_BoundingBox.Min.X, m_BoundingBox.Min.Y, m_BoundingBox.Max.Z);

	glVertex3f(m_BoundingBox.Min.X, m_BoundingBox.Min.Y, m_BoundingBox.Max.Z);
	glVertex3f(m_BoundingBox.Min.X, m_BoundingBox.Max.Y, m_BoundingBox.Max.Z);

	glVertex3f(m_BoundingBox.Min.X, m_BoundingBox.Max.Y, m_BoundingBox.Max.Z);
	glVertex3f(m_BoundingBox.Min.X, m_BoundingBox.Max.Y, m_BoundingBox.Min.Z);

	//Right side
	glVertex3f(m_BoundingBox.Max.X, m_BoundingBox.Min.Y, m_BoundingBox.Min.Z);
	glVertex3f(m_BoundingBox.Max.X, m_BoundingBox.Min.Y, m_BoundingBox.Max.Z);

	glVertex3f(m_BoundingBox.Max.X, m_BoundingBox.Min.Y, m_BoundingBox.Max.Z);
	glVertex3f(m_BoundingBox.Max.X, m_BoundingBox.Max.Y, m_BoundingBox.Max.Z);

	glVertex3f(m_BoundingBox.Max.X, m_BoundingBox.Max.Y, m_BoundingBox.Max.Z);
	glVertex3f(m_BoundingBox.Max.X, m_BoundingBox.Max.Y, m_BoundingBox.Min.Z);

	//Front side
	glVertex3f(m_BoundingBox.Max.X, m_BoundingBox.Min.Y, m_BoundingBox.Max.Z);
	glVertex3f(m_BoundingBox.Min.X, m_BoundingBox.Min.Y, m_BoundingBox.Max.Z);

	glVertex3f(m_BoundingBox.Max.X, m_BoundingBox.Max.Y, m_BoundingBox.Max.Z);
	glVertex3f(m_BoundingBox.Min.X, m_BoundingBox.Max.Y, m_BoundingBox.Max.Z);

	glEnd();
}