#include <cstdint>
#include <string>

/**
 * @brief
 * File containing resource functions.
 */

/**
 * @brief
 * Resource namespace, for loading files on the filesystem or resources embedded into the application.
 *
 * Resources are referred to using paths.
 * A path can follow these formats:
 *
 * - `file:C:/Normal/file/path` - A filesystem path. If the path starts with `file:`
 *   the library will always only search on the filesystem for this file.
 *
 * - `res:MyResourceDir/Data` - An embedded resource path, where
 *
 * - `C:/Normal/file/path` or `MyResourceDir/Data` - A filesystem or resource path.
 *   The library will check both for a matching path, but resources have priority.
 *   (If res/test.txt is an embedded resource, and res/test.txt exists in the current
 *   working directory of the application, the embedded resource will be used)
 *
 * Currently for web builds only embedded resources work.
 */
namespace kui::resource
{
	struct BinaryData
	{
		const uint8_t* const Data = nullptr;
		const size_t FileSize = 0;
		const size_t ResourceType = SIZE_MAX;
	};

	BinaryData GetBinaryResource(const std::string& Path);
	std::string GetStringResource(const std::string& Path);

	bool ResourceExists(const std::string& Path);

	/**
	 * @brief
	 * Gets the string content of a file from the given resource path.
	 * @param Path
	 * A resource path to the file to search for.
	 * @return
	 * A string containing the file contents interpreted as a string.
	 */
	std::string GetStringFile(const std::string& Path);
	BinaryData GetBinaryFile(const std::string& Path);

	/**
	 * @brief
	 * Frees the binary file
	 * @param Data
	 */
	void FreeBinaryFile(BinaryData Data);

	/**
	 * @brief
	 * Checks if the resource file exists.
	 * @param Path
	 * @return
	 */
	bool FileExists(const std::string& Path);

	extern thread_local bool ErrorOnFail;
}