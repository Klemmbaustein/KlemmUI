#pragma once
#include <string>
#include <cstdint>

/**
* @brief
* File containing image loading functions.
*
* @see kui::image
*/

/**
 * @brief
 * Image loading functions
 */
namespace kui::image
{
	/**
	 * @brief
	 * Loads an image file, returns the uncompressed image bytes.
	 * @param File
	 * A resource path to the file to load.
	 * @param Width
	 * @param Height
	 * @param Flipped
	 * @return
	 * A pointer to the bytes containing the image data in a RGBA8888 format.
	 *
	 * @see kui::resource
	 */
	uint8_t* LoadImageBytes(std::string File, size_t& Width, size_t& Height, bool Flipped = false);

	/**
	 * @brief
	 * Frees bytes allocated by LoadImageBytes
	 * @param Bytes
	 * The bytes to free.
	 *
	 * Right now, this just calls free()
	 *
	 * @see FreeImageBytes
	 */
	void FreeImageBytes(uint8_t* Bytes);

	/**
	 * @brief
	 * Loads an image from a path, and returns it'd image ID.
	 * @param File
	 * A resource path to the image file to load.
	 * @return
	 * The image ID of the loaded image.
	 *
	 * @see kui::resource
	 * @see UnloadImage()
	 */
	unsigned int LoadImage(std::string File);

	/**
	 * @brief
	 * Creates an image object from RGBA8888 image data.
	 * @param Bytes
	 * The image bytes containing 1 byte red, 1 byte green, 1 byte blue and 1 byte alpha data.
	 * @param Width
	 * The width of the image data in pixels.
	 * @param Height
	 * The height of the image data in pixels.
	 * @return
	 * The image ID of the loaded image.
	 *
	 * @see UnloadImage()
	 */
	unsigned int LoadImage(uint8_t* Bytes, size_t Width, size_t Height);
	struct ImageInfo
	{
		unsigned int ID;
		size_t Width;
		size_t Height;
	};

	ImageInfo LoadImageWithInfo(std::string File);

	/**
	 * @brief
	 * Unloads an image created by LoadImage() from it's image ID
	 * @param ID
	 * The image ID of the image to unload.
	 *
	 * @see LoadImage()
	 */
	void UnloadImage(unsigned int ID);
}
