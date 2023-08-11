#include "Material.h"

namespace Rhea 
{
	bool Material::getAlphaBlending()
	{
		return getValue("alpha_blending", false);
	}

	Unit* Material::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new Material(name, type);
		return ret;
	}
}
