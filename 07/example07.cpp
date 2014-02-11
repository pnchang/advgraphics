/* Filename:  example07.cpp

   Author:  Daniel Lioi

   Date:  December 2002

   Modified:  June 2003

   Example 7 is a Direct3D program that is similar to example 6.  It shows one
   rotating cube with color and demonstrates texture shading methods and a
   little bit of state changing using keyboard input.

   Tested with:
      Microsoft Visual C++ 6.0, Visual C++ .NET, and the Direct X 9 SDK.

   Detailed explanation:

   When you run Example 7 the first thing you will notice is a message box
   giving you instructions.  The instructions give the keys to pick between
   three different kinds of texture filtering.  Note that the settings on
   some video cards can force different filtering methods to be used, it's
   best to let each program choose the filter method for itself however.
   The code for this can be found in the WinProc function, where keyboard
   input is handled.

   Point filtering is the fastest texture filter method, but also the lowest
   quality.  You'll notice it looks kind of boxy up close.  At a distance
   however you can lose texture information.  For example, consider a texture
   that has thin vertical or horizontal stripes.  At a distance a square of
   four pixels will be averaged into one pixel, which could destroy the
   existence of stripes completely and leave a single color in its place.

   Linear filtering is much more complex.  Linear filtering is like expanding
   and then blending.  Just look and you'll see.  Anisotropic is very
   similar to linear filtering except that it adds some minor corrections
   that usually occur at a distance.

   Sometimes to make a program run faster we want things to look really good
   up close, and we don't care so much how they look far away.  You'll notice
   there is a min and mag filter.  One is used for minimizing an image; the
   other is used for magnifying an image.  Note that this doesn't directly
   correspond to distance; rather it corresponds to the size of the actual
   texture image relative to its raster size on the screen.

   Below the filtering code in WinProc is some simple code to toggle lighting
   and blending on and off.  Notice that blending also requires the state
   change of culling and the z-buffer.  In an example like this we wouldn't
   actually need both a z-buffer and culling.  There is only the cube.  One
   or the other will work.  Culling is typically set to counterclockwise.
   This means that a triangle that would appear to be drawn counterclockwise
   will not be drawn.  Triangles that would appear to be drawn clockwise will
   be drawn.  If an object is clockwise when it's facing you, then when you
   rotate it away from you, it will be appearing counterclockwise.  You
   typically don't want to see the back of objects, and with a z buffer you
   wouldn't anyway, so culling keeps the back side of objects from being drawn.

   In the case of transparency, we want the back face drawn, because we want
   a glass-like cube.  We want the entire cube to be drawn.  We don't want
   the z buffer stopping parts from being draw, so that we can blend with the
   back faces.  When we turn blending or transparency off, we want to enable
   the z-buffer and culling again.
*/

#define D3D_OVERLOADS
#define INITGUID

#include <windows.h>
#include <d3dx9.h>
#include <time.h>
#include <stdio.h>
#include "Rect3D2.h"

LPDIRECT3D9 lpD3D9 = NULL;   // will store a pointer to the Direct3D9 object
      // which always exists as part of the directX runtime on the computer

LPDIRECT3DDEVICE9 lpD3DDevice9 = NULL;   // will store a pointer to the Direct3DDevice9
      // which is created.  Think of this as the object that is and controls
      // the portion of (or the entire) screen.

LPD3DXFONT lpD3DXFont = NULL;   // used for displaying text in D3D

LPDIRECT3DTEXTURE9 lpD3DTex1 = NULL;   // pointer to a texture object

//  pointer to object
Rect3D2 * myRect1;


// we added some keyboard input
LRESULT CALLBACK WinProc(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam)
{
   switch(uMsg)             // switch for messages..
   {
   case WM_DESTROY:         // on WM_DESTROY message
      // cleanup...
      PostQuitMessage(0);   // post quit message
      break;

   case WM_KEYDOWN:         // Is A Key Being Held Down?
      switch(wParam)
      {
      case VK_ESCAPE:
	PostQuitMessage(0);   // post quit message
	break;
      case '1':
         {
            lpD3DDevice9->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
            lpD3DDevice9->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
/*
            lpD3DDevice9->SetTextureStageState( 0, D3DTSS_MAGFILTER, 
               D3DTEXF_POINT );
            lpD3DDevice9->SetTextureStageState( 0, D3DTSS_MINFILTER, 
               D3DTEXF_POINT );
*/
            break;
         }   
         
      case '2':
         {
            lpD3DDevice9->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
            lpD3DDevice9->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
            break;
         }

      case '3':
         {
            lpD3DDevice9->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
            lpD3DDevice9->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
            break;
         }

      case 'B':
         {   
            DWORD val;
            lpD3DDevice9->GetRenderState(D3DRS_ALPHABLENDENABLE, &val);
            if (val)   // blending is enabled...
            {  //disable it..
               lpD3DDevice9->SetRenderState(D3DRS_ALPHABLENDENABLE , false);
               lpD3DDevice9->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
               lpD3DDevice9->SetRenderState( D3DRS_ZENABLE, true);
            }
            else
            {  // enable blending
               lpD3DDevice9->SetRenderState(D3DRS_ALPHABLENDENABLE , true);
               lpD3DDevice9->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
               lpD3DDevice9->SetRenderState( D3DRS_ZENABLE, false);
            }
            break;
         }

      case 'L':
         {
            DWORD val;
            lpD3DDevice9->GetRenderState( D3DRS_LIGHTING, &val);
            if (val)   // true... make false..
               lpD3DDevice9->SetRenderState( D3DRS_LIGHTING, false );
            else   // false.. make true
               lpD3DDevice9->SetRenderState( D3DRS_LIGHTING, true );
            break;
         }

      }  // end of wParam switch
   }  // end of the uMsg switch

   return DefWindowProc(hWnd, uMsg, wParam, lParam);   // send the rest to def..
}


//*******
// this function initializes Direct3D... 
bool init3D(HWND hWnd)
{
   D3DDISPLAYMODE d3ddm;   // Direct3D Display Mode..
   D3DPRESENT_PARAMETERS d3dpp;   // d3d present parameters..details on how it should present
         // a scene.. such as swap effect.. size.. windowed or full screen.. 
   HFONT fnt;

   // create D3D9.. if error (like that ever happens..) return false..
   if (NULL == (lpD3D9 = Direct3DCreate9(D3D_SDK_VERSION)))
      return false;
   //  get display adapter mode.. if error return false..
   if (FAILED(lpD3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
      return false;

   // NOTE:  it's possible that this example will not work as is
   //        in windowed mode due to display format and depth buffer format
   ZeroMemory(&d3dpp, sizeof(d3dpp));            // blank the memory for good measure..
   //   d3dpp.Windowed = true;                   // use this for window mode..
   d3dpp.Windowed = false;                       // use this for full screen... it's that easy!
   d3dpp.BackBufferWidth = 1024;                 // rather useless for windowed version
   d3dpp.BackBufferHeight = 768;                 // ditto..but left it in anyway for full screen
   d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;        // flip using a back buffer...easy performance
   d3dpp.BackBufferFormat = D3DFMT_R5G6B5;       // just set back buffer format from display mode
   d3dpp.EnableAutoDepthStencil = true;
   d3dpp.AutoDepthStencilFormat =  D3DFMT_D16;   //  16 bit depth buffer..

   // Create the D3DDevice
   if (FAILED(lpD3D9->CreateDevice(D3DADAPTER_DEFAULT,   // which display adapter..
              D3DDEVTYPE_HAL,  
              hWnd, D3DCREATE_MIXED_VERTEXPROCESSING,    // mixed, software, or hardware..
              &d3dpp, &lpD3DDevice9)))                   // sends stuff.. and to receive
   {  // if it fails to create with HAL, then try (usually succeeds) to create
      // with ref (software) using a much lower screen res
      d3dpp.BackBufferWidth = 320;
      d3dpp.BackBufferHeight = 240;
      d3dpp.Windowed = false;
      if (FAILED(lpD3D9->CreateDevice(D3DADAPTER_DEFAULT,
                 D3DDEVTYPE_REF,
                 hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                 &d3dpp, &lpD3DDevice9)))
      {
         return false;     
      }
      else
      {  // create smaller font for software... lower res..
         fnt = CreateFont(20, 10, 2, 0, 500, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
               CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_MODERN, 0);
         // disable dithering for software..
         lpD3DDevice9->SetRenderState( D3DRS_DITHERENABLE, false);
      }
   }
   else
   {  // create larger font for hardware.. higher res
      fnt = CreateFont(50, 22, 2, 0, 500, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_MODERN, 0);
      // enable dithering for hardware... 
      lpD3DDevice9->SetRenderState( D3DRS_DITHERENABLE, true);
   }

   lpD3DDevice9->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
   lpD3DDevice9->SetRenderState( D3DRS_ZENABLE, true);
   
   lpD3DDevice9->SetRenderState( D3DRS_LIGHTING, false );

   // create a D3DXFont from a windows font created above...
   // for use in drawing text with D3D
   D3DXCreateFont(lpD3DDevice9,  
                     50,               //Font height
                     0,                //Font width
                     FW_NORMAL,        //Font Weight
                     1,                //MipLevels
                     false,            //Italic
                     DEFAULT_CHARSET,  //CharSet
                     OUT_DEFAULT_PRECIS, //OutputPrecision
                     ANTIALIASED_QUALITY, //Quality
                     DEFAULT_PITCH|FF_DONTCARE, //PitchAndFamily
                     "Arial",          //pFacename
                     &lpD3DXFont);

   lpD3DDevice9->SetRenderState(D3DRS_ALPHABLENDENABLE , false);
   lpD3DDevice9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
   lpD3DDevice9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);

   return true;
}  // end of init3D


bool initData()
{ 
   // creates a texture from file with default options
   D3DXCreateTextureFromFile( lpD3DDevice9, "tex1.bmp", &lpD3DTex1 );

   // default blending
   lpD3DDevice9->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
   lpD3DDevice9->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );

   // initialize objects and give them data..
   myRect1 = new Rect3D2(lpD3DDevice9, lpD3DTex1);
   myRect1->setPosition(0.0f, 0.0f, 0.0f);
   myRect1->setPitch(0.0f, .20f, 0.0f);
   myRect1->setRoll(0.0f, .20f, 0.0f);
   myRect1->setYaw(0.0f, .20f, 0.0f);
   myRect1->setSize(3.0f, 3.0f, 3.0f);

   return true;
}


void doMath()
{
   D3DXMATRIX matView;   // this is the view matrix..
   D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f, 1.50f, -6.0f ),   // from point..
                                 &D3DXVECTOR3(0.0f, 0.0f, 0.0f ),      // to point..
                                 &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );   // world up..
   lpD3DDevice9->SetTransform( D3DTS_VIEW, &matView );                 // sets above
 
   D3DXMATRIX matProj; 
   // set 90 degree view field..height/width aspect(1.3333)..near plane..(1.0f)..far plane (100.0f)
   // everything beyond the far plane is clipped.. you don't see it
   D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 1.3333f, 1.0f, 200.0f );

   lpD3DDevice9->SetTransform( D3DTS_PROJECTION, &matProj );   // set the view field
}


void render()
{
   static RECT rc = {0, 0, 320, 100};   // rectangular region.. used for text drawing
   static DWORD frameCount = 0;
   static DWORD startTime = clock();
   char str[16];
   DWORD val;

   doMath();   // do the math.. :-P   
   
   frameCount++;   // increment frame count
    
   // Clear the back buffer to a black... values r g b are 0-256
   lpD3DDevice9->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(256, 256, 256), 1.0f, 0);
   
   lpD3DDevice9->GetRenderState(D3DRS_LIGHTING, &val);
   if (val)
   {
      D3DLIGHT9 light;                           // structure for light data
      ZeroMemory( &light, sizeof(D3DLIGHT9) );   // zero the mem
      light.Type = D3DLIGHT_POINT;               // set type, either SPOT, DIRECTIONAL, or POINT

      // light being put onto objects..
      light.Diffuse.r = 1.0f;
      light.Diffuse.g = 1.0f;
      light.Diffuse.b = 1.0f;
      // ambient light of the light source.. only on when light is on
      light.Ambient.r = 0.25f;
      light.Ambient.g = 0.25f;
      light.Ambient.b = 0.25f;

      // attenuation = Atten0 + Atten1 * d + Atten2 * d * d;
      // where d is distance
      // attenuation is decreasing brightness as 
      // a vertex is further away from the light source
      light.Attenuation0 = .5f;
      light.Attenuation1 = 0.10f;
      light.Attenuation2 = 0.01f;

      // spot lights and directional lights also have a direction
      // but a POINT light shines in all directions.. kinda like a light bulb
      
      light.Position = D3DXVECTOR3(0, 0, -4);

      light.Range = 15;   // after this range the light doesn't affect vertices
      lpD3DDevice9->SetLight( 0, &light );    // set the light as index 0
      lpD3DDevice9->LightEnable( 0, true );   // enable light at index 0
      
      // ambient light.. for everything.. not attached to 
      // this light, just lighting in general
      lpD3DDevice9->SetRenderState( D3DRS_AMBIENT, 0x00404040 );
   }  // end of lighting enabled code

   // render the cube
   myRect1->render(clock());

   // this function writes a formatted string to a character string
   // in this case.. it will write "Avg fps" followed by the 
   // frames per second.. with 2 decimal places
   sprintf(str, "Avg fps %.2f", (float) frameCount / ((clock() - startTime) / 1000.0f));
      
   // draw the text string..
   // lpD3DXFont->Begin();
   lpD3DXFont->DrawText(NULL, str, -1, &rc, DT_LEFT, 0xFFFFFFFF);
   // lpD3DXFont->End();
   
   // present the back buffer.. or "flip" the page
   lpD3DDevice9->Present( NULL, NULL, NULL, NULL );   // these options are for using rectangular
      // regions for rendering/drawing...
      // 3rd is which target window.. NULL makes it use the currently set one (default)
      // last one is NEVER used.. that happens with DirectX often
}   // end of render


void cleanup()   // it's a dirty job.. but some function has to do it...
{
   if (myRect1)
      delete myRect1;

   if ( lpD3DDevice9 != NULL ) 
        lpD3DDevice9->Release();

   if ( lpD3D9 != NULL )       
        lpD3D9->Release();

   if ( lpD3DXFont != NULL )
      lpD3DXFont->Release();

   if ( lpD3DTex1 != NULL )
      lpD3DTex1->Release();
}


// windows stuff.. 
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
   MSG       msg;
   WNDCLASS  wc;
   HWND      hWnd;

   // display some simple instructions to the user
   MessageBox(NULL, 
      "Press 1 for Point filtering\nPress 2 for Linear filtering\nPress 3 for Anisotropic filtering\n\nPress B to toggle blending\nPress L to toggle lighting\n",
      "Filter Instructions", NULL);

   // set up and register wndclass wc... windows stuff
   wc.style = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc = (WNDPROC) WinProc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = sizeof(DWORD);
   wc.hInstance = hInstance;
   wc.hIcon = NULL;
   wc.hCursor = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
   wc.lpszMenuName = NULL;
   wc.lpszClassName = "D3D9PROG";

   if (! RegisterClass(&wc))
   {
      // error
      return false;
   }

   //  create window
   hWnd = CreateWindow("D3D9PROG", "Direct3D 9 Program",   // class and caption
            WS_VISIBLE | WS_POPUP,   // style
            0,                       // horizontal start..
            0,                       // vertical start..
            800,                     // horz end
            600,                     // vertical end..
            GetDesktopWindow(),      // parent..
            NULL,                    // menu..
            hInstance,               // instance
            NULL);                   // params..
   
   if (! hWnd)
      return false;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   if (init3D(hWnd) != true)   // error checking...
   {   
      cleanup();
      return false;
   }
    
   if (initData() != true)
   {
      cleanup();
      return false;
   }

   do   // control loop... life of the program
   {
      if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))   // gets messages when they appear
      {
         TranslateMessage(&msg);   // standard win message stuff..
         DispatchMessage(&msg);
      }
      else   // if no message.. render
      {
         // this is where you do stuff when there aren't messages.. mainly render..
         // might rearrange and doMath and/or other stuff here...
         // depends what would be best in the future.. 
         // for now just render         
         render();
      }
   } while (WM_QUIT != msg.message);   // end of program main loop

   cleanup();   // clean it up!
   return msg.wParam;   // more win stuff..
}  // end of winMain


