/* Filename:  Rectangle3D.cpp

   Author:  Daniel Lioi

   Date:  December 2002

   Modified:  June 2003

   This file accompanies example03.cpp.
*/

#include "Rectangle3D.h"

extern LPDIRECT3DDEVICE8 lpD3DDevice8;

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)   // FVF means Flexible Vertex Format
      // all this define does is put together two properties that are sent to the vertex shader...
      // for rendering basically

struct CUSTOMVERTEX   // temp storage for vertices that will be put into a vertex buffer
{   // this could/would be used as a destination for file loading of data
    // this vertex is quite simple, no textures...
    FLOAT x, y, z;    // position
    DWORD color;      // color
};

// the vertices and color for the cube
const CUSTOMVERTEX CUBE_VERTS2[] = 
{
   0.5f,   0.5f,   0.5f, 0xFFFFFFFF,
   0.5f,   0.5f,  -0.5f, 0xFF00FF00,
   0.5f,  -0.5f,   0.5f, 0xFFFF0000,
   0.5f,  -.5f,   -0.5f, 0xFF0000FF,
   
  -0.5f,   0.5f,   0.5f, 0xFF0000FF,
  -0.5f,   0.5f,  -0.5f, 0xFFFF0000,
  -0.5f,  -0.5f,   0.5f, 0xFF00FF00,
  -0.5f,  -0.5f,  -0.5f, 0xFFFFFFFF
};

// indices for the cube
// insides are the order of points used to draw each triangle
// each number is an index of one of the vertices above
const short CUBE_INDICES[] = 
{
   3, 1, 0,   // right
   0, 2, 3,

   4, 5, 7,   // left
   7, 6, 4,

   1, 3, 5,
   3, 7, 5,   //front

   4, 2, 0,
   4, 6, 2,   // back

   0, 1, 5,   //top
   0, 5, 4,

   7, 2 ,6,   //bottom
   7, 3, 2
};


Rectangle3D::Rectangle3D()
{
   // set default values..
   m_ddPitch  = m_ddRoll = m_ddYaw  = 0.0f;
   m_dPitch   = m_dRoll  = m_dYaw   = 0.0f;
   m_pitch    = m_roll   = m_yaw    = 0.0f;
   m_depth    = m_height = m_width  = 1.0f;
   m_posX     = m_posY   = m_posZ   = 0.0f;
   m_oldTime  = 0;

   lpD3DDevice8->CreateVertexBuffer(sizeof(CUBE_VERTS2),   // create a vertex buffer..
                 0,                     // type and processing style.. none for this 
                 D3DFVF_CUSTOMVERTEX,   // use our defined properties..
                 D3DPOOL_DEFAULT,       // memory class to place the resource..
                 &m_vertBuffer);        // stored in member variable

   VOID * pVertices;   // stores pointer to the data portion of the vertex buffer
   if (FAILED(m_vertBuffer->Lock(0, sizeof(CUBE_VERTS2), (BYTE**) &pVertices, 0 )))
      return ;   // gets and locks data portion of vert buffer
   memcpy(pVertices, CUBE_VERTS2, sizeof(CUBE_VERTS2));   // copies mem..
   m_vertBuffer->Unlock();   // unlocks vert buffer.. VERY IMPORTANT!!!

   // create index buffer... it refers to indices in the
   // vertex buffer, created almost the same as vertex buffer
   lpD3DDevice8->CreateIndexBuffer(sizeof(CUBE_INDICES), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_indexBuffer);
   VOID * pIndices;
   if (FAILED(m_indexBuffer->Lock(0, sizeof(CUBE_INDICES), (BYTE**) &pIndices, 0)))
      return;
   memcpy(pIndices, CUBE_INDICES, sizeof(CUBE_INDICES));
   m_indexBuffer->Unlock();
}


Rectangle3D::~Rectangle3D()
{  // cleanup
   m_indexBuffer->Release();
   m_vertBuffer->Release();
}


void Rectangle3D::setPosition(float x, float y, float z)
{   
   m_posX = x; 
   m_posY = y; 
   m_posZ = z; 
}


void Rectangle3D::setSize(float x, float y, float z)
{      
   m_width = x; 
   m_height = y; 
   m_depth = z; 
}


// sets yaw, this is rotation around the y axis
void Rectangle3D::setYaw( float y, float dy, float ddy)
{   
   m_yaw = y; 
   m_dYaw = dy; 
   m_ddYaw = ddy; 
}


// sets pitch, this is rotation around the x axis
void Rectangle3D::setPitch( float x, float dx, float ddx)
{   
   m_pitch = x;
   m_dPitch = dx;
   m_ddPitch = ddx; 
}


// sets roll, this is rotation around z axis
void Rectangle3D::setRoll( float z, float dz, float ddz)
{   
   m_roll = z; 
   m_dRoll = dz; 
   m_ddRoll = ddz; 
}


void Rectangle3D::render(DWORD curTime)
{
   DWORD elapsedTime;
   D3DXMATRIX matWorld, matTemp;
   D3DXMATRIX matRot, matTranslate, matScale;
   float t, halfTSqrd;

   //  calculate time elapsed and store current time for next cycle      
   if (m_oldTime == 0)
   {
      m_oldTime = curTime;
      elapsedTime = 0;
   }
   else
   {
      elapsedTime = curTime - m_oldTime;
      m_oldTime = curTime;
   }   
   
   t = elapsedTime * 0.001f;    //  convert time to seconds and store
   halfTSqrd = 0.5f * t * t;    //  store half of time squared

   // calculate rotations
   // d = d + v * t + (1/2)at^2
   // v = v + at;
   m_yaw += m_dYaw * t + halfTSqrd * m_ddYaw;
   m_dYaw += m_ddYaw * t;

   m_pitch += m_dPitch * t + halfTSqrd * m_ddPitch;
   m_dPitch += m_ddPitch * t;

   m_roll += m_dRoll * t + halfTSqrd * m_ddRoll;
   m_dRoll += m_ddRoll * t;


   // MATRICES   these take effect for drawing primitives until they are reset
   // so to draw objects with different orientation, just
   // use the matrices, draw, then reset again
   // in the case of multiple objects
   // each object sets the matrices for its use, renders, then the next sets..
   // renders.. and so on
   D3DXMatrixRotationYawPitchRoll( &matRot, m_yaw, m_pitch, m_roll);
   D3DXMatrixTranslation( &matTranslate, m_posX, m_posY, m_posZ );
   D3DXMatrixScaling( &matScale, m_width, m_height, m_depth );

   D3DXMatrixMultiply( &matTemp, &matScale, &matRot);
   D3DXMatrixMultiply( &matWorld, &matTemp, &matTranslate);

   lpD3DDevice8->SetTransform( D3DTS_WORLD, &matWorld );

   // SCENE RENDERING
   // Begin the scene
   lpD3DDevice8->BeginScene();

   lpD3DDevice8->SetStreamSource( 0, m_vertBuffer, sizeof(CUSTOMVERTEX) );   // set vertex stream..
   lpD3DDevice8->SetIndices(m_indexBuffer, 0);                           //  set the index buffer stream..
   lpD3DDevice8->SetVertexShader( D3DFVF_CUSTOMVERTEX );                 // set vertex shader options
   lpD3DDevice8->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 8, 0, 12);  // draw indexed triangle list

   lpD3DDevice8->EndScene();
}


