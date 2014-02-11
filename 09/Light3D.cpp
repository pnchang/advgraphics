/* Filename:  Light3D.cpp

   Author:  Daniel Lioi

   Date:  December 2002

   Modified:  June 2003

   This file accompanies example09.cpp.
*/

#include "Light3D.h"


Light3D::Light3D(LPDIRECT3DDEVICE9 dev, DWORD type, DWORD num)
{
   m_device = dev;                              // set device
   ZeroMemory( &m_light, sizeof(D3DLIGHT9) );   // zero a light structure
   if (type < 4)
      m_light.Type = (_D3DLIGHTTYPE) type;      // set light to selected value
   else
      m_light.Type = D3DLIGHT_POINT;            // if out of range, set default as point

   m_num = num;                // store which light number this is
   m_light.Diffuse.r = 1.0f;   // set some default light values...
   m_light.Diffuse.g = 1.0f;
   m_light.Diffuse.b = 1.0f;
   m_light.Ambient.r = 0.25f;
   m_light.Ambient.g = 0.25f;
   m_light.Ambient.b = 0.25f;
   m_light.Range = 1000.0f;    // default distance..
}


Light3D::~Light3D()
{
   m_device->LightEnable( m_num, false );   // disable light, descrutor not really necessary
}


void Light3D::setDiffuse(float r, float g, float b)   // sets values
{
   m_light.Diffuse.r = r;
   m_light.Diffuse.g = g;
   m_light.Diffuse.b = b;
}


void Light3D::setAmbient(float r, float g, float b)   // sets values
{
   m_light.Ambient.r = r;
   m_light.Ambient.g = g;
   m_light.Ambient.b = b;
}


void Light3D::setSpecular(float r, float g, float b)   // sets values
{
   m_light.Specular.r = r;
   m_light.Specular.g = g;
   m_light.Specular.b = b;
}


void Light3D::setRange(float d)   // sets the range
{
   m_light.Range = d;
}


void Light3D::setPosition(float x, float y, float z)   // sets the light position
{
   m_light.Position = D3DXVECTOR3(x, y, z);
}


void Light3D::setDirection(float dx, float dy, float dz)   // light direction (not needed for point)
{
   D3DXVECTOR3 vecDir;
   vecDir = D3DXVECTOR3(dx, dy, dz);
   D3DXVec3Normalize( (D3DXVECTOR3*) &m_light.Direction, &vecDir );
}


void Light3D::aimAt(float x, float y, float z)   // aims the light at a point/direction relative
{                            // to the light's current position
   float dx, dy, dz;
   D3DXVECTOR3 vecDir;
   dx = x - m_light.Position.x;
   dy = y - m_light.Position.y;
   dz = z - m_light.Position.z;
   vecDir = D3DXVECTOR3(dx, dy, dz);
   D3DXVec3Normalize( (D3DXVECTOR3*) &m_light.Direction, &vecDir );
}


void Light3D::setSpotProps(float theta, float phi, float falloff)   // sets properties needed for spot lights
{
   m_light.Theta = theta;
   m_light.Phi = phi;
   m_light.Falloff = falloff;
}

   
void Light3D::render(bool state)   // enables or disables a light based on state
{
   m_device->SetLight( m_num, &m_light );   // set the light 
   m_device->LightEnable( m_num, state );   // enable/disable light 
}


