REM Visual Studio 2005 / VS2010
cl /c /D"_WINDOWS" /I"C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include"  Rect3D2.cpp 
cl /c /D"_WINDOWS" /I"C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include"  example07.cpp 
link example07.obj Rect3D2.obj /out:example07.exe gdi32.lib user32.lib Advapi32.lib d3d9.lib d3dx9.lib  /LIBPATH:"C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x86"
