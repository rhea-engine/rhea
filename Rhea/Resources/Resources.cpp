#include <Rhea/Resources/Resources.h>

namespace rhea
{

	std::vector<Unit::Export> Resources::getReferences()
	{
		std::vector<Export> refs;

		for (auto it = m_values.begin(); it != m_values.end(); ++it)
		{
			if (it->first.compare("type"))
				refs.push_back({ it->first, "//meworla/scene", it->second });
		}
		return refs;
	}
}