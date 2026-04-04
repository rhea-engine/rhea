#ifndef ORION_MIME_H
#define ORION_MIME_H

#include <string>
#include <unordered_map>

namespace orion
{
	class MIME 
	{
	public:
		/* Get the MIME type for the given URL
		*
		*  Returns false if no mime type was found for this extension
		*  Returns true if mime type was found - result is set to the type
		*/
		static bool GetFromExtension(std::string& url, std::string& result);

		// register a MIME type extension
		static void RegisterExtenstion(std::string mime_type, std::string extension);
	};
}

#endif // !ORION_MIME_H