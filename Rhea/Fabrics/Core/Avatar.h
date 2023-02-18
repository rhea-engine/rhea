#ifndef RHEA_AVATAR_H
#define RHEA_AVATAR_H

#include <Rhea/Resource.h>
#include <Math/Vector3.h>

namespace Rhea
{
	class Avatar : public Resource
	{
	public:
		Avatar(std::string name) :
			Resource(name, "//meworla/avatar")
		{}

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// factory function
		static Unit* createType(std::string name, std::string type);

		Math::Vector3 getPosition() { return m_position; }

	protected:
		Math::Vector3 m_position;
	};
}

#endif // !RHEA_AVATAR_H
