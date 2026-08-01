/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Text/BitmapFont.h"
#include <vector>
#include <string>
#include <exception>
#include <sstream>
#include <unordered_map>

#include "Rhea/Resources/ResourceList.h"
#include "Rhea/Application.h"
#include "Rhea/UnitTree.h"
#include "Rhea/Resources/ImageResource.h"
#include "Orion/ResourceStream.h"
#include <Psio/Fs/Fs.h>
#include <Psio/Log.h>
#include <Rhea/Core/Shader.h>

namespace rhea
{
	namespace
	{
		std::unordered_map<std::string, int> parseBmFontValues(const std::string& line)
		{
			std::unordered_map<std::string, int> values;
			std::istringstream stream(line);
			std::string token;

			// Skip the record type ("common", "char", ...).
			stream >> token;
			while (stream >> token) {
				const size_t separator = token.find('=');
				if (separator == std::string::npos)
					continue;

				const std::string key = token.substr(0, separator);
				try {
					values[key] = std::stoi(token.substr(separator + 1));
				}
				catch (const std::exception&) {
					// Non-numeric fields (for example page file names) are irrelevant here.
				}
			}
			return values;
		}

		bool loadBmFont(const char* data, size_t length, int& texture_width,
			int& texture_height, float& font_size, float& line_height,
			unsigned int first_character,
			std::vector<Glyph>& glyphs)
		{
			const std::string contents(data, length);
			if (contents.find("common ") == std::string::npos ||
				contents.find("char id=") == std::string::npos)
				return false;

			std::istringstream lines(contents);
			std::string line;
			while (std::getline(lines, line)) {
				if (line.compare(0, 5, "info ") == 0) {
					const auto values = parseBmFontValues(line);
					if (values.count("size") && values.at("size") != 0) {
						const int size = values.at("size");
						font_size = static_cast<float>(size < 0 ? -size : size);
					}
				}
				else if (line.compare(0, 7, "common ") == 0) {
					const auto values = parseBmFontValues(line);
					if (values.count("scaleW")) texture_width = values.at("scaleW");
					if (values.count("scaleH")) texture_height = values.at("scaleH");
					if (values.count("lineHeight") && values.at("lineHeight") > 0)
						line_height = static_cast<float>(values.at("lineHeight"));
				}
				else if (line.compare(0, 5, "char ") == 0) {
					const auto values = parseBmFontValues(line);
					if (!values.count("id") || !values.count("x") || !values.count("y") ||
						!values.count("width") || !values.count("height") ||
						!values.count("xadvance"))
						continue;

					const int id = values.at("id");
					if (id < static_cast<int>(first_character) || texture_width <= 0 || texture_height <= 0)
						continue;

					const size_t index = static_cast<size_t>(id - first_character);
					if (glyphs.size() <= index)
						glyphs.resize(index + 1, Glyph{ 0, 0, 0, 0, 0, 0, 0, 0, 0 });
					
					Glyph& glyph = glyphs[index];
					const int bitmap_width = values.at("width");
					const int bitmap_height = values.at("height");
					const float inset_x = bitmap_width > 1 ? 0.5f : 0.0f;
					const float inset_y = bitmap_height > 1 ? 0.5f : 0.0f;
					glyph.left = (static_cast<float>(values.at("x")) + inset_x) / texture_width;
					glyph.top = (static_cast<float>(values.at("y")) + inset_y) / texture_height;
					glyph.right = (static_cast<float>(values.at("x") + bitmap_width) - inset_x) / texture_width;
					glyph.bottom = (static_cast<float>(values.at("y") + bitmap_height) - inset_y) / texture_height;
					glyph.bearing_x = values.count("xoffset")
						? values.at("xoffset") : 0;
					glyph.bearing_y = values.count("yoffset")
						? values.at("yoffset") : 0;
					glyph.width = static_cast<float>(bitmap_width);
					glyph.height = static_cast<float>(bitmap_height);
					glyph.advance = static_cast<float>(values.at("xadvance"));
				}
			}

			return !glyphs.empty();
		}
	}

	void BitmapFont::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);

		// dependent resources
		m_values["shader"] = "//rhea/shaders/text";

		// default values
		m_sz = getValue("font_size", 32.0f);
		m_line_height = m_sz;
		m_dim_w = getValue("bmp_width", 256.0f);
		m_dim_h = getValue("bmp_height", 256.0f);
	}

	std::vector<Unit::ResourceReference> BitmapFont::getReferences()
	{
		std::vector<ResourceReference> refs;
		if (m_values.find("texture") != m_values.end()) {
			refs.push_back({ "texture", "//meworla/texture", m_values["texture"] });
		}
		if (m_values.find("shader") != m_values.end()) {
			refs.push_back({ "shader", "//meworla/shader", m_values["shader"] });
		}
		if (m_values.find("font_info") != m_values.end()) {
			refs.push_back({ "fontinfo", "//meworla/fontinfo", m_values["font_info"] });
		}
		return refs;
	}
	
	void BitmapFont::ready()
	{
		Unit::ready();

		if (m_material)
			return;

		// load and add textures
		if (m_values.find("texture") != m_values.end())
		{						
			Unit* tex = doCast<Unit>(m_app->getUnitTree()->find(m_values["texture"].toString(), m_attributes["basepath"], false));
			if (tex) {

				// create dummy material
				m_material = new Material(m_path + "_material");
				m_material->setup(m_app, {}, {});
				m_material->setValue("shader", m_values["shader"]);

				// get the shader
				Shader* shader_res = doCast<Shader>(m_app->getUnitTree()->find(m_values["shader"].toString(), m_attributes["basepath"], false));
				if (shader_res) {
					m_material->setShader(shader_res);
				}
				else {
					LOG(LOG_ERROR) << "BitmapFont::ready(): Shader '" << (std::string)m_values["shader"] << "' for " << m_pathname << " not found";
					return;
				}
				m_material->setTexture(0, static_cast<ImageResource*>(tex));
			}
			else {
				LOG(LOG_ERROR) << "BitmapFont::ready(): texture for " << m_pathname << " not found";
			}
		}

		// load font description if any		
		if (m_values.find("font_info")!=m_values.end()) {
			TextResource* font_info = static_cast<TextResource*>(m_app->getUnitTree()->find(m_values["font_info"].toString(), m_attributes["basepath"], false));
			if (font_info && font_info->isA("//meworla/text")) {
				char* buf;
				size_t buflen;
				font_info->getRaw(&buf, &buflen);
				if (loadBmFont(buf, buflen, m_dim_w, m_dim_h, m_sz, m_line_height,
					m_start_letter, m_glyphs))
					return;

				std::string key;
				std::string value;
				size_t start = 0;
				int state = 0;
				bool meta = true;

				unsigned int nof_cols = 0;
				unsigned int nof_rows = 0;

				Glyph glyph = {
				0,0,
				0,0,
				0,0,
				0,0,
				32.0f
				};

				for (size_t i = 0; i < buflen; i++) {
					if (buf[i] == ',') {
						if (!state) {
							state = 1;
							if (i > start) {
								key = std::string(&buf[start], i - start);								
							}
							else {
								key = "";
								printf("Error in font info file: 2\n");
							}
						}
						else {
							value = "";
							printf("Error in font info file: 1\n");
						}
						start = i + 1;
					}
					else if (buf[i] == '\n') {
						if (state == 1) {
							if (i > start) {
								value = std::string(&buf[start], i - start);
								int val = atoi(value.c_str());
								if (!key.compare("Image Width")) {
									m_dim_w = val;
								} 
								else if (!key.compare("Image Height")) {
									m_dim_h = val;
								}
								else if (!key.compare("Cell Width")) {
									m_cell_w = (float)val;									
								}
								else if (!key.compare("Cell Height")) {
									m_cell_h = (float)val;									
									m_line_height = (float)val;
								}
								else if (!key.compare("Font Height")) {
									m_sz = (float)val;
								}
								else if (!(key.substr(0,5).compare("Char "))) {
									if (meta) {
										// first char means header is read
										meta = false;
										nof_cols = (unsigned int)floor(m_dim_w / m_cell_w);
										nof_rows = (unsigned int)floor(m_dim_h / m_cell_h);
										m_glyphs.reserve((size_t)nof_cols * (size_t)nof_rows);
									}
									size_t len;
									for (len = 1; len < 4; len++) {
										if (key[5 + len] == ' ')
											break;
									}
									int letter = std::atoi(key.substr(5, len).c_str());
									if ((unsigned int)letter >= m_start_letter) {
										
										len = 5 + len + 1;

										if (!key.substr(len, 4).compare("Base")) {

											int letter_fixed = letter - m_start_letter;
											unsigned int col = (unsigned int)floor(letter_fixed % nof_cols);
											unsigned int row = (unsigned int)floor(letter_fixed / nof_rows);

											// compute position of glyph (in uv coords 0/0-1/1)
											glyph.left = (float)((col * m_cell_w) / m_dim_w);
											glyph.right = glyph.left + ((float)val / m_dim_w);
											glyph.top = (float)((row * m_cell_h) / m_dim_h);
											glyph.bottom = glyph.top + (m_sz / (float)m_dim_h);
											glyph.width = (float)val;
											glyph.height = m_sz;
											glyph.advance = (float)val;
											m_glyphs.push_back(glyph);
										}
									}
								}


							}
							else {
								value = "";
								printf("Error in font info file: 4\n");
							}
						}
						else {
							value = "";
							printf("Error in font info file: 3\n");
						}
						state = 0;
						start = i + 1;
					}
				}
			}
		}
		else {

			// initalize the glyphs		
			unsigned int nof_cols = (unsigned int)floor(m_dim_w / m_sz);
			unsigned int nof_rows = (unsigned int)floor(m_dim_h / m_sz);
			m_glyphs.reserve((size_t)nof_cols * (size_t)nof_rows);

			Glyph glyph = {
				0,0,
				0,0,
				0,0,
				0,0,
				22.0f
			};

			for (unsigned int row = 0; row < nof_rows; row++) {
				for (unsigned int col = 0; col < nof_cols; col++) {
					// compute position of glyph
					glyph.left = (float)((col * m_sz) / m_dim_w);
					glyph.right = glyph.left + (m_sz / m_dim_w);
					glyph.top = (float)((row * m_sz) / m_dim_h);
					glyph.bottom = glyph.top + (m_sz / m_dim_h);
					glyph.width = m_sz;
					glyph.height = m_sz;
					m_glyphs.push_back(glyph);
				}
			}
		}
	}

	Glyph* BitmapFont::getGlyph(unsigned int letter)
	{
		if (m_glyphs.empty())
			return nullptr;

		if (letter < m_start_letter)
			return &m_glyphs[0];

		const size_t index = static_cast<size_t>(letter - m_start_letter);
		if (index >= m_glyphs.size() || m_glyphs[index].advance <= 0.0f)
			return &m_glyphs[0];

		return &m_glyphs[index];
	}
}
