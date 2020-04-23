#ifndef RHEA_TEXTRESOURCE_H
#define RHEA_TEXTRESOURCE_H

#include "Resource.h"

namespace Rhea
{
	class TextResource : public Resource
	{
	public:
		TextResource(std::string basename, std::string type_name) :
			m_buf(NULL),
			m_buflen(0),
			Resource(basename, type_name)
		{}
		~TextResource();
		void load(ReadyHandler handler);

		std::string getAsString();

		// factory function
		static Unit* createType(std::string name, std::string type);

	protected:
		char* m_buf;
		size_t m_buflen;
	};
}

#endif // !RHEA_TEXTRESOURCE_H
