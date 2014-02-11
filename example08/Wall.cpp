/* Filename:  Wall.cpp

   Author:  Daniel Lioi

   Date:  December 2002

   Modified:  June 2003

   This file accompanies example08.cpp.
*/

#include "Wall.h"

// defines our vertex structure
struct CUSTOMVERTEX
{
   float x, y, z;
   float tu1, tv1;   // the texture coordinates
   float tu2, tv2;   // the second texture's coordinates
};

// the vertices of the wall, which is just a 2d square
// the first texture's coordinates
// and the second texture's coordinates
// the 2nd don't matter, they are overwritten (once it is in the vert buffer)
// before actual rendering
const CUSTOMVERTEX WALL_VERTS[] = 
{
   .5,  .5, 0,      1, 1,   1, 1,
   .5, -.5, 0,      1, 0,   1, 0,
  -.5, -.5, 0,      0, 0,   0, 0,
  -.5,  .5, 0,      0, 1,   0, 1,
};

// our vertex information has an XYZ component and 2 texture components
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_TEX2)


Wall::Wall(LPDIRECT3DDEVICE8 dev, LPDIRECT3DTEXTURE8 tex, LPDIRECT3DTEXTURE8 lightmap)
{  // init member variables..
   m_device = dev;
   m_texture = tex;
   m_width = m_height = 1;
   m_yaw = m_roll = m_pitch = 0;
   m_posX = m_posY = m_posZ = 0;
   m_lightMap = lightmap;
   m_ltMapX = m_ltMapY = .5;
   m_ltMapH = m_ltMapW = 1;
   m_updateTexCoords = true;   // texture coordinates need to be updated before rendering
   m_ltMapOp = D3DTOP_MODULATE;   // set default op

   dev->CreateVertexBuffer(sizeof(WALL_VERTS),   // create a vertex buffer..
               0,                     // type and processing style.. none for this 
               D3DFVF_CUSTOMVERTEX,   // use our defined properties..
               D3DPOOL_DEFAULT,       // memory class to place the resource..
               &m_vertBuffer);        // stored in member variable

   void * pVertices;   // stores pointer to the data portion of the vertex buffer
   m_vertBuffer->Lock(0, sizeof(WALL_VERTS), (BYTE**) &pVertices, 0 );
   memcpy(pVertices, WALL_VERTS, sizeof(WALL_VERTS));   // copies mem..
   m_vertBuffer->Unlock();   // unlocks vert buffer.. VERY IMPORTANT!!!   
}


// cleanup..
Wall::~Wall()
{
   if (m_vertBuffer)
      m_vertBuffer->Release();
}


// sets dimensions of the wall
void Wall::setHeightWidth(float y, float x)
{
   m_height = y;
   m_width = x;
}


// increase the lt map size
void Wall::incLtSize()
{
   m_ltMapH = m_ltMapW += 0.01f;   // increase the size
   if (m_ltMapH > 2.0f)            // if it gets big make it small
      m_ltMapH = m_ltMapW = 0.0f;
   m_updateTexCoords = true;       // set update flag to true
}


void Wall::decLtSize()
{
   m_ltMapH = m_ltMapW -= 0.01f;   // decrease size..
   if (m_ltMapH < 0.0f)            // if it gets small make it big
      m_ltMapH = m_ltMapW = 2.0f;   
   m_updateTexCoords = true;       // set update flag
}


void Wall::mvLtUp()
{
   m_ltMapY += 0.01f;                   // move up..
   if ((m_ltMapY - m_ltMapH / 2) > 1)   // don't let it move so high that we lose it
      m_ltMapY =- (m_ltMapH / 2);
   m_updateTexCoords = true;            // set update flag
}


void Wall::mvLtDn()
{
   m_ltMapY -= 0.01f;                   // move down
   if ((m_ltMapY + m_ltMapH / 2) < 0)   // don't lose it...
      m_ltMapY = 1 + m_ltMapH / 2;
   m_updateTexCoords = true;            // update it when rendering..
}


void Wall::mvLtL()
{
   m_ltMapX -= 0.01f;                   // move light..
   if ((m_ltMapX + m_ltMapW / 2) < 0)   // don't lose it..
      m_ltMapX = 1 + m_ltMapW / 2;
   m_updateTexCoords = true;            // update..
}


void Wall::mvLtR()
{
   m_ltMapX += 0.01f;
   if ((m_ltMapX - m_ltMapW / 2) > 1)
      m_ltMapX =- (m_ltMapW / 2);
   m_updateTexCoords = true;
}


// sets op to Modulate
void Wall::ltMapModulate()
{   
   m_ltMapOp = D3DTOP_MODULATE;
}


// sets op to modulate twice..
void Wall::ltMapModulate2x()
{
   m_ltMapOp = D3DTOP_MODULATE2X;
}


// sets op to modulate 4 times..
void Wall::ltMapModulate4x()
{
   m_ltMapOp = D3DTOP_MODULATE4X;
}


// sets op to add the textures..
void Wall::ltMapAdd()
{
   m_ltMapOp = D3DTOP_ADD;
}


// subtract 2nd texture from first..
void Wall::ltMapSubtract()
{
   m_ltMapOp = D3DTOP_SUBTRACT;
}


// disable 2nd texture
void Wall::ltMapDisable()
{
   m_ltMapOp = D3DTOP_DISABLE;
}


void Wall::render()
{
   // matrices..
   D3DXMATRIX matWorld, matTemp;
   D3DXMATRIX matRot, matTranslate, matScale;   
   // matrix stuff..
   D3DXMatrixRotationYawPitchRoll( &matRot, m_yaw, m_pitch, m_roll);
   D3DXMatrixTranslation( &matTranslate, m_posX, m_posY, m_posZ );
   D3DXMatrixScaling( &matScale, m_width, m_height, 0);
   D3DXMatrixMultiply( &matTemp, &matScale, &matRot);
   D3DXMatrixMultiply( &matWorld, &matTemp, &matTranslate);
   // set the matrix..
   m_device->SetTransform( D3DTS_WORLD, &matWorld );

   // if the lightmap (2nd texture) has been moved.. update its coordinates..
   if (m_updateTexCoords)
   {  // only if lightmap has moved or resized by user
      CUSTOMVERTEX * ptr;   // stores pointer to the data portion of the vertex buffer
      m_vertBuffer->Lock(0, sizeof(WALL_VERTS), (BYTE**) &ptr, 0 );
   
      // set the location of the light map.. a little complex due to 
      // texture coordinates.. note we are using CLAMP for this 
      ptr[0].tu2 = ptr[1].tu2 = (1 - m_ltMapX) / (m_ltMapW / 2);
      ptr[2].tu2 = ptr[3].tu2 = 1 - (m_ltMapX) / (m_ltMapW / 2);

      ptr[0].tv2 = ptr[3].tv2 = (1 - m_ltMapY) / (m_ltMapH / 2);
      ptr[1].tv2 = ptr[2].tv2 = 1 - (m_ltMapY) / (m_ltMapH / 2);
      
      m_vertBuffer->Unlock();   // unlocks vert buffer.. VERY IMPORTANT!!!
   }

   // set the first texture and its ops
   m_device->SetTexture(0, m_texture);
   m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
   m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
   m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
   m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

   // set the 2nd texture (the light map) and its ops
   m_device->SetTexture(1, m_lightMap);
   m_device->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
   m_device->SetTextureStageState( 1, D3DTSS_COLOROP,   m_ltMapOp );
   m_device->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
   m_device->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
   m_device->SetTextureStageState( 1, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
   m_device->SetTextureStageState( 1, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

   // do usual stuff
   m_device->BeginScene();
   m_device->SetStreamSource( 0, m_vertBuffer, sizeof(CUSTOMVERTEX) );   // set vertex stream..
   m_device->SetVertexShader( D3DFVF_CUSTOMVERTEX );   // set vertex shader options
   m_device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
   m_device->EndScene();
}


