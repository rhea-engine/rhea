/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GUI_ALIGNMENT_H
#define RHEA_GUI_ALIGNMENT_H

#include <Math/Vector2.h>
#include <string>

namespace rhea {

	class Alignment
	{
	public:
		enum class AlignItems {
			START,
			END,
			CENTER,
			STRETCH
		};

		Alignment(AlignItems horizontal, AlignItems vertical) :
			m_h_alignment(horizontal),
			m_v_alignment(vertical)
		{}

		void apply(Vector2& size_constraints, Vector2& childSize, Vector2& position) {
			// Note: if the size was set to fill this makes no sense
			Vector2 padding = size_constraints - childSize;
			switch (m_h_alignment) {
			case AlignItems::START:
				break;
			case AlignItems::END:
				position.x = padding.x;
				break;
			case AlignItems::CENTER:
				position.x += (padding.x / 2);
				break;
			}
			switch (m_v_alignment) {
			case AlignItems::START:
				break;
			case AlignItems::END:
				position.y = padding.y;
				break;
			case AlignItems::CENTER:
				position.y += (padding.y / 2);
				break;
			}
		};

		void setHorizontal(AlignItems horizontal) { m_h_alignment = horizontal; }
		void setVertical(AlignItems vertical) { m_v_alignment = vertical; }

		AlignItems getHorizontal() const { return m_h_alignment; }
		AlignItems getVertical() const { return m_v_alignment; }

		std::string name() { return "alignment"; }

	protected:
		AlignItems m_h_alignment;
		AlignItems m_v_alignment;
	};
}
#endif // !RHEA_GUI_ALIGNMENT_H
