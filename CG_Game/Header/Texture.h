//
//  Texture.h
//  RealtimeRending
//
//  Created by Philipp Lensing on 24.10.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//

#ifndef __RealtimeRending__Texture__
#define __RealtimeRending__Texture__

#include <iostream>
#include <windows.h>
#include <GL/glew.h>
#include <GL/GLUT.h>
#include <GL/GL.h>

class RGBImage;

class Texture
{
public:
    Texture();
    ~Texture();
    bool LoadFromBMP( const char* Filename );
	unsigned char* LoadBMP(const char* Filename, unsigned int& width, unsigned int& height);
    void apply() const;
    bool isValid() const;
    const RGBImage* getRGBImage() const;
	const GLuint getTexID() const;
protected:
    
    RGBImage* createImage( unsigned char* Data, unsigned int width, unsigned int height );
    GLuint m_TextureID;
    RGBImage* m_pImage;
};

#endif /* defined(__RealtimeRending__Texture__) */
