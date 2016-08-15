//
//  PlaneModel.cpp
//  RealtimeRending
//
//  Created by Philipp Lensing on 25.11.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//

#include "../Header/PlaneModel.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


PlaneModel::PlaneModel()
{

    
    
}

PlaneModel::~PlaneModel()
{
    
}

bool PlaneModel::load(const char* tex1, const char* tex2)
{
    PlaneVertex Vertices[4] =
    {
        { Vector(-1,0,-1), Vector(0,1,0), 0, 0, 0, 0 },
        { Vector(-1,0,1), Vector(0,1,0), 0, 1, 0, 4 },
        { Vector(1,0,1), Vector(0,1,0), 1, 1, 4, 4 },
        { Vector(1,0,-1), Vector(0,1,0), 1, 0, 4, 0 }
    };
    unsigned int Indices[6] = { 0,1,2,0,2,3 };
    
    // create gpu buffer for vertices
    glGenBuffers( 1, &m_VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PlaneVertex)*4, Vertices, GL_STATIC_DRAW );
    // create gpu buffer for indices
    glGenBuffers( 1, &m_IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*6, Indices, GL_STATIC_DRAW);
    
    if(!m_GrassTex.LoadFromBMP(tex1))
        return false;
    
    if(!m_SandTex.LoadFromBMP(tex2))
        return false;
    
    return true;
}


void PlaneModel::draw()
{
    // inform the client that we want to use array buffers
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // setup position & normal pointers
    glVertexPointer( 3, GL_FLOAT, sizeof(PlaneVertex), BUFFER_OFFSET(0));
    glNormalPointer( GL_FLOAT, sizeof(PlaneVertex), BUFFER_OFFSET(12));

    // setup texture-environment-unit 0 => grass
    glActiveTexture(GL_TEXTURE0);
    glClientActiveTexture(GL_TEXTURE0);
    glTexCoordPointer(2, GL_FLOAT, sizeof(PlaneVertex), BUFFER_OFFSET(24)); // first uv-set
    m_GrassTex.apply();
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    // setup texture-environment-unit 1 => sand
    glActiveTexture(GL_TEXTURE1);
    glClientActiveTexture(GL_TEXTURE1);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(PlaneVertex), BUFFER_OFFSET(32)); // second uv-set
    m_SandTex.apply();
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    // setup texture-environment-unit 2 => grass*sand
    glActiveTexture(GL_TEXTURE2);
    glClientActiveTexture(GL_TEXTURE2);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(PlaneVertex), BUFFER_OFFSET(24)); // is obsolete, but we have to supply it
    m_GrassTex.apply(); // we dont use the actual texture but we have to supply a texture to the unit
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvf( GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
    glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE0); // arg0 is texture from unit 0
    glTexEnvf(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_TEXTURE1); // arg1 is texture from unit 1
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);

    // we draw our plane
    glDrawElements( GL_TRIANGLES, 6 , GL_UNSIGNED_INT, BUFFER_OFFSET(0) );

    // disable states in reverse order
    glDisable(GL_TEXTURE_2D);
    
    glActiveTexture(GL_TEXTURE1);
    glClientActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);
    
    glActiveTexture(GL_TEXTURE0);
    glClientActiveTexture(GL_TEXTURE0);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // set modulate as default behaviour for unit 0
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}