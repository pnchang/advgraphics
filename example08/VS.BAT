REM Visual Studio 2005
cl /c /D"_WINDOWS" /I"D:\DXSDK\include"  Wall.cpp 
cl /c /D"_WINDOWS" /I"D:\DXSDK\include"  example08.cpp 
link example08.obj Wall.obj /out:example08.exe gdi32.lib user32.lib Advapi32.lib d3d8.lib d3dx8.lib  /NODEFAULTLIB:"libci.lib" /LIBPATH:"D:\DXSDK\lib"