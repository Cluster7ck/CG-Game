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

BoundingBox BoundingBox::operator*(const Matrix& M) const {
	BoundingBox temp = *this;
	temp.Min = M * temp.Min;
	temp.Max = M *  temp.Max;
	return temp;
}

void BoundingBox::draw() {
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glColor3f(1.0, 1.0, 0);		//rgb(60%,20%,60%) = violette

								//Back side
	glVertex3f(Min.X, Min.Y, Min.Z);
	glVertex3f(Max.X, Min.Y, Min.Z);

	glVertex3f(Max.X, Min.Y, Min.Z);
	glVertex3f(Max.X, Max.Y, Min.Z);

	glVertex3f(Max.X, Max.Y, Min.Z);
	glVertex3f(Min.X, Max.Y, Min.Z);

	glVertex3f(Min.X, Max.Y, Min.Z);
	glVertex3f(Min.X, Min.Y, Min.Z);

	//Left side
	glVertex3f(Min.X, Min.Y, Min.Z);
	glVertex3f(Min.X, Min.Y, Max.Z);

	glVertex3f(Min.X, Min.Y, Max.Z);
	glVertex3f(Min.X, Max.Y, Max.Z);

	glVertex3f(Min.X, Max.Y, Max.Z);
	glVertex3f(Min.X, Max.Y, Min.Z);

	//Right side
	glVertex3f(Max.X, Min.Y, Min.Z);
	glVertex3f(Max.X, Min.Y, Max.Z);

	glVertex3f(Max.X, Min.Y, Max.Z);
	glVertex3f(Max.X, Max.Y, Max.Z);

	glVertex3f(Max.X, Max.Y, Max.Z);
	glVertex3f(Max.X, Max.Y, Min.Z);

	//Front side
	glVertex3f(Max.X, Min.Y, Max.Z);
	glVertex3f(Min.X, Min.Y, Max.Z);

	glVertex3f(Max.X, Max.Y, Max.Z);
	glVertex3f(Min.X, Max.Y, Max.Z);

	glEnd();
	glEnable(GL_LIGHTING);
}

//http://www.miguelcasillas.com/?p=30
bool BoundingBox::collision(const BoundingBox& otherBox)
{
	//if (std::fabs(this->c[0] - otherBox.c[0]) > (this->r[0] + otherBox.r[0])) return false;
	//if (std::fabs(this->c[1] - otherBox.c[1]) > (this->r[1] + otherBox.r[1])) return false;
	//if (std::fabs(this->c[2] - otherBox.c[2]) > (this->r[2] + otherBox.r[2])) return false;
	//Check if Box1's max is greater than Box2's min and Box1's min is less than Box2's max
	/*return(this->Max.X > otherBox.Min.X &&
		this->Min.X < otherBox.Max.X &&
		this->Max.Y > otherBox.Min.X &&
		this->Min.Y < otherBox.Max.Y &&
		this->Max.Z > otherBox.Min.X &&
		this->Min.Z < otherBox.Max.Z);*/

	if (this->Min.X > otherBox.Max.X) return false;
	if (this->Min.Y > otherBox.Max.Y) return false;
	if (this->Min.Z > otherBox.Max.Z) return false;
	if (this->Max.X < otherBox.Min.X) return false;
	if (this->Max.Y < otherBox.Min.Y) return false;
	if (this->Max.Z < otherBox.Min.Z) return false;

	return true;

	//If not, it will return false
}

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

const BoundingBox& GameObject::getBoundingBox() const{
	return m_BoundingBox;
}

void GameObject::setBoundingBox(BoundingBox bBox) {
	m_BoundingBox = bBox;
}

ShaderProgram GameObject::getShaderProgram() {
	return m_ShaderProgram;
}

void GameObject::drawBoundingBox() {
	m_BoundingBox.draw();
}

void GameObject::loadShaders(const char* VertexShader, const char* FragmentShader) {
	std::string compileErrors;
	if (FragmentShader == NULL && VertexShader != NULL) {
		m_ShaderProgram.loadVertexShader(VertexShader);
		m_ShaderProgram.compile(&compileErrors);

	}
	else if (FragmentShader != NULL && VertexShader == NULL) {
		m_ShaderProgram.loadFragmentShader(FragmentShader);
		m_ShaderProgram.compile(&compileErrors);
	}
	else if (VertexShader != NULL && FragmentShader != NULL) {
		m_ShaderProgram.load(VertexShader, FragmentShader);
		m_ShaderProgram.compile(&compileErrors);
	}
	std::cout << compileErrors << std::endl;
}

void GameObject::setShaders(ShaderProgram shaderProgram) {
	m_ShaderProgram = shaderProgram;
}