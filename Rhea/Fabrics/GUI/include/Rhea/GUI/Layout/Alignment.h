/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GUI_ALIGNMENT_H
#define RHEA_GUI_ALIGNMENT_H

#include <Math/Vector2.h>

namespace rhea {

	class Alignment
	{
	public:
		enum class Horizontal {
			START,
			END,
			CENTER
		};

		enum class Vertical {
			TOP,
			BOTTOM,
			CENTER
		};

		Alignment(Horizontal horizontal, Vertical vertical) :
			m_h_alignment(horizontal),
			m_v_alignment(vertical)
		{}

		void apply(Vector2& size_constraints, Vector2& childSize, Vector2& position) {
			// Note: if the size was set to fill this makes no sense
			Vector2 padding = size_constraints - childSize;
			switch (m_h_alignment) {
			case Horizontal::START:
				break;
			case Horizontal::END:
				position.x = padding.x;
				break;
			case Horizontal::CENTER:
				position.x += (padding.x / 2);
				break;
			}
			switch (m_v_alignment) {
			case Vertical::TOP:
				break;
			case Vertical::BOTTOM:
				position.y = padding.y;
				break;
			case Vertical::CENTER:
				position.y += (padding.y / 2);
				break;
			}
		};

		std::string name() { return "alignment"; }

	protected:
		Horizontal m_h_alignment;
		Vertical m_v_alignment;
	};
}
#endif // !RHEA_GUI_ALIGNMENT_H
