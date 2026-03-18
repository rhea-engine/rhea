#ifndef ORION_RESOLVE_LOCAL_H
#define ORION_RESOLVE_LOCAL_H

#include <Orion/ResolverInterface.h>

namespace Orion
{
	class ResolveLocal : public ResolverInterface
	{
	public:
		ResolveLocal(std::string path, bool is_local = false) :
			m_is_local(is_local),
			m_path(path)
		{}

		std::unique_ptr<ResourceStream> getResource(std::string realm_name, std::string resource_path);

		bool checkResourceExists(std::string realm_name, std::string resource_path);

	protected:
		std::string m_path;
		bool m_is_local;
	};
}

#endif // !ORION_RESOLVE_LOCAL_H
