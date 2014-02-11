/* Filename:  example08.cpp

   Author:  Daniel Lioi

   Date:  December 2002

   Modified:  June 2003

   This is a Direct3D program that shows multitexturing and light mapping on
   a wall.

   Tested with:
      Microsoft Visual C++ 6.0, Visual C++ .NET, and the Direct X 8 SDK.

   Detailed explanation:

   In example 8 we get rid of Rect3D2 and make a new class Wall.  This class
   takes two texture pointers, one for a primary texture and one for a second
   texture which is used as a light map in this example.  A light map isn't
   real lighting, it's a texture that looks like a projected light on a
   surface, typically a wall.  Examples of light maps can be found in many
   games including Quake2, Quake3, Star Trek Elite Force, and many others.
   Multitexturing is used for more than just light maps.  Other multitexturing
   uses include bump mapping, reflections, shadow mapping (which is similar to
   light mapping).

   Example 8 contains no lighting code or alpha blending code.  InitData()
   loads two textures instead of one, sets their filters, then creates myWall
   using those textures.  The render function just renders the wall instead of
   the cubes.  Cleanup deletes myWall.  The message box in WinMain has been
   changed to display the new instructions.  WinProc has been changed to call
   the set functions in our class to do various things (see code).

   In the class, the CUSTOMVERTEX is defined with an XYZ component and two sets
   of texture coordinates.  A const array of our new vertices is defined.  The
   constructor initializes variables including a flag variable.  When moving
   the 2nd texture the coordinates will have to change.  However, they aren't
   changed on every frame.  So when the coordinates are changed, the flag is
   set.  Before the class renders, the coordinates are updated if this flag is
   set to true.  The constructor sets a default position and size for the 2nd
   texture.  The values for the 2nd texture's coordinates stored from our const
   array are never used; they are overwritten on the first render pass.  They
   only serve to keep storage space when creating the vertex buffer.

   The functions to move and change the size of the 2nd texture only change
   values.  Check to make sure that the values aren't too big or too small and
   set the flag to true.  The functions to set the texture op only set a value.
   The real work is done in the render function.  If the flag is true, the
   vertex buffer is locked, the coordinates are changed in respect to the 2nd
   texture's location and size, and then the vertex buffer is unlocked.  The
   calls to SetTexture with 0 and 1 aren't actually setting a texture by
   itself; they are setting a texture in a chain.  So to change a texture to
   another texture you would still set the value of 0, as 0 is always used
   first.  The other texture stages (1, 2, etc.) are for multitexturing.  All
   textures in the chain will be used together (or ignored depending on the
   ops and args).  The ops and args are set for the texture.  

   For the light map we use address clamping.  This allows us to place a
   texture anywhere inside of a primitive.  For example, is we use values .25
   and .5 for x, our texture will appear in the 2nd fourth of the width.  If we
   use .5 and 1.5, half of the texture will appear on the right half of the
   base.  Texture coordinates range from 0 to 1.  Values outside will clip the
   texture, and values inside will cause the edge pixels to be smeared across
   the rest of the base.  See MSDN's documentation for more details.  The last
   step is to render the primitives.
*/

#define D3D_OVERLOADS
#define INITGUID

#include <windows.h>
#include <d3dx8.h>
#include <time.h>
#include <stdio.h>
#include "Wall.h"

LPDIRECT3D8 lpD3D8 = NULL;   // will store a pointer to the Direct3D8 object
      // which always exists as part of the directX runtime on the computer

LPDIRECT3DDEVICE8 lpD3DDevice8 = NULL;   // will store a pointer to the Direct3DDevice8
      // which is created.  Think of this as the object that is and controls
      // the portion of (or the entire) screen.

LPD3DXFONT lpD3DXFont = NULL;   // used for displaying text in D3D

LPDIRECT3DTEXTURE8 lpD3DTex1 = NULL;   // pointer to a texture object
LPDIRECT3DTEXTURE8 lpD3DTex2 = NULL;   // pointer for light map

//  pointer to object
Wall * myWall;


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
      case 'W':             // move the light up on the wall
         if (myWall)
            myWall->mvLtUp();
         break;

      case 'S':             // move the light down on the wall
         if (myWall)
            myWall->mvLtDn();
         break;

      case 'A':             // move the light left
         if (myWall)
            myWall->mvLtL();
         break;

      case 'D':             // move the light right
         if (myWall)
            myWall->mvLtR();
         break;

      case 'Q':             // make the light smaller
         if (myWall)
            myWall->decLtSize();
         break;

      case 'E':             // make the light bigger
         if (myWall)
            myWall->incLtSize();
         break;

      case '1':             // use MODULATE as the op
         if (myWall)
            myWall->ltMapModulate();
         break;

      case '2':             // use MODULATE2X as the op (does it twice)
         if (myWall)
            myWall->ltMapModulate2x();
         break;

      case '3':             // use MODULATE4X as the op (does it 4 times)
         if (myWall)
            myWall->ltMapModulate4x();
         break;

      case '4':             // use ADD as the op (adds values of one texture to other)
         if (myWall)
            myWall->ltMapAdd();
         break;

      case '5':             // uses SUBTRACT (subtracts color values..)
         if (myWall)
            myWall->ltMapSubtract();
         break;

      case '6':             // disables 2nd texture
         if (myWall)
            myWall->ltMapDisable();
         break;

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

   // create D3D8.. if error (like that ever happens..) return false..
   if (NULL == (lpD3D8 = Direct3DCreate8(D3D_SDK_VERSION)))
      return false;
   // get display adapter mode.. if error return false..
   if (FAILED(lpD3D8->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
      return false;

   // NOTE:  it's possible that this example will not work as is
   //        in windowed mode due to display format and depth buffer format
   ZeroMemory(&d3dpp, sizeof(d3dpp));           // blank the memory for good measure..
   //   d3dpp.Windowed = true;                  // use this for window mode..
   d3dpp.Windowed = false;                      // use this for full screen... it's that easy!
   d3dpp.BackBufferWidth = 1024;                // rather useless for windowed version
   d3dpp.BackBufferHeight = 768;                // ditto..but left it in anyway for full screen
   d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;       // flip using a back buffer...easy performance
   d3dpp.BackBufferFormat = D3DFMT_R5G6B5;      // just set back buffer format from display mode
   d3dpp.EnableAutoDepthStencil = true;
   d3dpp.AutoDepthStencilFormat = D3DFMT_D16;   //  16 bit depth buffer..

   // Create the D3DDevice
   if (FAILED(lpD3D8->CreateDevice(D3DADAPTER_DEFAULT,   // which display adapter..
              D3DDEVTYPE_HAL,   
              hWnd, D3DCREATE_MIXED_VERTEXPROCESSING,    // mixed, software, or hardware..
              &d3dpp, &lpD3DDevice8)))                   // sends stuff.. and to receive
   {  //  if it fails to create with HAL, then try (usually succeeds) to create
      //  with ref (software) using a much lower screen res
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

   lpD3DDevice8->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
   lpD3DDevice8->SetRenderState( D3DRS_ZENABLE, true );
   
   lpD3DDevice8->SetRenderState( D3DRS_LIGHTING, false );

   // create a D3DXFont from a windows font created above...
   // for use in drawing text with D3D
   D3DXCreateFont(lpD3DDevice8, fnt, &lpD3DXFont);

   lpD3DDevice8->SetRenderState(D3DRS_ALPHABLENDENABLE , false);
   lpD3DDevice8->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
   lpD3DDevice8->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);

   return true;
}  // end of init3D


bool initData()
{ 
   // creates a texture from file with default options
   D3DXCreateTextureFromFile( lpD3DDevice8, "tex1.bmp", &lpD3DTex1 );
   D3DXCreateTextureFromFile( lpD3DDevice8, "tex2.bmp", &lpD3DTex2 );

   // default blending
   lpD3DDevice8->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
   lpD3DDevice8->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );

   myWall = new Wall(lpD3DDevice8, lpD3DTex1, lpD3DTex2);
   myWall->setHeightWidth(5, 5);

   return true;
}


void doMath()
{
   D3DXMATRIX matView;   // this is the view matrix..
   D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f, 0.0f, -6.0f ),     // from point..
                                 &D3DXVECTOR3(0.0f, 0.0f, 0.0f ),       // to point..
                                 &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );    // world up..
   lpD3DDevice8->SetTransform( D3DTS_VIEW, &matView );                  // sets above

   D3DXMATRIX matProj; 
   // set 90 degree view field..height/width aspect(1.3333)..near plane..(1.0f)..far plane (100.0f)
   // everything beyond the far plane is clipped.. you don't see it
   D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 1.3333f, 1.0f, 200.0f );

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
    
   // Clear the backbuffer to a black... values r g b are 0-256
   lpD3DDevice8->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(256, 256, 256), 1.0f, 0);

   // render the wall with the light map on it using the set op
   myWall->render();

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
}


void cleanup()   // it's a dirty job.. but some function has to do it...
{
   if (myWall)
      delete myWall;

   if ( lpD3DDevice8 != NULL ) 
        lpD3DDevice8->Release();

   if ( lpD3D8 != NULL )       
        lpD3D8->Release();

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
      "Q/E Light Size\nA/D Left and right\nW/S Up and down\n\nTexture Operations\n1 - Modulate\n2 - Modulate2x\n3 - Modulate4x\n4 - Add\n5 - Subtract\n6 - Disable\n",
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


