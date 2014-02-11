/* Filename:  example02.cpp

   Author:  Daniel Lioi

   Date:  December 2002

   Modified:  June 2003

   This program uses OpenGL to draw the same rotating pyramid that
   was produced using Direct X in example01.

   Tested with:
      Microsoft Visual C++ 6.0 and Visual C++ .NET, both under Windows 2000.

   Detailed explanation:

   Welcome back!  Had enough yet?  I guess not as you're still here.  Example
   2 has pretty much the same result as example 1 except that example 2 is
   using OpenGL which is software only whereas Direct3D is hardware and
   software.  The result is that OpenGL is a bit slower but generally has
   higher quality.

   Again we start by looking at the WinMain function.  We create a window,
   then run init3D, initData, and enter our message loop.  Let's go look at
   init3D.  Well, there's a lot more to initialize in OpenGL.  We have an
   unsigned integer defined followed by a static data structure.  We check
   to see if we are trying full screen mode.  If we are then we try to get
   full screen mode.  I emphasize try as some hardware cannot support full
   screen mode for OpenGL.  We create a data structure of type DEVMODE, zero
   it, set some data in it, then we try to set full screen.

   Next we get a Device Context.  OpenGL has to do this because the code is
   mostly cross platform, so we have to get specific stuff for Windows on our
   own, unless we were to use glut which is a helper system for OpenGL with
   Windows but allows for less control.  Then we try to match a pixel format.
   If it works we set it.  Then we create an OpenGL rendering context.  This
   is kind of like a Device in Direct3D8, except it's a handle and not an
   object.  After creating it we activate it.

   Then we set the shade model to smooth, which makes for smooth shading.
   (Can you think of something from example 1 that was similar to this?)
   Then we set the clear color, set some info for the depth buffer, and
   enable depth testing which is explained in the code comments.  We give
   OpenGL a hint of how we want things to look.  Note that this isn't
   necessarily going to happen, it's only a hint; it will try to do that.
   We define a viewport, select a matrix, set that matrix to default.  Then
   set a perspective which is our view angle from the z axis, our view ratio,
   near and far planes.  Then we select another matrix and  default value it.

   Then we have initData, where nothing happens, since in this example
   everything was done at the top of the code.  The vertex data is used a bit
   differently in OpenGL.  Since it's not object-oriented we just kind of
   tell OpenGL what the vertices are.  Then in the render function we clear
   the depth buffer and color buffer bits, load the default for the matrices,
   and call doMath.  DoMath updates the thetaY value, tells OpenGL where to
   look at and from where.  We rotate the world some, then back in render we
   draw the arrays and flip the back buffer.

   Something important to note about the matrices is that we aren't actually
   doing the matrix calculations when we call the functions, we are setting
   them up in the rendering pipeline which will then process the vertices in
   order as it actually renders.

   Another alternative to this would be that you could do all the vertex
   calculations for position yourself prior to each scene.  This would give
   you some more control over the data.  It takes a bit of coding to write a
   good matrix math library but it's a good learning experience and helps you
   to respect what's actually going on.
*/

#include <windows.h>     // windows..
#include <gl\gl.h>       // ...OpenGL32 Library
#include <gl\glu.h>      // ...GLu32 Library
#include <gl\glaux.h>    // ...GLaux Library

bool fullscreen = false;

GLfloat verts[] = 
{//   r      g      b      x      y      z
   1.0f,   1.0f,   1.0f,   0.0f,   0.75f,   0.0f, 
   0.0f,   0.5f,   0.0f,   -1.0f,   -0.5f,   -1.0f, 
   0.0f,   0.4f,   0.0f,   -1.0f,   -0.5f,   1.0f, 
   0.0f,   0.5f,   0.0f,   1.0f,   -0.5f,   1.0f, 
   0.0f,   0.4f,   0.0f,   1.0f,   -0.5f,   -1.0f, 
   0.0f,   0.5f,   0.0f,   -1.0f,   -0.5f,   -1.0f, 
};

HDC   hDC = NULL;   // handle to device context
HGLRC hRC = NULL;   // handle to rendering context


float thetaY = 0.0f;   // this is the angle in radians through which the pyramid is rotated..
      // I used this to keep the code simple and let the user/coder see the pyramid rotate..
      // a better example would allow for user input or a timer..
      // since its not timer based.. the spin speed will depend on the CPU..
      // this as well as Frames per Second are NOT locked, they will run as fast as possible


void cleanup()   // its a dirty job.. but some function has to do it...
{
   if (fullscreen)                      // if in fullscreen..
   {
      ChangeDisplaySettings(NULL, 0);   // switch back to desktop..
      ShowCursor(TRUE);                 // show mouse cursor
   }

   if (hRC)                             // we probably have a rendering context to cleanup..
      wglDeleteContext(hRC);            // delete the RC

   return;
}


//*******
// a simple message handler.. keyboard input would go here for simple stuff.. 
// or it could go into a directInput section.. 
// if you are daring, try learning DirectInput and use that for input
// for now it just processes WM_DESTROY to kill the program
//*******
LRESULT CALLBACK WinProc(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam)
{
   switch(uMsg)            // switch for messages..
   {
   case WM_DESTROY:        // on WM_DESTROY message
      cleanup();           // cleanup...
      PostQuitMessage(0);  // post quit message
      break;
   }

   return DefWindowProc(hWnd, uMsg, wParam, lParam);   // send the rest to def..
}


//*******
// this function initializes 3d stuff
//*******
bool init3D(HWND hWnd)
{
   GLuint PixelFormat;                     // stores a matching pixel format

   static PIXELFORMATDESCRIPTOR pfd =      // gives windows info on what we want
   {
      sizeof(PIXELFORMATDESCRIPTOR),       // size
      1,                                   // version
      PFD_DRAW_TO_WINDOW |                 // must support drawing to a window
      PFD_SUPPORT_OPENGL |                 // must support OpenGL
      PFD_DOUBLEBUFFER,                    // "    "       double buffering
      PFD_TYPE_RGBA,                       // request red green blue alpha color mode
      16,                                  // color depth
      0, 0, 0, 0, 0, 0,                    // color bits..
      0,                                   // no alpha buffer
      0,                                   // no shift bit
      0,                                   // no accumulation buffer
      0, 0, 0, 0,                          // accumulation bits
      16,                                  // 16 bit z buffer (depth)  
      0,                                   // no stencil buffer
      0,                                   // no auxiliary buffer
      PFD_MAIN_PLANE,                      // main drawing layer
      0,                                   // reserved
      0, 0, 0                              // layer masks
   };

   if (fullscreen)   // try full screen if fullscreen is true
   {
      DEVMODE dmScreenSettings;                               // device mode
      memset(&dmScreenSettings, 0, sizeof(dmScreenSettings)); // zero memory
      dmScreenSettings.dmSize=sizeof(dmScreenSettings);       // tell it how big it is
      dmScreenSettings.dmPelsWidth = 800;                     // pick screen width for full screen
      dmScreenSettings.dmPelsHeight = 600;                    // pick height for full screen..
      dmScreenSettings.dmBitsPerPel = 16;                     // pick bits per pixel
      dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;  // tell it what it's getting

      // try to set to full screen with our options.. 
      if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
      {
         MessageBox(NULL, "Full screen failed, trying window mode.", "ERROR", MB_OK);
         fullscreen = false;      // set for windowed mode..
      }
   }

   if (! (hDC = GetDC(hWnd)))   // Did We Get A Device Context?
   {
      cleanup(); 
      return false; 
   }

   if (! (PixelFormat = ChoosePixelFormat(hDC, &pfd)))   // Did Windows Find A Matching Pixel Format?
   {
      cleanup();
      return false;
   }

   if (! SetPixelFormat(hDC, PixelFormat, &pfd))   // Are We Able To Set The Pixel Format?
   {
      cleanup();
      return false;
   }

   if (! (hRC = wglCreateContext(hDC)))   // Are We Able To Get A Rendering Context?
   {
      cleanup();
      return false;
   }

   if (! wglMakeCurrent(hDC, hRC))   // Try To Activate The Rendering Context
   {
      cleanup();
      return false;
   }
   
   glShadeModel(GL_SMOOTH);                 // smooth shading
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);    // Black Background
   glClearDepth(1.0f);                      // clear depth buffer

   glEnable(GL_DEPTH_TEST);                 // enable depth testing
      // this is for when drawing objects in front and behind other objects..
      // the very essense of 3d.  Sometimes however you want to disable it
      // for things such as "transparency".  An example would be glass
      // with it on, so that nothing will appear behind the glass.
   glDepthFunc(GL_LEQUAL);                  // type of depth testing
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // nice perspective calculations.. more cpu work

   glViewport(0, 0, 800, 600);              // set view port...
   glMatrixMode(GL_PROJECTION);             // select projection matrix
   glLoadIdentity();                        // set projection matrix to default

   // set the aspect ratio.. 45 degrees from center.. 1.333 is like 800/600 for the ratio
   // 0.1 is the near plane, 100.0f is the far plane
   // objects that are not between them are cut away or clipped depending on set modes
   gluPerspective(45.0f, 1.333f, 0.1f, 100.0f);

   glMatrixMode(GL_MODELVIEW);              // select modelview mat
   glLoadIdentity();                        // reset it to default

   return true;
}


bool initData()
{ 
   glInterleavedArrays(GL_C3F_V3F, 0, verts);   // tell openGL that the verts are...
   return true;
}


void doMath()
{
   thetaY -= 0.05f;   // increase rotation angle.. could go anywhere
            // since it's global.. but it's math so I put it here for now

   //  look from 0, 1.5, -6 to 0, 0, 0  world up is 0, 1, 0
   gluLookAt(0.0f, 1.5f, -6.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

   glRotatef(thetaY, 0.0f, 1.0f, 0.0f);   // rotate everything (the pyramid)
}


void render()
{
   // set up with 3 floats for color.. then 3 floats for a vertex
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   // clear buffers
   glLoadIdentity();   // reset matrices..

   doMath();   

   glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(verts) / (4 * 6));   // draws verts from item 0 to end of verts
      // is a triangle fan..

   SwapBuffers(hDC);
   return;
}


// windows stuff.. 
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
   MSG       msg;
   WNDCLASS  wc;
   HWND      hWnd;
   //  set up and register wndclass wc... windows stuff
   wc.style = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc = (WNDPROC) WinProc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = sizeof(DWORD);
   wc.hInstance = hInstance;
   wc.hIcon = NULL;
   wc.hCursor = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
   wc.lpszMenuName = NULL;
   wc.lpszClassName = "GLPROG";

   if (! RegisterClass(&wc))
   {
      // error
      return false;
   }

   // creates a "popup" window that is without borders.. unmovable..
   hWnd = CreateWindow("GLPROG", "OpenGL Program",  // class and caption
            WS_POPUP |
            WS_CLIPSIBLINGS | WS_CLIPCHILDREN,      // pretty sure this is required style..
            0,                                      // horizontal start..
            0,                                      // vertical start..
            800,                                    // horz end
            600,                                    // vertical end..
            GetDesktopWindow(),                     // parent..
            NULL,                                   // menu..
            hInstance,                              // instance
            NULL);                                  // params..
   
   if (! hWnd)
      return false;

   ShowWindow(hWnd, SW_SHOW);   // show the window..
   SetFocus(hWnd);   

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
      else                         // if no message.. render
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


