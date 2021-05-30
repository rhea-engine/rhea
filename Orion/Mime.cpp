#include "Mime.h"

namespace Orion
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

}