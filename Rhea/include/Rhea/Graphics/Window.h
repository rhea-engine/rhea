/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GRAPHICS_WINDOW_H
#define RHEA_GRAPHICS_WINDOW_H

#include <cstdint>
#include <vector>

#include "View.h"

namespace rhea {
    using WindowId = uint32_t;

	struct Window
	{
		std::string name = "Main";
		std::string title;
		int width = 0;
		int height = 0;
		bool resizable = true;
		std::vector<View> views;
	};
}

#endif // !RHEA_GRAPHICS_WINDOW_H
