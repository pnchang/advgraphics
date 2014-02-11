/* Filename:  Wall.h

   Author:  Daniel Lioi

   Date:  December 2002

   Modified:  June 2003

   This file accompanies example08.cpp.
*/

#include <d3dx8.h>

class Wall
{
public:
   Wall(LPDIRECT3DDEVICE8 dev, LPDIRECT3DTEXTURE8 tex, LPDIRECT3DTEXTURE8 lightmap);   // default constructor
   ~Wall();   // default destructor

   void setHeightWidth(float y, float x);   // sets size of the wall
   void incLtSize();         // makes 2nd texture bigger..
   void decLtSize();         // makes it smaller..
   void mvLtUp();            // move it up on the wall..
   void mvLtDn();            // ... down...
   void mvLtL();             // .. left..
   void mvLtR();             // .. right..
   void render();            // render
   void ltMapModulate();     // set Modulate as the op
   void ltMapModulate2x();   // set Modulate2x as the op..
   void ltMapModulate4x();   // .. modulate4x...
   void ltMapAdd();          // .. add...
   void ltMapSubtract();     // .. subtract..
   void ltMapDisable();      // disable 2nd texture

private:
   LPDIRECT3DDEVICE8         m_device;
   LPDIRECT3DVERTEXBUFFER8   m_vertBuffer;
   LPDIRECT3DTEXTURE8        m_texture;    // primary texture..
   LPDIRECT3DTEXTURE8        m_lightMap;   // 2nd texture..

   float m_posX, m_posY, m_posZ;   // position of wall
   float m_height, m_width;        // dimensions of wall
   float m_yaw, m_pitch, m_roll;   // rotations..(stays at 0, not really needed)
   float m_ltMapX, m_ltMapY;       // location of lt map on other texture 
   float m_ltMapH, m_ltMapW;       // relative size of lt map on other texture 
   bool m_updateTexCoords;         // flag for updating texture coordinates
   DWORD m_ltMapOp;                // stores the op
};


