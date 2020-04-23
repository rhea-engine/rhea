#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

using std::string;

namespace Poseidon
{
	bool Image::load(const string& file_name)
	{
		string extension_s;

		size_t pos = file_name.find_last_of('.');
		if (pos != string::npos && pos < (file_name.length() - 1))
		{
			extension_s = file_name.substr(pos + 1);
		}
		
		unsigned char* src = stbi_load(file_name.c_str(), &m_width, &m_height, &m_channels, 0);

		return _load(src);
	}

	bool Image::load(unsigned char* buf, size_t buflen)
	{
		unsigned char* src = stbi_load_from_memory(buf, buflen, &m_width, &m_height, &m_channels, 0);
		return _load(src);
	}

	bool Image::_load(unsigned char* src)
	{
		int depth = 1;

		if (src) {

			// check for colors
			if (m_channels > 4)
			{
				printf("More than 4 color channels are not supported\n");
				return false;
			}

			m_buffer = new unsigned char[(size_t)m_width * m_height * depth * m_channels];
			auto size = (size_t)m_width * m_height * depth * m_channels;
			memcpy(m_buffer, src, size);

		}
		else {
			printf("%s\n", stbi_failure_reason());
			return false;
		}
		return true;
	}

	bool Image::SetSize(int width, int height, int depth, unsigned channels)
	{
		if (width == m_width && height == m_height && depth == m_depth && channels == m_channels)
			return true;

		if (width <= 0 || height <= 0 || depth <= 0)
			return false;

		if (channels > 4)
		{
			printf("More than 4 color channels are not supported");
			return false;
		}

		m_buffer = new unsigned char[(size_t)width * height * depth * channels];
		m_width = width;
		m_height = height;
		m_depth = depth;
		m_channels = channels;
		m_is_compressed = false;
		//compressedFormat_ = CF_NONE;
		//numCompressedLevels_ = 0;
		//nextLevel_.Reset();

		//SetMemoryUse(width * height * depth * channels);
		return true;
	}

	

	Image* Image::ConvertToRGBA()
	{
		if (isCompressed())
		{
			printf("Can not convert compressed image to RGBA");
			return new Image();
		}
		if (m_channels < 1 || m_channels > 4)
		{
			printf("Illegal number of image channels for conversion to RGBA");
			return new Image();
		}
		if (!m_buffer)
		{
			printf("Can not convert image without data to RGBA");
			return new Image();
		}

		// Already RGBA?
		if (m_channels == 4)
			//return SharedPtr<Image>(const_cast<Image*>(this));
			return this;

		Image *ret = new Image();
		ret->SetSize(m_width, m_height, m_depth, 4);

		const unsigned char* src = m_buffer;
		unsigned char* dest = ret->m_buffer;

		switch (m_channels)
		{
		case 1:
			for (unsigned i = 0; i < static_cast<unsigned>(m_width * m_height * m_depth); ++i)
			{
				unsigned char pixel = *src++;
				*dest++ = pixel;
				*dest++ = pixel;
				*dest++ = pixel;
				*dest++ = 255;
			}
			break;

		case 2:
			for (unsigned i = 0; i < static_cast<unsigned>(m_width * m_height * m_depth); ++i)
			{
				unsigned char pixel = *src++;
				*dest++ = pixel;
				*dest++ = pixel;
				*dest++ = pixel;
				*dest++ = *src++;
			}
			break;

		case 3:
			for (unsigned i = 0; i < static_cast<unsigned>(m_width * m_height * m_depth); ++i)
			{
				*dest++ = *src++;
				*dest++ = *src++;
				*dest++ = *src++;
				*dest++ = 255;
			}
			break;

		default:
			assert(false);  // Should never reach nere
			break;
		}

		return ret;
	}

}