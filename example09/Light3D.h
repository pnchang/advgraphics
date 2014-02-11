/* Filename:  Light3D.h

   Author:  Daniel Lioi

   Date:  December 2002

   Modified:  June 2003

   This file accompanies example09.cpp.
*/

#include <d3dx8.h>

class Light3D
{
public:
   Light3D(LPDIRECT3DDEVICE8 dev, DWORD type, DWORD num);
   ~Light3D();
   void setDiffuse(float r, float g, float b);
   void setAmbient(float r, float g, float b);
   void setSpecular(float r, float g, float b);
   void setRange(float d);
   void setPosition(float x, float y, float z);
   void setDirection(float dx, float dy, float dz);
   void aimAt(float x, float y, float z);
   void setSpotProps(float theta, float phi, float falloff);
   
   void render(bool state);   // doesn't actually put anything on screen; just sets up the light

private:
   D3DLIGHT8 m_light;
   DWORD m_num;
   LPDIRECT3DDEVICE8 m_device;
};


