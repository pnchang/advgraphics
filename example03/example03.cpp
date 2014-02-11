/* Filename:  example03.cpp
   
   Author:  Daniel Lioi
   
   Date:  December 2002
   
   Modified:  June 2003

   This is a Direct3D program that shows a pair of rotating cubes.
   
   Tested with: 
      Microsoft Visual C++ 6.0, Visual C++ .NET, and the Direct X 8 SDK.
   
   Detailed explanation:

   The big addition to this Direct3D program is the Rectangle3D class.  Take a
   look at its header file.  There are variables for the position, size, yaw,
   pitch, and roll.  Yaw, pitch, and roll are a type of rotation.  Yaw is
   rotation around the y-axis, pitch is rotation about the x-axis, and roll
   is about the z axis.  The versions of these variables that are preceded
   with a d are the velocity of them, and dd is the acceleration of them.
   All member variables start with m_.  There is a double word variable
   oldTime which will store the time of the last rendered frame.  There are
   two pointer variables that will point to the vertex buffer and the index
   buffer.

   Now take a look at Rectangle3D.cpp  The first stuff you will find are some
   of the things that used to be in the primary file, like custom vertex.
   Glance through that part on your own as it's self-explanatory or will be
   explained later.  It's just data and structures.

   The constructor initializes all of the floating point variables as well as
   the oldTime variable.  Then it creates a vertex buffer, copies the vertices
   into it, then creates an index buffer, and copies the data into it.
   Indices in the index buffer refer to the vertices as array elements.  This
   way you do not have to repeat the x, y, and z values as well as the color
   value over and over, which is 16 bytes per vertex.  You can define all
   vertices and use an index buffer to refer to them.  Elements in an index
   buffer can be 2 or 4 bytes per index value.  In this example I am using
   shorts, so we have 2 bytes per index value.

   Glance at the destructor.  It just releases the buffers.  Glance over the
   next few functions.  They just set values.  The real work is done in the
   render function.  Now look at that.  First it checks to see if this is the
   first time render is being called.  It can determine that by the oldTime
   value, which is set to 0 on initialization.  If it's the first time, it sets
   the value of it, and there is no elapsed time.  For all calls after the first
   there will be a value for oldTime, and  the function will calculate the
   elapsed time in milliseconds which is the units returned by the clock
   function used when calling this render function.  Note that you could create
   simulations by picking out an array of time values and sending them to
   render, or you could use a different timing method.  Just make sure you
   send the time in milliseconds to the render function.  The function then
   calculates time elapsed and half of the time squared which is used in
   calculations a few times.  

   The physics part is rather simple.  The new yaw position is equal to the
   old yaw, plus the yaw velocity * time, plus half the yaw acceleration *
   time squared.  Take a look at the code.  Next it updates the velocity.
   This constant updating process is useful if you were to change the
   acceleration or such at a given point and wanted to continue from where
   you were.  Right now that isn't coded in, but its just a simple matter of
   adding a few setFunctions to the class.

   Next we create the three matrices we will use, one for YawPitchRoll, one
   for translation or moving, and one for scaling.  Note that the order is
   important in which you multiply them.  For example if you were to move the
   rectangle to (3, 0, 0) and then rotate it, you would be rotating it about
   the origin.  An object rotates about the origin which is its center if you
   create the object centered at the origin.  If you move it before rotating
   it, its center is still the origin, but the object is no longer at the
   origin.  Think of 4 dots on a piece of paper making a square.  Place a
   pencil point in the middle of the square and rotate it about that pencil
   point.  The square rotates since it's centered at the origin.  Now move
   the pencil point somewhere else on the paper and do the same thing.  The
   box rotates around the origin still, but it's no longer centered on the
   origin.  

   After multiplying the matrices we set the transformation.  Then we are
   ready to begin the scene.  We set the vertex stream, then the index
   stream, the vertex shader, and finally draw an indexed primitive which
   is slightly different than a regular primitive.  The parameters to
   DrawIndexedPrimitive are as follows:  The primitive type is first.  The
   next value is the lowest index value in the vertex buffer used by this
   operation.  Since we are using all vertices this is 0.  The next is the
   highest, that's 7.  (Remember that arrays start at 0.)  The next value is
   the index to start from and the last value is the number of primitives to
   draw.

   In the main program the function initData has been changed.  It just
   creates the two objects and sets data to them.  Then the render function
   of the objects is called in the render function, sending it the time in
   milliseconds.  The only other place the objects are used is in cleanup,
   which deletes the objects.

   I also added some code to calculate and display the frame rate.  See if you
   can figure that out for yourself, keeping in mind that it counts all the
   frames and counts all the time elapsed, and then uses that to get the
   average.
*/

#define D3D_OVERLOADS
#define INITGUID

#include <windows.h>
#include <d3dx8.h>
#include <time.h>
#include <stdio.h>
#include "Rectangle3D.h"

LPDIRECT3D8 lpD3D8 = NULL;   // will store a pointer to the Direct3D8 object
      // which always exists as part of the directX runtime on the computer
LPDIRECT3DDEVICE8 lpD3DDevice8 = NULL;   // will store a pointer to the Direct3DDevice8
      // which is created.  Think of this as the object that is and controls
      // the portion of (or the entire) screen.
LPD3DXFONT lpD3DXFont = NULL;   // used for displaying text in D3D

Rectangle3D * myRect1;
Rectangle3D * myRect2;


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
   ZeroMemory(&d3dpp, sizeof(d3dpp));       // blank the memory for good measure..
   //   d3dpp.Windowed=true;                // use this for window mode..
   d3dpp.Windowed = false;                  // use this for full screen... it's that easy!
   d3dpp.BackBufferWidth = 1024;            // rather useless for windowed version
   d3dpp.BackBufferHeight = 768;            // ditto..but left it in anyway for full screen
   d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;   // flip using a back buffer...easy performance
   d3dpp.BackBufferFormat = D3DFMT_R5G6B5;  // just set back buffer format from display mode
   d3dpp.EnableAutoDepthStencil = true;
   d3dpp.AutoDepthStencilFormat =  D3DFMT_D16;

   // Create the D3DDevice
   if (FAILED(lpD3D8->CreateDevice(D3DADAPTER_DEFAULT,   // which display adapter..
               D3DDEVTYPE_HAL,   
               hWnd, D3DCREATE_MIXED_VERTEXPROCESSING,   // mixed, software, or hardware..
               &d3dpp, &lpD3DDevice8)))                  // sends stuff.. and to receive
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
   // cull counter clockwise.. triangles drawn counterclockwise from the
   // view will not be rendered.. this is rather normal culling
   lpD3DDevice8->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
   // turn off lighting since we have color values..
   // and no light sources exist..
   lpD3DDevice8->SetRenderState( D3DRS_LIGHTING, false );
   
   // uses Windows create font function to create a font to use for text..
   // not too important as of now..
   // this was just the easiest way to give fps for now

   D3DXCreateFont(lpD3DDevice8, fnt, &lpD3DXFont);

   return true;
}


bool initData()
{ 
   // initialize objects and give them data..
   myRect1 = new Rectangle3D;
   myRect1->setPosition(2.0f, 0.0f, 0.0f);
   myRect1->setPitch(0.0, 1.0f, 0.0f); 
   myRect1->setRoll(0.0f, 0.0f, 0.0f);
   myRect1->setYaw(0.0f, 1.0f, 0.0f);
   myRect1->setSize(1.0f, 1.0f, 1.0f);

   myRect2 = new Rectangle3D;
   myRect2->setPosition(-2.0f, 0.0f, 0.0f);
   myRect2->setPitch(0.0f, 1.0f, 0.0f);
   myRect2->setRoll(0.0f, 0.0f, 0.0f);
   myRect2->setYaw(0.0f, -1.0f, 0.0f);
   myRect2->setSize(1.0f, 1.0f, 1.0f);

   return true;
}


void doMath()
{
   D3DXMATRIX matView;   // this is the view matrix..
   D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f, 1.50f, -6.0f ),   // from point..
                                  &D3DXVECTOR3(0.0f, 0.0f, 0.0f ),     // to point..
                                  &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );  // world up..
   lpD3DDevice8->SetTransform( D3DTS_VIEW, &matView );                 // sets above

 
   D3DXMATRIX matProj; 
   // set 90 degree view field..height/width aspect(1.3333)..near plane..(1.0f)..far plane (100.0f)
   // everything beyond the far plane is clipped.. you don't see it
   D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 1.3333f, 1.0f, 100.0f );

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
    lpD3DDevice8->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER ,D3DCOLOR_XRGB(256, 256, 256), 1.0f, 0);

   // render the cubes
   myRect1->render(clock());
   myRect2->render(clock());

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
   if (myRect1)
      delete myRect1;
   if (myRect2)
      delete myRect2;

   if ( lpD3DDevice8 != NULL ) 
        lpD3DDevice8->Release();

   if ( lpD3D8 != NULL )       
        lpD3D8->Release();

   if ( lpD3DXFont != NULL )
      lpD3DXFont->Release();
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
   
   do   //  control loop... life of the program
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


