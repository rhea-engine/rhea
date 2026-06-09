#ifndef RHEA_VIEWLAYER_H
#define RHEA_VIEWLAYER_H

#include <string>

namespace rhea
{
	struct ViewLayer
	{
		std::string name;
		int x = 0, y = 0, w = 0, h = 0;
	};
}
#endif // !RHEA_VIEWLAYER_H
