REM Visual Studio 2005
cl /D"_WINDOWS" /I"C:\Program Files\Microsoft DirectX SDK (June 2010)\Include"  example01.cpp /link /out:example01.exe gdi32.lib user32.lib Advapi32.lib d3d9.lib d3dx9.lib  /LIBPATH:"C:\Program Files\Microsoft DirectX SDK (June 2010)\Lib\x86"
