/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#ifndef RHEA_GUI_LAYOUTALIGNMENT_H
#define RHEA_GUI_LAYOUTALIGNMENT_H

#include "Alignment.h"

namespace rhea
{
    struct AxisLayout {
        float offset;
        float spacing;
    };

    class LayoutAlignment {
    public:
        enum class JustifyContent {
            START,
            END,
            CENTER,
            SPACEBETWEEN,
            SPACEAROUND,
            SPACEEVENLY
        };

        LayoutAlignment(JustifyContent justify_content, Alignment::AlignItems align_items)
            : m_justify_content(justify_content), m_align_items(align_items) {}

		void setJustifyContent(JustifyContent justify_content) { m_justify_content = justify_content; }
		void setAlignItems(Alignment::AlignItems align_items) { m_align_items = align_items; }
		JustifyContent getJustifyContent() const { return m_justify_content; }
		Alignment::AlignItems getAlignItems() const { return m_align_items; }

        AxisLayout justify(            
            float container_size,
            float children_size,
            int child_count
        ) {
            AxisLayout layout{ 0.0f, 0.0f };
            if (child_count <= 0)
                return layout;
            float free_space = container_size - children_size;
            switch (m_justify_content) {
            case JustifyContent::START:
                layout.offset = 0.0f;
                layout.spacing = 0.0f;
                break;
            case JustifyContent::END:
                layout.offset = free_space;
                layout.spacing = 0.0f;
                break;
            case JustifyContent::CENTER:
                layout.offset = free_space / 2.0f;
                layout.spacing = 0.0f;
                break;
            case JustifyContent::SPACEBETWEEN:
                layout.offset = 0.0f;
                layout.spacing = child_count > 1 ? free_space / (child_count - 1) : 0.0f;
                break;
            case JustifyContent::SPACEAROUND:
                layout.spacing = free_space / child_count;
                layout.offset = layout.spacing / 2.0f;
                break;
            case JustifyContent::SPACEEVENLY:
                layout.spacing = free_space / (child_count + 1);
                layout.offset = layout.spacing;
                break;
            }
			return layout;
        }

        float align(
            float slot_size,
            float child_size
        )
        {
            float offset = 0.0f;
            switch (m_align_items) {
            case Alignment::AlignItems::START:
                offset = 0.0f;
                break;
            case Alignment::AlignItems::END:
                offset = slot_size - child_size;
                break;
            case Alignment::AlignItems::CENTER:
                offset = (slot_size - child_size) / 2.0f;
                break;
            case Alignment::AlignItems::STRETCH:
                offset = 0.0f; // Stretch would require resizing the child, so we return 0 offset here.
                break;
			}
            return offset;
		}

        void setGap(float gap) { m_gap = gap; }
        float getGap() const { return m_gap; }

    protected:
		JustifyContent m_justify_content;
		Alignment::AlignItems m_align_items;
        float m_gap = 0.0f;
    };
}
#endif // !RHEA_GUI_LAYOUTALIGNMENT_H
