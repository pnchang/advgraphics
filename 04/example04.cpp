/* Filename:  example04.cpp

   Author:  Daniel Lioi

   Date:  December 2002

   Modified:  June 2003

   This is a Direct3D program that shows 5 rotating cubes with textures.

   Tested with:
      Microsoft Visual C++ 6.0, Visual C++ .NET, and the Direct X 9 SDK.

   Detailed explanation:

   Example 4 introduces textures.  First of all the class has been redesigned.
   It no longer uses an index buffer, it has a list of triangle vertices.
   Each vertex has its position as well as a texture position.  The texture
   position is mapped to the vertex.  

   The constructor for the new class takes a pointer to a device and a texture.
   The only major change in the render function is the call to SetTexture().

   The initData function creates a texture and sends it to each of the five
   objects it creates.  The render function renders each of those objects.
   Cleanup cleans up those 5 objects.
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
LPDIRECT3DTEXTURE9 lpD3DTex1 = NULL;

//  five pointers to Rect3D2 objects
Rect3D2 * myRect1;
Rect3D2 * myRect2;
Rect3D2 * myRect3;
Rect3D2 * myRect4;
Rect3D2 * myRect5;


//*******
// a simple message handler.. keyboard input would go here for simple stuff.. 
// or it could go into a directInput section.. 
// if you are daring, try learning DirectInput and use that for input
// for now it just processes WM_DESTROY to kill the program
//*******
LRESULT CALLBACK WinProc(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam)
{
   switch(uMsg)             // switch for messages..
   {
   case WM_KEYFIRST:
	if (wParam==VK_ESCAPE)
		PostQuitMessage(0);   // post quit message
	break;
   case WM_DESTROY:         // on WM_DESTROY message
      // cleanup...
      PostQuitMessage(0);   // post quit message
      break;
   }

   return DefWindowProc(hWnd, uMsg, wParam, lParam);   // send the rest to def..
}


//*******
// this function initializes Direct3D... 
bool init3D(HWND hWnd)
{
   D3DDISPLAYMODE d3ddm;   // Direct3D Display Mode..
   D3DPRESENT_PARAMETERS d3dpp;    // d3d present parameters..details on how it should present
         // a scene.. such as swap effect.. size.. windowed or full screen.. 
   HFONT fnt;

   // create D3D9.. if error (like that ever happens..) return false..
    if (NULL == (lpD3D9 = Direct3DCreate9(D3D_SDK_VERSION)))
        return false;
   // get display adapter mode.. if error return false..
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
   d3dpp.AutoDepthStencilFormat =  D3DFMT_D16;   // 16 bit depth buffer..

   // Create the D3DDevice
   if (FAILED(lpD3D9->CreateDevice(D3DADAPTER_DEFAULT,   // which display adapter..
               D3DDEVTYPE_HAL,   
               hWnd, D3DCREATE_MIXED_VERTEXPROCESSING,   // mixed, software, or hardware..
                    &d3dpp, &lpD3DDevice9)))             // sends stuff.. and to receive
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

   // cull counter clockwise.. triangles drawn counterclockwise from the
   // view will not be rendered.. this is rather normal culling
   lpD3DDevice9->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
   // turn off lighting since we have color values..
   // and no light sources exist..
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

   return true;
}



bool initData()
{ 
   // creates a texture from file with default options
   D3DXCreateTextureFromFile( lpD3DDevice9, "tex1.bmp", &lpD3DTex1 );

   // initialize objects and give them data..
   myRect1 = new Rect3D2(lpD3DDevice9, lpD3DTex1);
   myRect1->setPosition(2.0f, 0.0f, 0.0f);
   myRect1->setPitch(0.0, 1.0f, 0.0f); 
   myRect1->setRoll(0.0f, 0.0f, 0.0f);
   myRect1->setYaw(0.0f, 1.0f, 0.0f);
   myRect1->setSize(1.0f, 1.0f, 1.0f);

   myRect2 = new Rect3D2(lpD3DDevice9, lpD3DTex1);
   myRect2->setPosition(-2.0f, 0.0f, 0.0f);
   myRect2->setPitch(0.0f, 1.0f, 0.0f);
   myRect2->setRoll(0.0f, 0.0f, 0.0f);
   myRect2->setYaw(0.0f, -1.0f, 0.0f);
   myRect2->setSize(1.0f, 1.0f, 1.0f);

   myRect3 = new Rect3D2(lpD3DDevice9, lpD3DTex1);
   myRect3->setPosition(0.0f, 0.0f, 0.0f);
   myRect3->setPitch(0.0f, 1.0f, 0.0f);
   myRect3->setRoll(0.0f, 0.0f, 0.0f);
   myRect3->setYaw(0.0f, 0.0f, 0.0f);
   myRect3->setSize(1.0f, 1.0f, 1.0f);

   myRect4 = new Rect3D2(lpD3DDevice9, lpD3DTex1);
   myRect4->setPosition(0.0f, 2.0f, 0.0f);
   myRect4->setPitch(0.0f, 1.0f, 0.0f);
   myRect4->setRoll(0.0f, -1.0f, 0.0f);
   myRect4->setYaw(0.0f, 0.0f, 0.0f);
   myRect4->setSize(1.0f, 1.0f, 1.0f);

   myRect5 = new Rect3D2(lpD3DDevice9, lpD3DTex1);
   myRect5->setPosition(0.0f, -2.0f, 0.0f);
   myRect5->setPitch(0.0f, 1.0f, 0.0f);
   myRect5->setRoll(0.0f, 1.0f, 0.0f);
   myRect5->setYaw(0.0f, 0.0f, 0.0f);
   myRect5->setSize(1.0f, 1.0f, 1.0f);

   return true;
}


void doMath()
{
   D3DXMATRIX matView;   // this is the view matrix..
   D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f, 1.50f, -6.0f ),    // from point..
                                 &D3DXVECTOR3(0.0f, 0.0f, 0.0f ),       // to point..
                                 &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );    // world up..
   lpD3DDevice9->SetTransform( D3DTS_VIEW, &matView );                  // sets above
 
   D3DXMATRIX matProj; 
   // set 90 degree view field..height/width aspect(1.3333)..near plane..(1.0f)..far plane (100.0f)
   // everything beyond the far plane is clipped.. you don't see it
   D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 1.3333f, 1.0f, 100.0f );

   lpD3DDevice9->SetTransform( D3DTS_PROJECTION, &matProj );   // set the view field
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
   lpD3DDevice9->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER ,D3DCOLOR_XRGB(256, 256, 256), 1.0f, 0);

   // render the cubes
   myRect1->render(clock());
   myRect2->render(clock());
   myRect3->render(clock());
   myRect4->render(clock());
   myRect5->render(clock());

   // this function writex a formatted string to a character string
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
}


void cleanup()   // it's a dirty job.. but some function has to do it...
{
   if (myRect1)
      delete myRect1;
   if (myRect2)
      delete myRect2;
   if (myRect3)
      delete myRect3;
   if (myRect4)
      delete myRect4;
   if (myRect5)
      delete myRect5;

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
   hWnd = CreateWindow("D3D9PROG", "Direct3D 8 Program",   // class and caption
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
   } while (WM_QUIT != msg.message);

   cleanup();   // clean it up!
   return msg.wParam;   // more win stuff..
}


