#pragma once

namespace Wizzy
{
	namespace Image
	{
		byte* decode_image(const byte* encoded, int32 nPixels, int32& resultWidth, int32& resultHeight, int32& resultChannels, int32 desiredChannels = 0);
		byte* encode_png(const byte* pixels, int32 nPixels, int32 height, int32 channels, int32& resultLen);

		string get_failure_reason();

		void set_vertical_flip_on_load(bool value);
		void set_vertical_flip_on_write(bool value);

		void free_image(byte* image);
	}
}