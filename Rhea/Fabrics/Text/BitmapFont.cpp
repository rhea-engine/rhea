#include "BitmapFont.h"
#include <vector>
#include <string>

#include "BitmapFont.h"
#include "Resources/ResourceList.h"
#include "Application.h"
#include "UnitTree.h"
#include "Fabrics/Media/ImageResource.h"
#include "Orion/ResourceStream.h"
#include <Psio/Fs/Fs.h>
#include <Psio/Log.h>
#include <Rhea/Fabrics/Core/Shader.h>

namespace Rhea
{
	void BitmapFont::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);

		// dependent resources
		m_values["shader"] = "//rhea/shaders/text";

		// default values
		m_sz = getValue("font_size", 32.0f);
		m_dim_w = getValue("bmp_width", 256.0f);
		m_dim_h = getValue("bmp_height", 256.0f);
	}

	std::vector<Unit::Export> BitmapFont::getReferences()
	{
		std::vector<Export> refs;
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

		// load and add textures
		if (m_values.find("texture") != m_values.end())
		{						
			Unit* tex = m_app->getUnitTree()->find(m_values["texture"], m_attributes["basepath"], false);
			if (tex) {

				// create dummy material
				m_material = new Material(m_path + "_material");
				m_material->setup(m_app);
				m_material->addTexture(static_cast<ImageResource*>(tex));			

				// get the shader
				Unit* shader_res = m_app->getUnitTree()->find(m_values["shader"], m_attributes["basepath"], false);
				if (shader_res) {
					m_material->setShaderRes(doCast<Shader>(shader_res));
				}
				else {
					LOG(LOG_ERROR) << "BitmapFont::ready(): Shader '" << (std::string)m_values["shader"] << "' for " << m_pathname << " not found";
				}
			}
			else {
				LOG(LOG_ERROR) << "BitmapFont::ready(): texture for " << m_pathname << " not found";
			}
		}

		// load font description if any		
		if (m_values.find("font_info")!=m_values.end()) {
			TextResource* font_info = static_cast<TextResource*>(m_app->getUnitTree()->find(m_values["font_info"], m_attributes["basepath"], false));
			if (font_info && font_info->isA("//meworla/text")) {
				char* buf;
				size_t buflen;
				font_info->getRaw(&buf, &buflen);				
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
				22.0f
			};

			for (unsigned int row = 0; row < nof_rows; row++) {
				for (unsigned int col = 0; col < nof_cols; col++) {
					// compute position of glyph
					glyph.left = (float)((col * m_sz) / m_dim_w);
					glyph.right = glyph.left + (m_sz / m_dim_w);
					glyph.top = (float)((row * m_sz) / m_dim_h);
					glyph.bottom = glyph.top + (m_sz / m_dim_h);
					m_glyphs.push_back(glyph);
				}
			}
		}
	}

	Glyph* BitmapFont::getGlyph(unsigned int letter)
	{		
		// overflow?
		letter -= m_start_letter;
		if (letter >= m_glyphs.size()) {
			letter = 0;
			// check if any glyphs defined			
			if (!m_glyphs.size())
				return nullptr;
		}
		return &m_glyphs[letter];
	}
}