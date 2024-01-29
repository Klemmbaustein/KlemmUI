$build_configs = 
@(
	"Release",
	"Debug"
)

function Build-VcxProj 
{
	Param($config, $name)
	msbuild $name /p:Configuration=$config /p:Platform=x64 /p:CI_BUILD=1
}

cd Dependencies\SDL\VisualC\SDL

foreach ($config in $build_configs)
{
	Build-VcxProj -name SDL.vcxproj -config $config
}

cd ..\..\..\..

cd Dependencies\glew-cmake
cmake CMakeLists.txt
foreach ($config in $build_configs)
{
	Build-VcxProj -name libglew_static.vcxproj -config $config
}
cd ..\..\

$debug_dependencies = 
@(
	"Dependencies\glew-cmake\lib\Debug\glewd.lib",
	"Dependencies\SDL\VisualC\SDL\x64\Debug\SDL2.dll",
	"Dependencies\SDL\VisualC\SDL\x64\Debug\SDL2.lib"
)

$release_dependencies = 
@(
	"Dependencies\glew-cmake\lib\Release\glew.lib",
	"Dependencies\SDL\VisualC\SDL\x64\Release\SDL2.dll",
	"Dependencies\SDL\VisualC\SDL\x64\Release\SDL2.lib"
)

mkdir lib/Debug -f
mkdir lib/Release -f

foreach ($lib in $debug_dependencies)
{
	cp $lib "lib\Debug"
}

foreach ($lib in $release_dependencies)
{
	cp $lib "lib\Release"
}
