gcc -fpermissive -static  -I"/C/Program Files/Microsoft DirectX SDK (June 2010)/Include" -L"/C/Program Files/Microsoft DirectX SDK (June 2010)/lib/x86" -o example09G.exe example09.cpp Flag3D.cpp Light3D.cpp -ld3d9 -ld3dx9 -lstdc++ -mwindows -fno-exceptions