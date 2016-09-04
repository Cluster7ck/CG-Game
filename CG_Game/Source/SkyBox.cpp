#include "..\Header\SkyBox.h"
/*
With tutorial http://learnopengl.com/#!Advanced-OpenGL/Cubemaps
*/
SkyBox::SkyBox()
{

}

SkyBox::SkyBox(const char* VertexShader, const char* FragmentShader)
{
	m_VertexShader = VertexShader;
	m_FragmentShader = FragmentShader;

	TextureFilenames.push_back("Ressourcen/right.bmp");
	TextureFilenames.push_back("Ressourcen/left.bmp");
	TextureFilenames.push_back("Ressourcen/top.bmp");
	TextureFilenames.push_back("Ressourcen/bottom.bmp");
	TextureFilenames.push_back("Ressourcen/back.bmp");
	TextureFilenames.push_back("Ressourcen/sky2.bmp");

	createCube();
	loadSkybox();

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

bool SkyBox::loadSkybox()
{
	unsigned int width, height;
	unsigned char* image;

	glGenTextures(1, &m_CubeMapTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMapTexture);

	for (unsigned int i = 0; i < TextureFilenames.size(); i++) {
		image = m_CubeMap.LoadBMP(TextureFilenames[i].c_str(), width, height);

		if (image == NULL) {
			std::cout << "Image "<< i <<" could not be loaded" << std::endl;
			exit(5);
		}

		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	delete[] image;

	return true;
}

void SkyBox::createCube()
{
	GLfloat vertices[] =
	{         
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	/*
	for (int i = 0; i < 36 * 3; i++) {
		vertices[i] = vertices[i] * 100;
	}*/

	glGenVertexArrays(1, &m_VertexArrayObj);
	glGenBuffers(1, &m_VertexBuffer);
	glBindVertexArray(m_VertexArrayObj);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(GLfloat), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
}

void SkyBox::apply() const
{
	if (m_CubeMapTexture == 0)
		return;

	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMapTexture);
}

void SkyBox::draw(Matrix proj, Matrix view, Vector pos, Camera cam)
{
	//view.lookAt(Vector(0, 0, 0), cam.getUp(), pos);
	//Ignore depth
	glDepthMask(GL_FALSE);
	//check_gl_error();
	m_ShaderProgram.activate();
	m_ShaderProgram.setParameter(m_ShaderProgram.getParameterID("position"), pos);
	m_ShaderProgram.setParameter(m_ShaderProgram.getParameterID("view"), view);
	m_ShaderProgram.setParameter(m_ShaderProgram.getParameterID("projection"), proj);
	check_gl_error();
	glBindVertexArray(m_VertexArrayObj);
	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMapTexture);
	m_ShaderProgram.setParameter(m_ShaderProgram.getParameterID("skybox"), 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glDisable(GL_TEXTURE_CUBE_MAP);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	m_ShaderProgram.deactivate();
	glDepthMask(GL_TRUE);
}

