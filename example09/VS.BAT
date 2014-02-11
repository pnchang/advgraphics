REM Visual Studio 2005
cl /c /D"_WINDOWS" /I"D:\DXSDK\include"  Flag3D.cpp 
cl /c /D"_WINDOWS" /I"D:\DXSDK\include"  Light3D.cpp 
cl /c /D"_WINDOWS" /I"D:\DXSDK\include"  example09.cpp 
link example09.obj Flag3D.obj Light3D.obj /out:example09.exe gdi32.lib user32.lib Advapi32.lib d3d8.lib d3dx8.lib  /NODEFAULTLIB:"libci.lib" /LIBPATH:"D:\DXSDK\lib"