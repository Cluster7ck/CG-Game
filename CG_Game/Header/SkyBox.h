#ifndef SKYBOX_H
#define SKYBOX_H
/*
With tutorial http://learnopengl.com/#!Advanced-OpenGL/Cubemaps
Doesn't work like intended...
*/
#include <windows.h>
#include <GL/glew.h>
#include <GL/GLUT.h>
#include <GL/GL.h>
#include <string>
#include <vector>
#include "Matrix.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "GLError.h"
#include "Camera.h"

class SkyBox
{
public:
	SkyBox();
	SkyBox(const char* vertexShader, const char* fragmentShader);
	bool loadSkybox();
	void createCube();
	void apply() const;
	void draw(Matrix proj, Matrix view, Vector pos);
	Matrix m_Skybox;
private:
	std::vector<std::string> TextureFilenames;
	ShaderProgram m_ShaderProgram;
	Texture m_CubeMap;
	GLuint m_CubeMapTexture;
	const char* m_VertexShader;
	const char* m_FragmentShader;
	GLuint m_VertexBuffer;
	GLuint m_VertexArrayObj;
};

#endif // SKYBOX_H