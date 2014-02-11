/* Filename:  example01.cpp

   Author:  Daniel Lioi

   Date:  December 2002

   Modified:  June 2003

   This is a Direct3D program that demonstrates some simple uses of Direct X.
   It draws a rotating pyramid.  Note that you stop the program by using
   ALT F4.

   Tested with:
      Microsoft Visual C++ 6.0, Visual C++ .NET, and the Direct X 9 SDK.

   Detailed explanation:

   Program execution pretty much begins at the WinMain function so we should
   too.  Take a look at the WinMain function in example one.  The program
   starts, we declare variables to store messages, the wndclass structure,
   and a handle to the window.  We put some data into the wndclass structure
   we declared.  Then we register that class, create the window, show the
   window and update it.  On the way we check for errors.

   The first function we use is init3D.  The function receives a handle to
   the main window that was created in WinMain.  First we declare a structure
   of type D3DPRESENT_PARAMETERS which will be used to store information about
   how we want direct3d to work.  We use a helper function called
   Direct3DCreate8 which retrieves the Direct3D8 interface.  Then set some
   values in our data structure and call create device.  If create device
   fails we change the screen size and attempt to create the device in
   software mode.  The device is the rendering system.  HAL (Hardware
   Abstraction Layer) uses the device as a software interface to the hardware.
   With HAL, software and hardware are mixed to create the best performance.
   REF is software only and much slower.  If we are running in software we
   disable dithering, if we are running in HAL we enable it.  Dithering is a
   higher quality technique for blending colors which requires more processing
   time.  Finally we disable lighting.  

   If you change HAL to SW it will create an error for you and will run in REF
   mode (in case you would like to see it just to get an idea what it looks
   like).  REF runs much slower than HAL.  There exist software rendering
   systems that outperform Direct3D but they are much simpler.  We are only
   including this code in case HAL doesn't work.  The code would be
   significantly changed if we intended the program to run in REF.

   The next function that is run is initData.  This function creates a vertex
   buffer, locks it, copies vertex data to it, and then unlocks it.  This type
   of vertex contains x, y, and z data, as well as a DWORD used for the color.
   This color component is known as Diffuse.

   After initData we enter a continuous loop.  This look continues as long as
   you don't press Alt+F4 to close the program. That's coded internally as
   part of Windows.  The function render is called repeatedly.

   The first thing render() does is to call the doMath function.  This function
   sets up the vertices for the world transform which is applied to our
   pyramid, as well as the view transform and the projection transform.  The
   view transform affects from where we are looking and which direction is up.
   The projection transform affects the viewing angle we are looking with, as
   well as the aspect ratio between width and height.

   Back to the render function.  We clear the target (the back buffer actually).
   Then we begin the scene, set the vertex stream source, set the vertex
   shader, which tells the device what kind of information the vertices
   contain, draw the primitives, end the scene, and finally call present which
   flips the back buffer to the front buffer.

   Back buffers and front buffers work like this:  On the screen is the front
   or primary buffer.  The computer draws the back buffer.  Then when it's
   done they switch.  By using this process you don't ever see an incomplete image.

   When the program ends the cleanup function is called.  This function
   releases interfaces and data that we created during execution.
*/


#define D3D_OVERLOADS
#define INITGUID

#include <windows.h>
#include <d3dx9.h>

LPDIRECT3D9 lpD3D9 = NULL;      // will store a pointer to the Direct3D8 object
      // which always exists as part of the directX runtime on the computer

LPDIRECT3DDEVICE9 lpD3DDevice9 = NULL;      // will store a pointer to the Direct3DDevice8
      // which is created.  Think of this as the object that is and controls
      // the portion of (or the entire) screen.

LPDIRECT3DVERTEXBUFFER9 lpD3DVertBuffer = NULL;      // a vertex buffer, kinda new to Direct3D
      // all vertices will be stored here along with any data such as color
      // in previous versions you had to create your own storage space
      // this buffer class handles it now, a good thing :-)

struct CUSTOMVERTEX   // temp storage for vertices that will be put into a vertex buffer
{     // this could/would be used as a destination for file loading of data
      // this vertex is quite simple, no textures...
    FLOAT x, y, z;   // position
    DWORD color;     // color
};


#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)   // FVF means Flexible Vertex Format
      // all this define does is put together two properties that are sent to the vertex shader...
      // for rendering basically

#define GREEN 0xff006600   // this is useless.. I put this in so I could easily
      // modify the color green seen in the pyramid

float thetaY = 0.0f;   // this is the angle in radians through which the pyramid is rotated..
      // I used this to keep the code simple and let the user/coder see the pyramid rotate..
      // a better example would allow for user input or a timer..
      // since its not timer based.. the spin speed will depend on the CPU..
      // this as well as Frames per Second are NOT locked, they will run as fast as possible


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
      //  cleanup...
      PostQuitMessage(0);   // post quit message
      break;
   }

   return DefWindowProc(hWnd, uMsg, wParam, lParam);   // send the rest to def..
}


//*******
// this function initializes Direct3D... 
bool init3D(HWND hWnd)
{
    D3DPRESENT_PARAMETERS d3dpp;   // d3d present parameters..details on how it should present
         // a scene.. such as swap effect.. size.. windowed or full screen.. 

    // create D3D8.. if error (like that ever happens..) return false..
    if (NULL == (lpD3D9 = Direct3DCreate9(D3D_SDK_VERSION)))
        return false;
   
   ZeroMemory(&d3dpp, sizeof(d3dpp));        // blank the memory for good measure..
   //3dpp.Windowed = true;                   // use this for window mode..
   d3dpp.Windowed = false;                   // use this for full screen... its that easy!
   d3dpp.BackBufferWidth = 800;              // rather useless for Windowed version
   d3dpp.BackBufferHeight = 600;             // ditto..but left it in anyway for full screen
   d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;    // flip using a back buffer...easy performance
   d3dpp.BackBufferFormat = D3DFMT_R5G6B5;   // just set back buffer format from display mode
   d3dpp.BackBufferCount = 1;
   d3dpp.EnableAutoDepthStencil = true;
   d3dpp.AutoDepthStencilFormat =  D3DFMT_D16;   // 16 bit depth buffer..

   // Create the D3DDevice
   if (FAILED(lpD3D9->CreateDevice(D3DADAPTER_DEFAULT,   // which display adapter..
               D3DDEVTYPE_HAL,
               hWnd, D3DCREATE_MIXED_VERTEXPROCESSING,   // mixed, software, or hardware..
               &d3dpp, &lpD3DDevice9)))                  // sends stuff.. and to receive
   {  //  if it fails to create with HAL, then try (usually succeeds) to create
      //  with ref (software) using a much lower screen res
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
      {   
         
         // disable dithering for software..
         lpD3DDevice9->SetRenderState( D3DRS_DITHERENABLE, false);
      }
   }
   else
   {   
      // enable dithering for hardware... 
      lpD3DDevice9->SetRenderState( D3DRS_DITHERENABLE, true);
   }

   // turn off lighting since we have color values and no light sources exist
   lpD3DDevice9->SetRenderState( D3DRS_LIGHTING, false );
   return true;
}


bool initData()
{ 
   CUSTOMVERTEX verts[] =   // this is just data...
   {                  // it could be on a disk or such.. but this is easier
      {  0.0f,  0.75f,  0.0f, 0x00cccccc, },   // x, y, z, color
      { -1.0f, -0.50f, -1.0f, GREEN - 0x00002200, },
      { -1.0f, -0.50f,  1.0f, GREEN, },
      {  1.0f, -0.50f,  1.0f, GREEN - 0x00002200, },
      {  1.0f, -0.50f, -1.0f, GREEN, },
      { -1.0f, -0.50f, -1.0f, GREEN - 0x00002200, },    
   };
 
   if (FAILED(lpD3DDevice9->CreateVertexBuffer(sizeof(verts),   // create a vertex buffer..
              0,                     // type and processing style.. none for this 
              D3DFVF_CUSTOMVERTEX,   // use our defined properties..
              D3DPOOL_DEFAULT,       // memory class to place the resource..
              &lpD3DVertBuffer,      // place its stored..
              NULL)))
   {
      return false;   // on error return false
   }

   VOID * pVertices;   // stores pointer to the data portion of the vertex buffer
   if (FAILED(lpD3DVertBuffer->Lock(0, sizeof(verts), (VOID**) &pVertices, 0 )))
      return false;   // gets and locks data portion of vert buffer

   memcpy(pVertices, verts, sizeof(verts));   // copys mem..
   lpD3DVertBuffer->Unlock();   // unlocks vert buffer.. VERY IMPORTANT!!!

   return true;
}


void doMath()
{
   thetaY += 0.01f;   // increase rotation.. could go anywhere
            // sinces it's global.. but it's math so I put it here for now
   D3DXMATRIX matWorld;      // matrix World..stores the world movements
   D3DXMatrixRotationY( &matWorld, thetaY);   //  creates rotation for world..
   lpD3DDevice9->SetTransform( D3DTS_WORLD, &matWorld );   // sets it..

   D3DXMATRIX matView;   // this is the view matrix..
   D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f, 1.50f, -6.0f ),   // from point..
                                 &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ),     // to point..
                                 &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );   // world up..
   lpD3DDevice9->SetTransform( D3DTS_VIEW, &matView );                 // sets above

   D3DXMATRIX matProj;
   // set 90 degree view field..height/width aspect(1.3333)..near plane..(1.0f)..far plane (100.0f)
   // everything beyond the far plane is clipped.. you don't see it
   D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 1.3333f, 1.0f, 100.0f );

   lpD3DDevice9->SetTransform( D3DTS_PROJECTION, &matProj );   // set the view field
}


void render()
{
   doMath();   // do the math.. :-P
    
   // Clear the back buffer to a black... values r g b are 0-256
   lpD3DDevice9->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(256, 256, 256), 1.0f, 0);

   // Begin the scene
   lpD3DDevice9->BeginScene();

   lpD3DDevice9->SetFVF( D3DFVF_CUSTOMVERTEX );
   lpD3DDevice9->SetStreamSource( 0, lpD3DVertBuffer, 0, sizeof(CUSTOMVERTEX));   // set vertex stream..
   lpD3DDevice9->SetVertexShader( NULL);     // set vertex shader options
   lpD3DDevice9->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 4 );   // triangle fan.... 4 triangles..
   // the 0 above is the start vertex.. usually 0..
   // a triangle fan.. the first point is special.. the rest of the points form a "fan"
   // around that point.  so... 1 2 and 3 are connected... then 1 3 4.. 1 4 5..
         
   // End the scene
   lpD3DDevice9->EndScene();

   // present the back buffer.. or "flip" the page
   lpD3DDevice9->Present( NULL, NULL, NULL, NULL );   // these options are for using rectangular
      // regions for rendering/drawing...
      // 3rd is which target window.. NULL makes it use the currently set one (default)
      // last one is NEVER used.. that happens with DirectX often
}


void cleanup()   // it's a dirty job.. but some function has to do it...
{
   if ( lpD3DVertBuffer != NULL )        
      lpD3DVertBuffer->Release();

   if ( lpD3DDevice9 != NULL ) 
      lpD3DDevice9->Release();

   if ( lpD3D9 != NULL )       
      lpD3D9->Release();
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
   hWnd = CreateWindow("D3D9PROG", "Direct3D 9 Program",   // class and caption
            WS_VISIBLE | WS_POPUP,  // style
            0,                      // horizontal start..
            0,                      // vertical start..
            640,                    // horz end
            480,                    // vertical end..
            GetDesktopWindow(),     // parent..
            NULL,                   // menu..
            hInstance,              // instance
            NULL);                  // params..
   
   if (! hWnd)
      return false;

   ShowCursor(FALSE);
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


