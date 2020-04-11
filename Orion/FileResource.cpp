#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "os/Fs.h"
#include "FileResource.h"
#include "FileStream.h"

namespace Orion
{
	FileResource::FileResource(string path)
	{	
		struct _stat buf;
		m_path = path;

		int result = _stat(path.c_str(), &buf);
		if (result !=0) {
			accessible = false;
			m_length = 0;
		}
		else {
			accessible = true;
			m_length = buf.st_size;
			m_lastmod = buf.st_mtime;
		}
	}

	FileResource::~FileResource()
	{
	}

	ResourceStream* FileResource::Open(int mode)
	{
		FileResourceStream* s;

		if (!accessible)
			return NULL;

		s = new FileResourceStream(m_path, mode);
		return (ResourceStream*)s;
	}
}