/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_FONTFAMILY_H
#define RHEA_FONTFAMILY_H

#include <Rhea/Unit.h>

namespace rhea
{
	class Font;

	class FontFamily : public Unit
	{
		MECLASS_VIRTUAL(FontFamily, Unit)
	public:
		FontFamily(std::string name, std::string type_name) :
			Unit(name, type_name)
		{}

		virtual Font* resolve(float requested_size) = 0;
	};
}

#endif // !RHEA_FONTFAMILY_H
