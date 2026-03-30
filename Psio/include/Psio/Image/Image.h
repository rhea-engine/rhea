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
		struct Color {
			unsigned char r, g, b;
		};

		virtual bool load(const std::string& file_name);
		virtual bool load(unsigned char* buf, size_t buflen);
		Image* ConvertToRGBA();
		bool isCompressed() { return m_is_compressed; };
		bool SetSize(int width, int height, int depth, unsigned channels);
		unsigned char* get_data() { return m_buffer; };

		int getWidth() { return m_width; }
		int getHeight() { return m_height; }

		int getChannels() { return m_channels; }

		Color operator[](size_t idx);
		Color getPixel(size_t x, size_t y);

	protected:
		bool _load(unsigned char* src);
	};
}

#endif // !POSEIDON_IMAGE_H