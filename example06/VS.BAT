REM Visual Studio 2005
cl /c /D"_WINDOWS" /I"D:\DXSDK\include"  Rect3D2.cpp 
cl /c /D"_WINDOWS" /I"D:\DXSDK\include"  example06.cpp 
link example06.obj Rect3D2.obj /out:example06.exe gdi32.lib user32.lib Advapi32.lib d3d8.lib d3dx8.lib  /NODEFAULTLIB:"libci.lib" /LIBPATH:"D:\DXSDK\lib"