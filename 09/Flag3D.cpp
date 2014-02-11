/* Filename:  Flag3D.cpp

   Author:  Daniel Lioi

   Date:  December 2002

   Modified:  June 2003

   This file accompanies example09.cpp.
*/

#include "Flag3D.h"

// defines our vertex structure
struct CUSTOMVERTEX
{
   float x, y, z;      // vertex coord
   float nx, ny, nz;   // normal vector
   float tu1, tv1;     // The texture coordinates
   float tu2, tv2;     // the second texture's coordinates
};

// must be even... max is 120 x 120.. has something to do with vertex buffer size
// looks somewhat smooth at 8 x 8, but lighting is blocky due to a lack of many normals
#define LENGTH 32
#define WIDTH 32

// our vertex information has an XYZ component and 2 texture components
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_TEX2 | D3DFVF_NORMAL)


Flag3D::Flag3D(LPDIRECT3DDEVICE9 dev)
{
   CUSTOMVERTEX verts[LENGTH][WIDTH];   // big.. 140,000 bytes.. [x][z]
   int i, j;   // counting
   VOID * pVertices;   // stores pointer to the data portion of the vertex buffer
   VOID * pIndices;    // stores temp pointer to data portion of index buffer
   unsigned short triIndex[LENGTH - 1][WIDTH - 1][6];
   unsigned short lineIndex[2 * ((LENGTH - 1) * (WIDTH) + (LENGTH) * (WIDTH - 1))];
   unsigned short lineIndex1[LENGTH - 1][WIDTH][2];
   unsigned short lineIndex2[LENGTH][WIDTH - 1][2];
   unsigned short pointIndex[LENGTH][WIDTH];
   
   m_primitiveType = 0;   // set default primitive type
   m_device = dev;        // store device pointer

   m_textures[0] = m_textures[1] = NULL;
   m_indexBuffers[0] = m_indexBuffers[1] = m_indexBuffers[2] = NULL;

   // calculates the x and z values for the curve, y values are calculated prior to each render
   for (i = 0; i < LENGTH; i++)
   {
      for (j = 0; j < WIDTH; j++)
      {
         verts[i][j].x = (float(i - LENGTH / 2)) / (float) LENGTH;   // create x values..

         verts[i][j].tu1 = verts[i][j].tu2 = verts[i][j].x - 0.5f;   // create texture coordinates
                  // from the x values
         verts[i][j].z = (float(j - WIDTH / 2)) / (float) WIDTH;     // create z..
         
         verts[i][j].tv1 = verts[i][j].tv2 = verts[i][j].z - 0.5f;   // create tex coords.. y values..

         verts[i][j].y = 0.0f;   // 0 for now.. will be changed during execution
         verts[i][j].nx = verts[i][j].ny = verts[i][j].nz = 0.0f;    // initialize normals to 0

      }
   }

   dev->CreateVertexBuffer(sizeof(verts),   // create a vertex buffer..
                  0,                        // type and processing style.. none for this 
                  D3DFVF_CUSTOMVERTEX,      // use our defined properties..
                  D3DPOOL_DEFAULT,          // memory class to place the resource..
                  &m_vertBuffer,            // stored in member variable
                  NULL);

   if (FAILED(m_vertBuffer->Lock(0, sizeof(verts), (void**) &pVertices, 0 )))
      return ;   // gets and locks data portion of vert buffer
   memcpy(pVertices, verts, sizeof(verts));   // copies mem..
   m_vertBuffer->Unlock();   // unlocks vert buffer.. VERY IMPORTANT!!!
   pVertices = NULL;   // store null in pointer for safety

   // calculates all the indices to form the triangles for our wave.  Could be done by hand,
   // but this way you can change the LENGTH and WIDTH.
   // also very time-consuming to calculate a list of 57,600 numbers (if the flag is 120 x 120)
   for (i = 0; i < (LENGTH - 1); i++)
   {
      for (j = 0; j < (WIDTH - 1); j++)
      {
         triIndex[i][j][0] = ((i + 1) * (LENGTH)) + (j + 1);         
         triIndex[i][j][1] = ((i + 1) * (LENGTH)) + j;
         triIndex[i][j][2] = (i * (LENGTH)) + j;
         triIndex[i][j][3] = triIndex[i][j][2];
         triIndex[i][j][4] = (i * (LENGTH)) + (j + 1);
         triIndex[i][j][5] = triIndex[i][j][0];
      }
   }   

   m_device->CreateIndexBuffer(sizeof(triIndex), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT,
      &(m_indexBuffers[0]), NULL);
   if (FAILED(m_indexBuffers[0]->Lock(0, sizeof(triIndex), (void**) &pIndices, 0)))
      return;
   memcpy(pIndices, triIndex, sizeof(triIndex));
   m_indexBuffers[0]->Unlock();
   pIndices = NULL;

   // calculate all vertices to draw lines.. done in two groups then copied into one buffer
   for (i = 0; i < (LENGTH - 1); i++)
   {
      for (j = 0; j < (WIDTH); j++)
      {
         lineIndex1[i][j][0] = ((i) * (LENGTH)) + (j);
         lineIndex1[i][j][1] = ((i + 1) * (LENGTH)) + (j);
      }
   }

   for (i = 0; i < (LENGTH); i++)
   {
      for (j = 0; j < (WIDTH - 1); j++)
      {
         lineIndex2[i][j][0] = ((i) * (LENGTH)) + (j);
         lineIndex2[i][j][1] = ((i) * (LENGTH)) + (j + 1);
      }
   }

   memcpy(lineIndex, lineIndex1, sizeof(lineIndex1));
   memcpy(&(lineIndex[2 * ((LENGTH - 1) * (WIDTH))]), lineIndex2, sizeof(lineIndex2));
   m_device->CreateIndexBuffer(sizeof(lineIndex), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT,
      &(m_indexBuffers[1]), NULL);
   if (FAILED(m_indexBuffers[1]->Lock(0, sizeof(lineIndex), (void**) &pIndices, 0)))
      return;
   memcpy(pIndices, lineIndex,sizeof(lineIndex));
   m_indexBuffers[1]->Unlock();
   pIndices = NULL;

   // does point vertices, which are just counting...
   // could have also just drawn the vertex array instead, but this way
   // the program can use an array of index buffer pointers
   for (i = 0; i < LENGTH; i++)
   {
      for (j = 0; j < WIDTH; j++)
      {
         pointIndex[i][j] = i * LENGTH + j;
      }
   }

   m_device->CreateIndexBuffer(sizeof(pointIndex), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT,
      &(m_indexBuffers[2]), NULL);
   if (FAILED(m_indexBuffers[2]->Lock(0, sizeof(pointIndex), (void**) &pIndices, 0)))
      return;
   memcpy(pIndices, pointIndex, sizeof(pointIndex));
   m_indexBuffers[2]->Unlock();
   pIndices = NULL;
}


Flag3D::~Flag3D()   // cleans up the flag's buffers
{ 
   int i;
   if (m_vertBuffer != NULL)
      m_vertBuffer->Release();

   for (i = 0; i < 3; i++)
      if (m_indexBuffers[i] != NULL)
         m_indexBuffers[i]->Release();
}


void Flag3D::TogglePrimitiveType(void)   // toggles between 3 primitive types
{
   m_primitiveType++;
   if (m_primitiveType >= 3)
      m_primitiveType = 0;
}


void Flag3D::SetTexture(int num, LPDIRECT3DTEXTURE9 tex)
{
   if (num <= 2)
      m_textures[num] = tex;
   else
   {
      if (m_textures[0] == NULL)
         m_textures[0] = tex;
      else   
         m_textures[1] = tex;
   }
}


void Flag3D::render(DWORD curTime)
{
   float rads = ((curTime / 10) % 360) * (3.141592654f / 180.0f);
   CUSTOMVERTEX * ptr;   // stores pointer to the data portion of the vertex buffer
   float heights[WIDTH];
   float nz[WIDTH];
   float ny[WIDTH];
   int i, j;
   
   for (i = 0; i < WIDTH; i++)   // calculate height values
      heights[i] = sinf(rads + ((float) i * 5) / WIDTH) / 10.0f;

   for (i = 0; i < WIDTH; i++)   // makes some approximate normals
   {                  // could also do cross product of vectors to get normals
      float h = 1;
      float w = -cosf(rads + ((float) i * 5) / WIDTH);
      float d = sqrtf(w * w + h * h);
      ny[i] = (h / d);
      nz[i] = (w / d);
   }
      
   m_vertBuffer->Lock(0, LENGTH * WIDTH * sizeof(CUSTOMVERTEX), (void**) &ptr, 0);
   
   for (i = 0; i < WIDTH; i++)
   {
      for (j = 0; j < LENGTH; j++)
      {  // try this commented-out line for a funky looking flag.. 
         // the lighting isn't correct, however
         // (ptr + (i + WIDTH * j))->y = heights[i] * (((float) j) / 10.0f);

         (ptr + (i + WIDTH * j))->y = heights[i];
         (ptr + (i + WIDTH * j))->ny = ny[i];
         (ptr + (i + WIDTH * j))->nz = nz[i];
         (ptr + (i + WIDTH * j))->nx = 0;
      }
   }

   m_vertBuffer->Unlock();   // unlocks vert buffer.. VERY IMPORTANT!!!   
   
   D3DMATERIAL9 mtrl;
   ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
   mtrl.Diffuse.r = 1.0f;
   mtrl.Diffuse.g = 1.0f;
   mtrl.Diffuse.b = 1.0f;
   mtrl.Diffuse.a = 1.0f;
   mtrl.Specular.r = mtrl.Specular.g = mtrl.Specular.b = .250f;
   m_device->SetMaterial( &mtrl );   

   // set texture's stages for rendering
   m_device->SetTexture(0, m_textures[0]);
   m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
   m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
   m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
   m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
   m_device->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_MIRROR );
   m_device->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_MIRROR );

   m_device->SetTexture(1, m_textures[1]);
   m_device->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
   m_device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADD );
   m_device->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
   m_device->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );   
   m_device->SetSamplerState( 1, D3DSAMP_ADDRESSU,  D3DTADDRESS_MIRROR );
   m_device->SetSamplerState( 1, D3DSAMP_ADDRESSV,  D3DTADDRESS_MIRROR );
   
   m_device->BeginScene();
   m_device->SetStreamSource( 0, m_vertBuffer, 0, sizeof(CUSTOMVERTEX) );   // set vertex stream..
   m_device->SetFVF( D3DFVF_CUSTOMVERTEX );
   //m_device->SetVertexShader( D3DFVF_CUSTOMVERTEX );   // set vertex shader options
   m_device->SetIndices(m_indexBuffers[m_primitiveType]);
   if (m_primitiveType == 0)
      m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0 , LENGTH * WIDTH,
         0, (LENGTH - 1) * (WIDTH - 1) * 2);
   else if (m_primitiveType == 1)
      m_device->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, LENGTH * WIDTH,
         0, (LENGTH - 1) * (WIDTH) + (WIDTH - 1) * (LENGTH));
   else
      m_device->DrawIndexedPrimitive(D3DPT_POINTLIST, 0, 0, LENGTH * WIDTH, 0, LENGTH * WIDTH);
   m_device->EndScene();
}


