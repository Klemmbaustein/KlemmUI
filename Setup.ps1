cd SDL\VisualC\SDL
msbuild SDL.vcxproj /p:Configuration=Release /p:Platform=x64
msbuild SDL.vcxproj /p:Configuration=Debug /p:Platform=x64

cd ..\..\..

cp SDL\VisualC\SDL\x64\Release\SDL2.dll Examples\HelloWorld\SDL2.dll
cp SDL\VisualC\SDL\x64\Release\SDL2.dll Examples\FileExplorer\SDL2.dll

cd glew-cmake
cmake CMakeLists.txt
Write-Host "--- Building glew ---"
msbuild libglew_static.vcxproj -nologo /p:Configuration=Release /p:Platform=x64
cd ..\..\..