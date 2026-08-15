/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Resources/ResourceLibrary.h>

namespace rhea
{

	std::vector<Unit::ResourceReference> ResourceLibrary::getReferences()
	{
		std::vector<ResourceReference> refs;

		for (auto it = m_values.begin(); it != m_values.end(); ++it)
		{
			if (!it->first.compare("path"))
				refs.push_back({ it->first, "//meworla/scene", it->second, ResourceUsage::EnsureReady });
		}
		return refs;
	}
}