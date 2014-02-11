REM Visual Studio 2005
cl /c /D"_WINDOWS" /I"C:\Program Files\Microsoft DirectX SDK (June 2010)\Include"  Flag3D.cpp 
cl /c /D"_WINDOWS" /I"C:\Program Files\Microsoft DirectX SDK (June 2010)\Include"  Light3D.cpp 
cl /c /D"_WINDOWS" /I"C:\Program Files\Microsoft DirectX SDK (June 2010)\Include"  example09.cpp 
link example09.obj Flag3D.obj Light3D.obj /out:example09.exe gdi32.lib user32.lib Advapi32.lib d3d9.lib d3dx9.lib  /LIBPATH:"C:\Program Files\Microsoft DirectX SDK (June 2010)\Lib\x86"
