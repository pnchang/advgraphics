REM Visual Studio 2005
cl /D"_WINDOWS" /I"D:\DXSDK\include"  example02.cpp /link /out:example02.exe gdi32.lib user32.lib Advapi32.lib opengl32.lib glu32.lib   