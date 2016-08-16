//
//  shaderprogram.cpp
//  CG_5
//
//  Created by  on 02.06.16.
//  Copyright © 2016 hsos. All rights reserved.
//

#include "../Header/shaderprogram.h"


ShaderProgram::ShaderProgram() {
}

ShaderProgram::~ShaderProgram() {
}

bool ShaderProgram::load(const char* VertexShader, const char* FragmentShader) {
	if (!loadVertexShader(VertexShader)) {
		std::cout << "Fehler beim Lesen des VertexShader." << std::endl;
		return false;
	}

	if (!loadFragmentShader(FragmentShader)) {
		std::cout << "Fehler beim Lesen des FragmentShader." << std::endl;
		return false;
	}

	return true;
}

bool ShaderProgram::loadVertexShader(const char* VertexShader) {
	m_VertexShader = glCreateShader(GL_VERTEX_SHADER);

	m_VertexShaderString = readFile(VertexShader);
	if (m_VertexShaderString == "") {
		return false;
	}
	return true;
}

bool ShaderProgram::loadFragmentShader(const char* FragmentShader) {
	m_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	m_FragmentShaderString = readFile(FragmentShader);
	if (m_FragmentShaderString == "") {
		return false;
	}
	return true;
}

bool ShaderProgram::compile(std::string* CompileErrors) {

	// Read shaders
	const char *vertexShaderSource = m_VertexShaderString.c_str();
	const char *fragmentShaderSource = m_FragmentShaderString.c_str();

	GLint result = GL_FALSE;
	int logLength;

	std::cout << "Linking program..." << std::endl;
	m_ShaderProgram = glCreateProgram();
	// Compile vertex shader
	if (!m_VertexShaderString.empty()) {
		std::cout << "Compiling vertex shader..." << std::endl;
		glShaderSource(m_VertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(m_VertexShader);
		// Check vertex shader
		glGetShaderiv(m_VertexShader, GL_COMPILE_STATUS, &result);
		glGetShaderiv(m_VertexShader, GL_INFO_LOG_LENGTH, &logLength);
		if (result == GL_FALSE) {
			// Lässt sich ein Shader nicht kompilieren
			std::cout << "...failed." << std::endl;
			if (CompileErrors != NULL) {
				// Fehlermeldungen des Compilers in CompileErrors schreiben
				std::vector<char> vertexShaderError((logLength > 1) ? logLength : 1);
				glGetShaderInfoLog(m_VertexShader, logLength, NULL, &vertexShaderError[0]);
				*CompileErrors = std::string(&vertexShaderError[0]);
			}
			return false;
		}
		glAttachShader(m_ShaderProgram, m_VertexShader);
		std::cout << "...complete." << std::endl;
	}

	// Compile fragment shader
	if (!m_FragmentShaderString.empty()) {
		std::cout << "Compiling fragment shader..." << std::endl;
		glShaderSource(m_FragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(m_FragmentShader);
		// Check fragment shader
		glGetShaderiv(m_FragmentShader, GL_COMPILE_STATUS, &result);
		glGetShaderiv(m_FragmentShader, GL_INFO_LOG_LENGTH, &logLength);
		if (result == GL_FALSE) {
			// Lässt sich ein Shader nicht kompilieren
			std::cout << "...failed." << std::endl;
			if (CompileErrors != NULL) {
				// Fehlermeldungen des Compilers in CompileErrors schreiben
				std::vector<char> fragmentShaderError((logLength > 1) ? logLength : 1);
				glGetShaderInfoLog(m_FragmentShader, logLength, NULL, &fragmentShaderError[0]);
				*CompileErrors = std::string(&fragmentShaderError[0]);
			}
			return false;
		}
		glAttachShader(m_ShaderProgram, m_FragmentShader);
		std::cout << "...complete." << std::endl;
	}

	glLinkProgram(m_ShaderProgram);

	glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &result);
	glGetProgramiv(m_ShaderProgram, GL_INFO_LOG_LENGTH, &logLength);
	if (result == GL_FALSE) {
		// Lässt sich das Programm nicht verknüpfen
		std::cout << "...failed." << std::endl;
		if (CompileErrors != NULL) {
			// Fehlermeldungen des Compilers in CompileErrors schreiben
			std::vector<char> programError((logLength > 1) ? logLength : 1);
			glGetProgramInfoLog(m_ShaderProgram, logLength, NULL, &programError[0]);
			*CompileErrors = std::string(&programError[0]);
		}
		return false;
	}
	std::cout << "...complete." << std::endl;

	glDeleteShader(m_VertexShader);
	glDeleteShader(m_FragmentShader);

	return true;
}

GLint ShaderProgram::getParameterID(const char* ParameterName) const {
	return glGetUniformLocation(m_ShaderProgram, ParameterName);
}

void ShaderProgram::setParameter(GLint ID, float Param) {
	//void glUniform1f(GLint location, GLfloat v0);
	glUniform1f(ID, Param);
}

void ShaderProgram::setParameter(GLint ID, int Param) {
	//void glUniform1i(GLint location, GLint v0);
	glUniform1i(ID, Param);
}

void ShaderProgram::setParameter(GLint ID, const Vector& Param) {
	//void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
	glUniform3f(ID, Param.X, Param.Y, Param.Z);
}

void ShaderProgram::setParameter(GLint ID, const Color& Param) {
	//void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
	glUniform3f(ID, Param.R, Param.G, Param.B);
}

void ShaderProgram::setParameter(GLint ID, const Matrix& Param) {
	//void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	glUniformMatrix4fv(ID, 1, GL_FALSE, (GLfloat *)&Param);
}

void ShaderProgram::activate() const {
	glUseProgram(m_ShaderProgram);
}

void ShaderProgram::deactivate() const {
	glUseProgram(0);
}

std::string ShaderProgram::readFile(const char *filePath) {
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if (!fileStream.is_open()) {
		std::cout << "Could not read file " << filePath << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}