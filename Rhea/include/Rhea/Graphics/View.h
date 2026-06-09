#ifndef RHEA_VIEW_H
#define RHEA_VIEW_H

#include <string>
#include <vector>

#include "ViewLayer.h"

namespace rhea
{
	struct View
	{
		std::string name;
		int x=0, y=0, width=0, height=0;

		std::vector<ViewLayer> layers;
	};
}

#endif // !RHEA_VIEW_H
