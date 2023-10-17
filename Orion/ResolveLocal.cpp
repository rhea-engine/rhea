#include "ResolveLocal.h"
#include <Psio/Fs/Fs.h>
#include "FileStream.h"

std::unique_ptr<Orion::ResourceStream> Orion::ResolveLocal::getResource(std::string realm_name, std::string resource_path)
{
	std::unique_ptr<ResourceStream> stream;
	std::string path;

	if (!m_is_local)
		path = m_path + "/" + realm_name + resource_path;
	else
		path = m_path + resource_path;

	if (Psio::Fs::fileExists(path))
	{
		// file exists
		// create file resource and return it
		stream = FileResourceStream::getStream(path);
		return stream;
	}

	return nullptr;
}

bool Orion::ResolveLocal::checkResourceExists(std::string realm_name, std::string resource_path)
{
	std::string path;

	if (!m_is_local)
		path = m_path + "/" + realm_name + resource_path;
	else
		path = m_path + resource_path;

	return Psio::Fs::fileExists(path);
}
