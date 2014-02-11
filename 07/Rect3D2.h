/* Filename:  Rect3D2.h

   Author:  Daniel Lioi

   Date:  December 2002

   Modified:  June 2003

   This file accompanies example07.cpp.
*/

#include <d3dx9.h>

class Rect3D2
{
public:
   Rect3D2(LPDIRECT3DDEVICE9 dev, LPDIRECT3DTEXTURE9 tex);   // default constructor
   ~Rect3D2();   // default destructor
   
   void setPosition(float x, float y, float z);
   void setSize(float x, float y, float z);

   void setYaw( float y, float dy = 0, float ddy = 0);
   void setPitch( float x, float dx = 0, float ddx = 0);
   void setRoll( float z, float dz = 0, float ddz = 0);

   void render(DWORD curTime);

private:
   LPDIRECT3DDEVICE9 m_device;
   LPDIRECT3DVERTEXBUFFER9 m_vertBuffer;
   LPDIRECT3DTEXTURE9 m_texture;
   static LPDIRECT3DVERTEXBUFFER9 m_defaultVB;
   static DWORD m_objectCount;
   float m_posX, m_posY, m_posZ;
   float m_width, m_height, m_depth;
   float m_yaw, m_pitch, m_roll;
   float m_dYaw, m_dPitch, m_dRoll;
   float m_ddYaw, m_ddPitch, m_ddRoll;
   DWORD m_oldTime;
};


