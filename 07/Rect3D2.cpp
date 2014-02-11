/* Filename:  Rect3D2.cpp

   Author:  Daniel Lioi

   Date:  December 2002

   Modified:  June 2003

   This file accompanies example07.cpp.
*/

#include "Rect3D2.h"

struct CUSTOMVERTEX
{
   float x, y, z;
   float nx, ny, nz;   // normal vector
   DWORD color;
   float tu, tv;   // the texture coordinates
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_NORMAL | D3DFVF_DIFFUSE)

// the vertices... normal vector...  color...texture coord
// 1296 bytes

const CUSTOMVERTEX CUBE_VERTS2[] = 
{
   -0.5f, 0.5f, 0.5f,     -1, 0, 0,   0x6000FF00,         1, 1,
   -0.5f, 0.5f, -0.5f,    -1, 0, 0,   0x600000FF,         1, 0,
   -0.5f, -0.5f, 0.5f,    -1, 0, 0,   0x60FFFFFF,         0, 1,
   -0.5f, -0.5f, -0.5f,   -1, 0, 0,   0x60FF0000,         0, 0,   
   -0.5f, -0.5f, 0.5f,    -1, 0, 0,   0x60FFFFFF,         0, 1,
   -0.5f, 0.5f, -0.5f,    -1, 0, 0,   0x600000FF,         1, 0,   

   0.5f, 0.5, 0.5,         1, 0, 0,   0x60FF0000,         1, 1,      
   0.5, -0.5, 0.5,         1, 0, 0,   0x600000FF,         0, 1,
   0.5, 0.5, -0.5,         1, 0, 0,   0x60FFFFFF,         1, 0,      
   0.5, -0.5, -0.5,        1, 0, 0,   0x6000FF00,         0, 0,
   0.5, 0.5, -0.5,         1, 0, 0,   0x60FFFFFF,         1, 0,      
   0.5, -0.5, 0.5,         1, 0, 0,   0x600000FF,         0, 1,

   0.5, 0.5, 0.5,          0, 1, 0,   0x60FF0000,         1, 1,
   0.5, 0.5, -0.5,         0, 1, 0,   0x60FFFFFF,         0, 1,      
   -0.5, 0.5, 0.5,         0, 1, 0,   0x6000FF00,         1, 0,
   -0.5, 0.5, -0.5,        0, 1, 0,   0x600000FF,         0, 0,
   -0.5f, 0.5, 0.5,        0, 1, 0,   0x6000FF00,         1, 0,
   0.5, 0.5, -0.5,         0, 1, 0,   0x60FFFFFF,         0, 1,      

   0.5,  -0.5, 0.5,        0, -1, 0,  0x600000FF,         1, 1,
   -0.5, -0.5, 0.5,        0, -1, 0,  0x60FFFFFF,         1, 0,
   0.5,  -0.5, -0.5,       0, -1, 0,  0x6000FF00,         0, 1,
   -0.5, -0.5, -0.5,       0, -1, 0,  0x60FF0000,         0, 0,
   0.5,  -0.5, -0.5,       0, -1, 0,  0x6000FF00,         0, 1,
   -0.5f, -0.5, 0.5,       0, -1, 0,  0x60FFFFFF,         1, 0,

   0.5, 0.5,  -0.5,        0, 0, -1,  0x60FFFFFF,         1, 1,
   0.5, -0.5, -0.5,        0, 0, -1,  0x6000FF00,         0, 1,
   -0.5, 0.5, -0.5,        0, 0, -1,  0x600000FF,         1, 0,
   -0.5, -0.5, -0.5,       0, 0, -1,  0x60FF0000,         0, 0,
   -0.5, 0.5, -0.5,        0, 0, -1,  0x600000FF,         1, 0,
   0.5, -0.5, -0.5,        0, 0, -1,  0x6000FF00,         0, 1,

   0.5, 0.5,  0.5,         0, 0, 1,   0x60FF0000,         1, 1, 
   -0.5, 0.5, 0.5,         0, 0, 1,   0x6000FF00,         1, 0,
   0.5, -0.5, 0.5,         0, 0, 1,   0x600000FF,         0, 1, 
   -0.5, -0.5, 0.5,        0, 0, 1,   0x60FFFFFF,         0, 0, 
   0.5, -0.5, 0.5,         0, 0, 1,   0x600000FF,         0, 1,
   -0.5, 0.5, 0.5,         0, 0, 1,   0x6000FF00,         1, 0,
};


LPDIRECT3DVERTEXBUFFER9 Rect3D2::m_defaultVB = NULL;
DWORD Rect3D2::m_objectCount = 0;

Rect3D2::Rect3D2(LPDIRECT3DDEVICE9 dev, LPDIRECT3DTEXTURE9 tex)
{
   m_texture = tex;   // set the texture...
   m_device = dev;    // set the device instead of using the global
   // set default values..
   m_ddPitch = m_ddRoll = m_ddYaw = 0.0f;
   m_dPitch  = m_dRoll  = m_dYaw  = 0.0f;
   m_pitch   = m_roll   = m_yaw   = 0.0f;
   m_depth   = m_height = m_width = 1.0f;
   m_posX    = m_posY   = m_posZ  = 0.0f;
   m_oldTime = 0;

   // in this example we made a static vertex buffer that
   // can be shared between ALL objects created from this class
   // the first object will initialize it
   // the last one will delete it
   // each object still has its own pointer..
   // this is in case we decide to make a way to attach a different vertex buffer
   // to an object which would be 
   // created and deleted outside of the class
   m_objectCount++;

   if (m_defaultVB)
      m_vertBuffer = m_defaultVB;
   else
   {
      dev->CreateVertexBuffer(sizeof(CUBE_VERTS2),   // create a vertex buffer..
                     0,                     // type and processing style.. none for this 
                     D3DFVF_CUSTOMVERTEX,   // use our defined properties..
                     D3DPOOL_DEFAULT,       // memory class to place the resource..
                     &m_vertBuffer,         // stored in member variable
                     NULL);

      void * pVertices;   // stores pointer to the data portion of the vertex buffer
      m_vertBuffer->Lock(0, sizeof(CUBE_VERTS2), (void**) &pVertices, 0 );
      memcpy(pVertices, CUBE_VERTS2, sizeof(CUBE_VERTS2));   // copies mem..
      m_vertBuffer->Unlock();   // unlocks vert buffer.. VERY IMPORTANT!!!
      m_defaultVB = m_vertBuffer;
   }
}


Rect3D2::~Rect3D2()
{   
   m_objectCount--;
   // if this is the last object.. delete the vertex buffer
   if (m_objectCount == 0)
   {
      m_defaultVB->Release();
      m_defaultVB = NULL;
   }
}


void Rect3D2::setPosition(float x, float y, float z)
{   
   m_posX = x; 
   m_posY = y; 
   m_posZ = z; 
}


void Rect3D2::setSize(float x, float y, float z)
{      
   m_width = x; 
   m_height = y; 
   m_depth = z; 
}


// sets yaw, this is rotation around the y axis
void Rect3D2::setYaw( float y, float dy, float ddy)
{   
   m_yaw = y; 
   m_dYaw = dy; 
   m_ddYaw = ddy; 
}


// sets pitch, this is rotation around the x axis
void Rect3D2::setPitch( float x, float dx, float ddx)
{   
   m_pitch = x;
   m_dPitch = dx;
   m_ddPitch = ddx; 
}


// sets roll, this is rotation around z axis
void Rect3D2::setRoll( float z, float dz, float ddz)
{   
   m_roll = z; 
   m_dRoll = dz; 
   m_ddRoll = ddz; 
}


void Rect3D2::render(DWORD curTime)
{
   DWORD elapsedTime;
   D3DXMATRIX matWorld, matTemp;
   D3DXMATRIX matRot, matTranslate, matScale;
   float t, halfTSqrd;
   DWORD val;

   // if lighting is enabled set up the material 
   m_device->GetRenderState(D3DRS_LIGHTING, &val);
   if (val)
   {  // these options determine how different light
      // such as diffuse and ambient affect this object..
      // if 0 it has no effect.. 1 is a good
      // value to be normal...
      D3DMATERIAL9 mtrl;
      ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
      mtrl.Diffuse.r = 1.0f;   mtrl.Ambient.r = 0.0f;
      mtrl.Diffuse.g = 1.0f;   mtrl.Ambient.g = 0.0f;
      mtrl.Diffuse.b = 1.0f;   mtrl.Ambient.b = 0.0f;
      mtrl.Diffuse.a = 1.0f;   mtrl.Ambient.a = 0.0f;
      m_device->SetMaterial( &mtrl );
   }

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
   
   t = elapsedTime * 0.001f;   // convert time to seconds and store
   halfTSqrd = 0.5f * t * t;   // store half of time squared to save future calculations

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

   m_device->SetTransform( D3DTS_WORLD, &matWorld );
   
   // for now all we have to do is set the texture
   // there are a lot of options to set for textures 
   // when different forms of pixel shading are involved, for now it always
   // looks the same
   m_device->SetTexture(0, m_texture);
   m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
   m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
   m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
   m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_BLENDDIFFUSEALPHA );
   m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
   m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

   // SCENE RENDERING
   // Begin the scene
   m_device->BeginScene();

   m_device->SetStreamSource( 0, m_vertBuffer, 0, sizeof(CUSTOMVERTEX) );   // set vertex stream..

   m_device->SetFVF( D3DFVF_CUSTOMVERTEX );
//   m_device->SetVertexShader( D3DFVF_CUSTOMVERTEX );   // set vertex shader options

   m_device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 12);

   m_device->EndScene();
}


