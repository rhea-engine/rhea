#ifndef POSEIDON_IMAGE_H
#define POSEIDON_IMAGE_H

#include <string>

namespace Poseidon
{
	class Image {
	protected:
		unsigned char* m_buffer = 0;
		int m_width = 0;
		int m_height = 0;
		int m_channels = 0;
		int m_depth = 1;
		bool m_is_compressed = false;
	public:
		virtual bool load(const std::string& file_name);
		virtual bool load(unsigned char* buf, size_t buflen);
		Image* Image::ConvertToRGBA();
		bool isCompressed() { return m_is_compressed; };
		bool Image::SetSize(int width, int height, int depth, unsigned channels);
		unsigned char* get_data() { return m_buffer; };
	protected:
		bool _load(unsigned char* src);
	};
}

#endif // !POSEIDON_IMAGE_H