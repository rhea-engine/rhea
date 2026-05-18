/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Orion/ResolveLocal.h>
#include <Psio/Fs/Fs.h>
#include <Orion/FileStream.h>

std::unique_ptr<orion::ResourceStream> orion::ResolveLocal::getResource(std::string realm_name, std::string resource_path)
{
	std::unique_ptr<ResourceStream> stream;
	std::string path;

	if (!m_is_local || (!realm_name.empty() && realm_name != "local"))
		path = m_path + "/" + realm_name + resource_path;
	else
		path = m_path + resource_path;
	
	if (psio::Fs::fileExists(path))
	{
		// file exists
		// create file resource and return it
		stream = FileResourceStream::getStream(path);
		return stream;
	}

	return nullptr;
}

bool orion::ResolveLocal::checkResourceExists(std::string realm_name, std::string resource_path)
{
	std::string path;

	if (!m_is_local || (!realm_name.empty() && realm_name != "local"))
		path = m_path + "/" + realm_name + resource_path;
	else
		path = m_path + resource_path;

	return psio::Fs::fileExists(path);
}
