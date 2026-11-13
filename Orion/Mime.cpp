/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Orion/Mime.h>

namespace orion
{
	std::unordered_map<std::string, std::string> _base_mime_ext =
	{
		{"scene", "application/vnd.meworla.scene"},
		{"jpg", "image/jpeg"},
		{"jpeg", "image/jpeg"},
		{"gif", "image/gif"},
		{"png", "image/png"},
		{"tiff", "image/tiff"},
		{"tif", "image/tiff"},
		{"bmp", "image/bmp"},
		{"txt", "text/plain"},
		{"text", "text/plain"},
		{"csv", "text/csv"},
		{"css", "text/css"},
		{"html", "text/html"},
		{"htm", "text/html"},
		{"xml", "text/xml"},
		// the next two according to https://www.iana.org/assignments/media-types/media-types.xhtml#model registered on 2020-02-13
		{"obj", "model/obj"},
		{"mtl", "model/mtl"}		
	};


	bool MIME::GetFromExtension(std::string& url, std::string& result)
	{
		// safety check
		if (!url.size() || url[0] == '.')
			return false;

		// get the extension
		size_t ext_pos = url.find_last_of(".");
		if (ext_pos == std::string::npos)
			return false;

		std::string ext = url.substr(ext_pos + 1);
		if (_base_mime_ext.find(ext) != _base_mime_ext.end()) {
			result = _base_mime_ext[ext];
			return true;
		}

		return false;
	}
	
	void MIME::RegisterExtenstion(std::string mime_type, std::string extension)
	{
		_base_mime_ext[extension] = mime_type;
	}

	bool MIME::GetFromContentType(std::string& content_type, std::string& result)
	{
		// safety check
		if (!content_type.size())
			return false;

		result = content_type;

		const auto parameter_start = content_type.find(';');
		if (parameter_start != std::string::npos)
			result.erase(parameter_start);

		const auto first = content_type.find_first_not_of(" \t");
		if (first == std::string::npos) {
			result.clear();
			return false;
		}
		else {
			const auto last = content_type.find_last_not_of(" \t");
			result = result.substr(first, last - first + 1);
			for (char& c : result) {
				if (c >= 'A' && c <= 'Z')
					c = static_cast<char>(c + ('a' - 'A'));
			}
		}
		return true;
	}

}