/* Filename:  Flag3D.h

   Author:  Daniel Lioi

   Date:  December 2002

   Modified:  June 2003

   This file accompanies example09.cpp.
*/

// flag will be 50x50 vertices

#include <d3dx9.h>
#include <math.h>


class Flag3D
{
public:
   Flag3D(LPDIRECT3DDEVICE9 dev);
   ~Flag3D();
   void TogglePrimitiveType(void);
   void SetTexture(int num, LPDIRECT3DTEXTURE9 tex);
   void render(DWORD curTime);

private:
   LPDIRECT3DDEVICE9 m_device;
   LPDIRECT3DVERTEXBUFFER9 m_vertBuffer;
   LPDIRECT3DINDEXBUFFER9 m_indexBuffers[3];   // 0 is triangles, 1 is lines, 2 is points
   LPDIRECT3DTEXTURE9 m_textures[2];           // primary texture..
   int m_primitiveType;
};


