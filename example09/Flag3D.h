/* Filename:  Flag3D.h

   Author:  Daniel Lioi

   Date:  December 2002

   Modified:  June 2003

   This file accompanies example09.cpp.
*/

// flag will be 50x50 vertices

#include <d3dx8.h>
#include <math.h>


class Flag3D
{
public:
   Flag3D(LPDIRECT3DDEVICE8 dev);
   ~Flag3D();
   void TogglePrimitiveType(void);
   void SetTexture(int num, LPDIRECT3DTEXTURE8 tex);
   void render(DWORD curTime);

private:
   LPDIRECT3DDEVICE8 m_device;
   LPDIRECT3DVERTEXBUFFER8 m_vertBuffer;
   LPDIRECT3DINDEXBUFFER8 m_indexBuffers[3];   // 0 is triangles, 1 is lines, 2 is points
   LPDIRECT3DTEXTURE8 m_textures[2];           // primary texture..
   int m_primitiveType;
};


