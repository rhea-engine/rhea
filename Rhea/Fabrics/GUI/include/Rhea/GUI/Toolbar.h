/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#ifndef RHEA_GUI_TOOLBAR_H
#define RHEA_GUI_TOOLBAR_H

#include <Rhea/GUI/Widget.h>
#include <Rhea/GUI/Row.h>

namespace rhea
{

	class Toolbar : public rhea::Widget
	{
		RECLASS(Toolbar, rhea::Widget, "//meworla/gui/toolbar")
	public:
		Toolbar(std::string path_name, std::string type_name) :
			rhea::Widget(path_name, type_name)
		{
			undefineSlot("content");
			defineSlot("start", true);
			defineSlot("fill", true);
			defineSlot("end", true);
		}
		void ready() override;

		void onSetPosition() override;
		math::Vector2 innerSize(math::Vector2 size_constraints) override;

		bool addToSlot(std::string slot, Widget* child) override;
		bool removeFromSlot(std::string slot, Widget* child) override;
		Widget* resolveSlotWidget(std::string_view slot_name, std::string_view content_name) const override;
		void addStart(rhea::Widget* widget);
		void addFill(rhea::Widget* widget);
		void addEnd(rhea::Widget* widget);

		const std::string getSelector() override { return "Toolbar"; }
		bool buildDefaultLayout() override;

	private:		
		void createContainers();
		rhea::RowWidget* m_start_container = nullptr;
		rhea::RowWidget* m_fill_container = nullptr;
		rhea::RowWidget* m_end_container = nullptr;

		float m_spacing = 5.0f;
		float m_start_width = 0.0f;
		float m_end_width = 0.0f;
		float m_fill_width = 0.0f;
	};
}
#endif // !RHEA_GUI_TOOLBAR_H