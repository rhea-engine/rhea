#ifndef ORION_FILERESOURCE_H
#define ORION_FILERESOURCE_H

#include <string>
#include <stdio.h>

#include "Resource.h"

using std::string;

namespace Orion
{
	class FileResource : public Resource
	{
	protected:		
		size_t m_length;		
		string m_path;
		time_t m_lastmod;
		bool accessible;
	public:
		FileResource(string path);
		~FileResource();
		size_t get_length() { return m_length; }
		time_t getLastmod() { return m_lastmod; }

		virtual ResourceStream* Open(int mode);
	};
}
#endif // !ORION_FILERESOURCE_H
