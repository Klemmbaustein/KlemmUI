cd SDL\VisualC\SDL
msbuild SDL.vcxproj /p:Configuration=Release /p:Platform=x64
msbuild SDL.vcxproj /p:Configuration=Debug /p:Platform=x64

cd ..\..\..

cp SDL\VisualC\SDL\x64\Release\SDL2.dll Examples\HelloWorld\SDL2.dll
cp SDL\VisualC\SDL\x64\Release\SDL2.dll Examples\FileExplorer\SDL2.dll

cd glew-cmake\build\vc15
devenv.exe /upgrade glew_static.vcxproj

ping 127.0.0.1 10

msbuild glew_static.vcxproj /p:Configuration=Release /p:Platform=x64
msbuild glew_static.vcxproj /p:Configuration=Debug /p:Platform=x64
cd ..\..\..

 