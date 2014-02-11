REM Visual Studio 2005/VS2010
cl /c /D"_WINDOWS" /I"C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include"  Wall.cpp 
cl /c /D"_WINDOWS" /I"C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include"  example08.cpp 
link example08.obj Wall.obj /out:example08.exe gdi32.lib user32.lib Advapi32.lib d3d9.lib d3dx9.lib  /LIBPATH:"C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x86"
