REM Visual Studio 2005
cl /c /D"_WINDOWS" /I"D:\DXSDK\include"  Rectangle3D.cpp 
cl /c /D"_WINDOWS" /I"D:\DXSDK\include"  example03.cpp 
link example03.obj Rectangle3D.obj /out:example03.exe gdi32.lib user32.lib Advapi32.lib d3d8.lib d3dx8.lib  /NODEFAULTLIB:"libci.lib" /LIBPATH:"D:\DXSDK\lib"