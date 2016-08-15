//
//  shaderprogram.hpp
//  CG_5
//
//  Created by  on 02.06.16.
//  Copyright © 2016 hsos. All rights reserved.
//

#ifndef shaderprogram_hpp
#define shaderprogram_hpp

#ifdef WIN32
    #include <windows.h>
    #include <GL/glew.h>
    #include <GL/GLUT.h>
    #include <GL/GL.h>
#else
    #include <OpenGL/OpenGL.h>
    #include <GLUT/GLUT.h>
#endif
#include <fstream>
#include <string>
#include <vector>
#include "../Header/Matrix.h"
#include "../Header/Color.h"
#include "../Header/vector.h"

class ShaderProgram{
public:
    ShaderProgram();
    ~ShaderProgram();
    bool load(const char* VertexShader, const char* FragmentShader);
    bool loadVertexShader(const char* VertexShader);
    bool loadFragmentShader(const char* FragmentShader);
    bool compile( std::string* CompileErrors = NULL);
    
    GLint getParameterID(const char* ParameterName) const;
    
    void setParameter(GLint ID, float Param);
    void setParameter(GLint ID, int Param);
    void setParameter(GLint ID, const Vector& Param);
    void setParameter(GLint ID, const Color& Param);
    void setParameter(GLint ID, const Matrix& Param);
    
    void activate() const;
    void deactivate() const;
    
private:
    GLuint m_VertexShader;
    GLuint m_FragmentShader;
    GLuint m_ShaderProgram;
	std::string m_VertexShaderString;
	std::string m_FragmentShaderString;

	std::string readFile(const char *filePath);
};

#endif /* shaderprogram_hpp */
