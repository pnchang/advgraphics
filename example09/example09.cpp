/* Filename:  example09.cpp

   Author:  Daniel Lioi

   Date:  December 2002

   Modified:  June 2003

   This is a Direct3D program that shows a moving textured surface (flag)
   with light mapping.

   This program can use on average up to 6 MB of RAM (or more, depending on OS)
   during execution.  Taking into account that this program is rather small,
   it is worth noting how fast a large 3d program or game can fill up main
   and/or video memmory (especially if the programmer isn't careful).

   Tested with:
      Microsoft Visual C++ 6.0, Visual C++ .NET, and the Direct X 8 SDK.
*/

#define D3D_OVERLOADS
#define INITGUID

#include <windows.h>
#include <d3dx8.h>
#include <time.h>
#include <stdio.h>
#include "Flag3D.h"
#include "Light3D.h"

LPDIRECT3D8 lpD3D8 = NULL;   // will store a pointer to the Direct3D8 object
      // which always exists as part of the directX runtime on the computer

LPDIRECT3DDEVICE8 lpD3DDevice8 = NULL;   // will store a pointer to the Direct3DDevice8
      // which is created.  Think of this as the object that is and controls
      // the portion of (or the entire) screen.

LPD3DXFONT lpD3DXFont = NULL;   // used for displaying text in D3D

LPDIRECT3DTEXTURE8 lpD3DTex1 = NULL;   // pointer to a texture object
LPDIRECT3DTEXTURE8 lpD3DTex2 = NULL;   // pointer for light map
bool funkyLights = false;

//  pointers to objects
Flag3D * myFlag = NULL;
Light3D * myLights[8] = {NULL};


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
      case VK_F1:           // F1 key
         if (myFlag)
            myFlag->TogglePrimitiveType();
         break;

      case VK_F2:           // F2 key
         if (funkyLights)
            funkyLights = false;
         else
            funkyLights = true;
         break;

      }  // end of wParam switch
   }  // end of the uMsg switch

   return DefWindowProc(hWnd, uMsg, wParam, lParam);   // send the rest to def..
}


//*******
// this function initializes Direct3D... 
bool init3D(HWND hWnd)
{
   D3DDISPLAYMODE d3ddm;          // Direct3D Display Mode..
   D3DPRESENT_PARAMETERS d3dpp;   // d3d present parameters..details on how it should present
         // a scene.. such as swap effect.. size.. windowed or full screen.. 
   HFONT fnt;

   // create D3D8.. if error (like that ever happens..) return false..
   if (NULL == (lpD3D8 = Direct3DCreate8(D3D_SDK_VERSION)))
      return false;
   //  get display adapter mode.. if error return false..
   if (FAILED(lpD3D8->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
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
   if (FAILED(lpD3D8->CreateDevice(D3DADAPTER_DEFAULT,   // which display adapter..
              D3DDEVTYPE_HAL,   
              hWnd, D3DCREATE_MIXED_VERTEXPROCESSING,    // mixed, software, or hardware..
              &d3dpp, &lpD3DDevice8)))                   // sends stuff.. and to receive
   {  // if it fails to create with HAL, then try (usually succeeds) to create
      // with ref (software) using a much lower screen res
      d3dpp.BackBufferWidth = 320;
      d3dpp.BackBufferHeight = 240;
      d3dpp.Windowed = false;
      if (FAILED(lpD3D8->CreateDevice(D3DADAPTER_DEFAULT,
                 D3DDEVTYPE_REF,
                 hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                 &d3dpp, &lpD3DDevice8)))
      {
         return false;     
      }
      else
      {  // create smaller font for software... lower res..
         fnt = CreateFont(20, 10, 2, 0, 500, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
               CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_MODERN, 0);
         // disable dithering for software..
         lpD3DDevice8->SetRenderState( D3DRS_DITHERENABLE, false);
      }
   }
   else
   {  // create larger font for hardware.. higher res
      fnt = CreateFont(50, 22, 2, 0, 500, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_MODERN, 0);
      // enable dithering for hardware... 
      lpD3DDevice8->SetRenderState( D3DRS_DITHERENABLE, true);
   }

   lpD3DDevice8->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
   lpD3DDevice8->SetRenderState( D3DRS_ZENABLE, true);
   lpD3DDevice8->SetRenderState( D3DRS_LIGHTING, true );

   // create a D3DXFont from a windows font created above...
   // for use in drawing text with D3D
   D3DXCreateFont(lpD3DDevice8, fnt, &lpD3DXFont);

   lpD3DDevice8->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
   lpD3DDevice8->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
   //   lpD3DDevice8->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);
   lpD3DDevice8->SetRenderState( D3DRS_SPECULARENABLE, true );
   return true;
}  // end of init3D


bool initData()
{ 
   // creates a texture from file with default options
   D3DXCreateTextureFromFile( lpD3DDevice8, "tex1.bmp", &lpD3DTex1 );
   D3DXCreateTextureFromFile( lpD3DDevice8, "tex4.bmp", &lpD3DTex2 );

   // default blending
   lpD3DDevice8->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
   lpD3DDevice8->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
   lpD3DDevice8->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
   lpD3DDevice8->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );

   myFlag = new Flag3D(lpD3DDevice8);
   myFlag->SetTexture(0, lpD3DTex1);
   myFlag->SetTexture(1, lpD3DTex2);
   
   myLights[0] = new Light3D(lpD3DDevice8, 2, 0);
   myLights[0]->setPosition(0.0f, 6.0f, 0.0f);
   myLights[0]->aimAt(0.0f, 0.0f, 0.0f);
   myLights[0]->setRange(8.0f);
   myLights[0]->setSpotProps(.1f, .2f, .2f);
   myLights[0]->setDiffuse(.250f, .250f, .250f);

   myLights[1] = new Light3D(lpD3DDevice8, 2, 1);
   myLights[1]->setPosition(0.0, 3.0f, 0.0f);
   myLights[1]->aimAt(.1f, 0.0f, .1f);
   myLights[1]->setRange(15);
   myLights[1]->setSpotProps(.010f, 0.20f, 1.0f);
   myLights[1]->setDiffuse(1.0f, 0.0f, 0.0f);
   myLights[1]->setSpecular(1.0f, 0.0f, 0.0f);

   myLights[2] = new Light3D(lpD3DDevice8, 2, 2);
   myLights[2]->setPosition(0.0, 3.0f, 0.0f);
   myLights[2]->aimAt(-.1f, 0.0f, .1f);
   myLights[2]->setRange(15);
   myLights[2]->setSpotProps(.010f, 0.20f, 1.0f);
   myLights[2]->setDiffuse(0.0f, 1.0f, 0.0f);
   myLights[2]->setSpecular(0.0f, 1.0f, 0.0f);

   myLights[3] = new Light3D(lpD3DDevice8, 2, 3);
   myLights[3]->setPosition(0.0, 3.0f, 0.0f);
   myLights[3]->aimAt(.0f, 0.0f, -.110f);
   myLights[3]->setRange(15);
   myLights[3]->setSpotProps(.010f, 0.20f, 1.0f);
   myLights[3]->setDiffuse(0.0f, 0.0f, 1.0f);
   myLights[3]->setSpecular(0.0f, 0.0f, 1.0f);

   return true;
}  // end of initData


void doMath()
{
   float rot = ((clock() / 40) % 360) * (3.141592654f / 180.0f);
   float randomX = cosf(rot) / 5.0f;
   float randomY = sinf(rot) / 5.0f;
   D3DXMATRIX matView;   // this is the view matrix..
   D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3(1.2f * cosf(rot), 1.0f, 1.2f * sinf(rot)),   // from point..
                                 &D3DXVECTOR3(0.0f, 0.0f, 0.0f ),      // to point..
                                 &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );   // world up..
   lpD3DDevice8->SetTransform( D3DTS_VIEW, &matView );                 // sets above

   myLights[1]->aimAt(cosf(2 * rot) / 5.0f + randomX, 0.0f, sinf(rot) / 5.0f + randomY);
   myLights[2]->aimAt(cosf(2 * rot + (2.094395102f)) / 5.0f + randomX, 0.0f,
      sinf(rot + (2.094395102f)) / 5.0f + randomY);
   myLights[3]->aimAt(cosf(2 * rot + (4.188790205f)) / 5.0f + randomX, 0.0f,
      sinf(rot + (4.188790205f)) / 5.0f + randomY);
 
   D3DXMATRIX matProj; 
   // set 90 degree view field..height/width aspect(1.3333)..near plane..(1.0f)..far plane (100.0f)
   // everything beyond the far plane is clipped.. you don't see it
   D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 1.3333f, .10f, 200.0f );

   lpD3DDevice8->SetTransform( D3DTS_PROJECTION, &matProj );   // set the view field
}


void render()
{
   static RECT rc = {0, 0, 320, 100};   // rectangular region.. used for text drawing
   static DWORD frameCount = 0;
   static DWORD startTime = clock();
   char str[16];

   doMath();   // do the math.. :-P   
   
   frameCount++;   //  increment frame count
    
   // Clear the back buffer to a black... values r g b are 0-256
   lpD3DDevice8->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(256, 256, 256), 1.0f, 0);
   
   if (funkyLights == false)
      myLights[0]->render(true);
   else
      myLights[0]->render(false);

   myLights[1]->render(true);
   myLights[2]->render(true);
   myLights[3]->render(true);

   // render the wall with the light map on it using the set op
   myFlag->render(clock());

   // this function writes a formatted string to a character string
   // in this case.. it will write "Avg fps" followed by the 
   // frames per second.. with 2 decimal places
   sprintf(str, "Avg fps %.2f", (float) frameCount / ((clock() - startTime) / 1000.0f));
   
   // draw the text string..
   lpD3DXFont->Begin();
   lpD3DXFont->DrawText(str, -1, &rc, DT_LEFT, 0xFFFFFFFF);
   lpD3DXFont->End();
   
   // present the back buffer.. or "flip" the page
   lpD3DDevice8->Present( NULL, NULL, NULL, NULL );   // these options are for using rectangular
      // regions for rendering/drawing...
      // 3rd is which target window.. NULL makes it use the currently set one (default)
      // last one is NEVER used.. that happens with DirectX often
}  // end of render


void cleanup()   // it's a dirty job.. but some function has to do it...
{
   if (myFlag != NULL)
      delete myFlag;

   // lights don't really need to be cleaned up if the program is ending, they don't allocate any memmory

   if ( lpD3DDevice8 != NULL ) 
        lpD3DDevice8->Release();

   if ( lpD3D8 != NULL )       
        lpD3D8->Release();

   if ( lpD3DXFont != NULL )
      lpD3DXFont->Release();

   if ( lpD3DTex1 != NULL )
      lpD3DTex1->Release();

   if ( lpD3DTex2 != NULL )
      lpD3DTex2->Release();
}


// windows stuff.. 
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
   MSG       msg;
   WNDCLASS  wc;
   HWND      hWnd;

   // display some simple instructions to the user
   MessageBox(NULL, 
      "F1 - toggle primitive type\nF2 - Toggle white light\n",
      "Instructions", NULL);

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
   wc.lpszClassName = "D3D8PROG";

   if (! RegisterClass(&wc))
   {
      // error
      return false;
   }

   //  create window
   hWnd = CreateWindow("D3D8PROG", "Direct3D 8 Program",   // class and caption
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


