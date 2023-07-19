#include "Camera.h"

namespace Rhea
{
	Unit* Camera::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new Camera(name, type);
		return ret;
	}
}