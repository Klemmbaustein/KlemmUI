# Generates and compiles a project for the web using emscripten.
# Example usage (to compile the "HelloWorld" example):
# 
# PS KlemmUI/> ./Util/WebBuild/webBuild.ps1 --target HelloWorld
# 
# This will place the compiled files in KlemmUI/out/web/
# and some generated files in KlemmUI/out/web_tools and KlemmUI/out/resources
#
# For this script to work, the emscripten environment variables have to be set.
# 

param (

	# The CMake file to build from. If not set, this is "CMakeLists.txt" (in the current directory)
	[string] $file,

	# The CMake target that should be compiled.
	[Parameter(Mandatory=$true)]
	[string] $target,

	# A prefix to the path where the generated output files should be placed.
	# For example: "-outputPath hello" -> files will be placed in ./hello/out/web/
	[string] $outputPath,

	# If this is set, the script does not compile the emscripten CMake project it generates.
	[switch] $noCompile,

	# Addional options passed to CMake when configuring the emscripten CMake project.
	[string] $configOptions,

	# Additional options passed to "cmake --build" when compiling the project. Has no effect with -noCompile.
	[string] $compileOptions
)

if ([string]::IsNullOrWhitespace($file))
{
	$file = "CMakeLists.txt"
}

if ([string]::IsNullOrWhitespace($outputPath))
{
	$outputPath = "."
}

write-host $outputPath

write-host "Compiling resources and ui headers..."

$toolsDir = "${outputPath}/out/web_tools"
$outDir = "${outputPath}/out/web"
$resourceDir = "${outputPath}/out/resources"

write-host "Compiling in ${toolsDir}"

# Build the resources and headers first, using the native tools for this platform.
# These can't be built using the emscripten sdk because they need to run during the build to generate required files.

cmake $file -B $toolsDir
cmake --build $toolsDir --target ${target}_Resources
cmake --build $toolsDir --target KlemmUI_Resources
cmake --build $toolsDir --target ${target}_Headers

$emsdkToolchainFile = "${env:EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"

mkdir $resourceDir/headers -Force

function Copy-OfType
{
	param(
		[string] $pattern,
		[string] $path
	)

	return (Get-ChildItem -Exclude $path/resources -Path $path -Recurse -Filter $pattern).FullName 
}

# Copy the generated header and resource files to the $resourceDir for the web build to use.

$resources = Copy-OfType -path $toolsDir -pattern "*_res.c"
foreach ($res in $resources)
{
	Copy-Item $res $resourceDir -Force
}

$headers = Copy-OfType -path $toolsDir -pattern "*.kui.hpp"
foreach ($hdr in $headers)
{
	Copy-Item $hdr ${resourceDir}/headers -Force
}

# Configure and compile the actual emscripten CMake project using the generated header and resource files from before.

write-host Toolchain at: $emsdkToolchainFile

cmake $file -B $outDir -DCMAKE_TOOLCHAIN_FILE="${emsdkToolchainFile}" -DKLEMMUI_WEB=ON -DKLEMMUI_WEB_RESOURCE_PATH="${resourceDir}" -G Ninja $configOptions

if (-not $noCompile)
{
	cmake --build $outDir --target $target $compileOptions
}