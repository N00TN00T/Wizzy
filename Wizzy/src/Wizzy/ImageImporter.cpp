#include "wzpch.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "Wizzy/ImageImporter.h"

namespace Wizzy
{
	namespace Image
	{
		byte* decode_image(const byte* encoded, int32 nPixels, int32& resultWidth, int32& resultHeight, int32& resultChannels, int32 desiredChannels)
		{
			return stbi_load_from_memory(encoded, nPixels, &resultWidth, &resultHeight, &resultChannels, desiredChannels);
		}
		byte* encode_png(const byte* pixels, int32 width, int32 height, int32 channels, int32& resultLen)
		{
			return stbi_write_png_to_mem(pixels, 0, width, height, channels, &resultLen);
		}
		string get_failure_reason()
		{
			return stbi_failure_reason();
		}
		void set_vertical_flip_on_load(bool value)
		{
			stbi_set_flip_vertically_on_load(value);
		}
		void set_vertical_flip_on_write(bool value)
		{
			stbi_flip_vertically_on_write(value);
		}
		void free_image(byte* image)
		{
			stbi_image_free(image);
		}
	}
}