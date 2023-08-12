#include "Material.h"

namespace Rhea 
{
	bool Material::getAlphaBlending()
	{
		return getValue("alpha_blending", false);
	}	
}
