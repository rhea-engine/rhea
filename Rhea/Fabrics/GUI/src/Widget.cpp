/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#define NOMINMAX
#include <Rhea/GUI/Widget.h>
#include <cassert>
#include <Psio/Log.h>

#include <Rhea/GUI/WidgetEvent.h>

#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Core/Scene.h>
#include <Rhea/Resources/ImageResource.h>
#include <Rhea/Core/Shader.h>

#include <Rhea/Core/Item2D.h>

#include <Rhea/GUI/Modifier/FillSize.h>
#include <Rhea/GUI/Modifier/Offset.h>
#include <Rhea/GUI/Modifier/Size.h>
#include <Rhea/GUI/Modifier/SizeWidth.h>
#include <Rhea/GUI/Modifier/SizeHeight.h>
#include <Rhea/GUI/Modifier/Padding.h>
#include <Rhea/GUI/Modifier/FillWidth.h>
#include <Rhea/GUI/Styles/Property.h>
#include <Rhea/GUI/Styles/StyleSheet.h>
#include <Rhea/GUI/Modifier/SizePercentageWidth.h>
#include <Rhea/GUI/Modifier/FillHeight.h>
#include <Rhea/GUI/Modifier/SizePercentageHeight.h>
#include <Rhea/Components/MeshComponent.h>
#include <Psio/ScopedTimer.h>

namespace rhea
{
	Widget::~Widget()
	{		
	}

	void Widget::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Item2D::setup(app, attributes, values);

		// offset
		m_base_position.x = getValue("x", 0.0f);
		m_base_position.y = getValue("y", 0.0f);
		Variant offset_vec = getValue("position");
		if (offset_vec != Variant()) {
			m_base_position.x = offset_vec[0];
			m_base_position.y = offset_vec[1];
		}
		m_cur_position = m_base_position;

		if (m_base_position.x || m_base_position.y) {
			m_modifiers.push_back(std::make_unique<OffsetModifier>(m_base_position));
			m_properties[StylePropertyId::kLeft].push_back({ CssValueId::kNumeric, m_base_position.x });
			m_properties[StylePropertyId::kTop].push_back({ CssValueId::kNumeric, m_base_position.y });
		}

		// size set
		bool has_w = m_values.find("w") != m_values.end();
		bool has_h = m_values.find("h") != m_values.end();

		if (has_w || has_h) {
			Vector2 m_base_size;
			if (has_w && has_h) {
				m_base_size.x = getValue("w", 0.0f);
				m_base_size.y = getValue("h", 0.0f);
				m_modifiers.push_back(std::make_unique<SizeModifier>(m_base_size));
				m_properties[StylePropertyId::kWidth].push_back({ CssValueId::kNumeric, m_values["w"] });
				m_properties[StylePropertyId::kHeight].push_back({ CssValueId::kNumeric, m_values["h"] });
			}
			else if (has_w) {
				m_base_size.x = getValue("w", 0.0f);
				m_modifiers.push_back(std::make_unique<SizeWidthModifier>(m_base_size.x));
				m_properties[StylePropertyId::kWidth].push_back({ CssValueId::kNumeric, m_values["w"] });
			}
			else {
				m_base_size.y = getValue("h", 0.0f);
				m_modifiers.push_back(std::make_unique<SizeHeightModifier>(m_base_size.y));
				m_properties[StylePropertyId::kHeight].push_back({ CssValueId::kNumeric, m_values["h"] });
			}
		}

		// padding
		if (m_values.find("padding") != m_values.end()) {
			Variant padding = getValue("padding");
			if (!padding.isArray()) {
				m_modifiers.push_back(std::make_unique<PaddingModifier>(padding));
				CssValue css_padding = { CssValueId::kNumeric, simpleparser::Variant(padding) };
				m_properties[StylePropertyId::kPaddingLeft].push_back(css_padding);
				m_properties[StylePropertyId::kPaddingTop].push_back(css_padding);
				m_properties[StylePropertyId::kPaddingRight].push_back(css_padding);
				m_properties[StylePropertyId::kPaddingBottom].push_back(css_padding);
			}
			else if (padding.length() == 4) {
				CssValue css_padding_left = { CssValueId::kNumeric, simpleparser::Variant(padding[0]) };
				CssValue css_padding_top = { CssValueId::kNumeric, simpleparser::Variant(padding[1]) };
				CssValue css_padding_right = { CssValueId::kNumeric, simpleparser::Variant(padding[2]) };
				CssValue css_padding_bottom = { CssValueId::kNumeric, simpleparser::Variant(padding[3]) };
				m_modifiers.push_back(std::make_unique<PaddingModifier>(padding[0], padding[1], padding[2], padding[3]));
				m_properties[StylePropertyId::kPaddingLeft].push_back(css_padding_left);
				m_properties[StylePropertyId::kPaddingTop].push_back(css_padding_top);
				m_properties[StylePropertyId::kPaddingRight].push_back(css_padding_right);
				m_properties[StylePropertyId::kPaddingBottom].push_back(css_padding_bottom);
			}
		}

		// fillsize
		if (getValue("fillsize", false)) {
			m_modifiers.push_back(std::make_unique<FillSizeModifier>());
		}
		if (getValue("fillwidth", false)) {
			m_modifiers.push_back(std::make_unique<FillWidthModifier>());
		}

		// layout
		Variant align = getValue("alignment");
		if (align.isArray()) {
			if (align.size() > 1)
				setAlignment(align[0].toString(), align[1].toString());
			else if (align.size() > 0)
				setAlignment(align[0].toString(), "top");


		}

		// set styles based on values
		if (m_values.find("font") != m_values.end())
			m_properties[StylePropertyId::kFont].push_back({ CssValueId::kString, m_values["font"] });
		if (m_values.find("texture") != m_values.end())
			m_properties[StylePropertyId::kBackgroundImage].push_back({ CssValueId::kString, m_values["texture"] });

		std::unordered_map<std::string, std::string> attr;
		std::unordered_map<std::string, Variant> val;

		if (m_values.find("onClicked") != m_values.end())
			setOnClicked(m_values["onClicked"].toString());

		if (getValue("default", false))
			m_widget_states |= static_cast<int>(WidgetState::kDefault);
	}

	void Widget::ready()
	{
		// prepare material
		Item2D::ready();

		// make sure visualcomponent is set
		VisualComponent* visual_component = getComponent<VisualComponent>();
		if (!visual_component) {
			visual_component = addComponentUnique<VisualComponent>("visual");
		}

		// add to render phase
		visual_component->setRenderPhase(RenderPhase::Overlay);

		MeshComponent* mesh_component = getComponent<MeshComponent>();
		//  widgets always need a render component
		if (!mesh_component) {
			mesh_component = addComponentUnique<MeshComponent>("mesh");
		}

		// if we are hidden at the beggining - send the event
		if (!visual_component->isVisible()) {
			hide();
		}

		markDirtyStyleChildren();

		registerDefaultInputHandlers();
	}

	void Widget::setOnClicked(const std::string& target)
	{		
		m_onClick = target;	
	}

	bool Widget::onClicked(Event* ev, Unit* unit)
	{
		if (m_values.find("onClicked") != m_values.end()) {
			sendBoundEvent(m_values["onClicked"].toString(), ev);
		}
		return false;
	}

	void Widget::setPosition(Vector2 position)
	{
		if (!getComponent<VisualComponent>() || !getComponent<VisualComponent>()->isVisible())
			return;

		m_anchor_position = position;

		Vector2 old_final_position = m_position;

		// default final position = anchor + authored offset
		m_position = m_anchor_position + m_base_position;

		// compute clipping based on new position
		computeClipping();

		// allow subclasses to adjust m_position further
		onSetPosition();

		if (m_position != old_final_position) {
			invalidateRender();
		}
	}

	void Widget::clearWidgetChildren()
	{
		std::vector<Widget*> children_to_remove;
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Widget* w = doCast<Widget>(it->second);
			if (w) {
				children_to_remove.push_back(w);
			}
		}
		for (Widget* w : children_to_remove) {
			if (w->getParent() == this) {
				w->detachFromParent();
				w->destroy();
			}
		}
	}

	void Widget::computeClipping()
	{
		// get clipping from parents visual component
		auto* vis = getComponent<VisualComponent>();
		if (!vis)
			return;

		bool old_clip = vis->hasClipping();
		ClipRect old_rect = vis->getClipping();

		ClipRect rect{};
		bool enabled = false;
		Item* p = doCast<Item>(m_parent);
		if (p) {
			auto* p_vis = p->getComponent<VisualComponent>();
			if (p_vis) {
				enabled = p_vis->hasClipping();
				if (enabled)
					rect = p_vis->getClipping();
			}
		}
		// apply local clipping
		if (m_clip) {
			// Convert local clipping rect to world coordinates
			ClipRect local_rect = m_clipping_rect;
			local_rect.x += m_position.x;
			local_rect.y += m_position.y;

			if (enabled) {
				// intersect with parent
				float x1 = std::max(rect.x, local_rect.x);
				float y1 = std::max(rect.y, local_rect.y);
				float x2 = std::min(rect.x + rect.w, local_rect.x + local_rect.w);
				float y2 = std::min(rect.y + rect.h, local_rect.y + local_rect.h);
				if (x2 > x1 && y2 > y1) {
					rect.x = x1;
					rect.y = y1;
					rect.w = x2 - x1;
					rect.h = y2 - y1;
				}
				else {
					// no intersection
					enabled = false;
				}
			}
			else {
				enabled = true;
				rect = local_rect;
			}
		}

		vis->enableClipping(enabled);
		if (enabled)
			vis->setClipping(rect);

		if (old_clip != enabled || old_rect.x != rect.x || old_rect.y != rect.y || old_rect.w != rect.w || old_rect.h != rect.h) {
			invalidateRender();
		}
	}
	
	void Widget::onSetPosition()
	{
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Widget* w = doCast<Widget>(it->second);
			if (w) {
				w->setPosition(m_position + m_content_position);
			}
		}
	}

	// compute widget size
	// apply all modifiers	
	// get content size and add it (if size not forced)
	// compute and return outer_size
	Vector2 Widget::size(Vector2 size_constraints)
	{
		Vector2 old_size = m_size;
		Vector2 old_outer = m_outer_size;
		Vector2 old_content_pos = m_content_position;
		Vector2 old_content_size = m_content_size;

		m_size = Vector2(-1.0f, -1.0f);
		m_base_position = Vector2(0.0f, 0.0f);
		m_content_position = Vector2(0.0f, 0.0f);
		Vector2 margin;
		Vector2 outer_size;

		// constraints for children
		Vector2 inner_size = size_constraints;

		if (!getComponent<VisualComponent>() || !getComponent<VisualComponent>()->isVisible())
			return m_outer_size;

		// apply modifiers
		for (auto it = m_modifiers.begin(); it < m_modifiers.end(); ++it) {
			(*it)->apply(this, inner_size, m_size, outer_size, margin);
		}

		// get the content without modifiers from the concrete widget
		Vector2 content_size = innerSize(inner_size);

		// content size 
		// is the resulting inner size (TODO handle overflow?)
		m_content_size = content_size;

		// box size
		// if m_size is set it is the size of the box
		// else we use the contents size and add extra space from padding and borders etc but not margin
		m_size.x = (m_size.x >= 0.0f) ? m_size.x : content_size.x + outer_size.x;
		m_size.y = (m_size.y >= 0.0f) ? m_size.y : content_size.y + outer_size.y;

		// depending on box model?
		// outer size is size plus margin
		m_outer_size = m_size + margin;

		// enlarge size and m_outer_size if child is larger than size;
		float content_delta_x = content_size.x - m_size.x;
		if (content_delta_x > 0.0f) {
			m_size.x = content_size.x;
			m_outer_size.x += content_delta_x;
		}
		float content_delta_y = content_size.y - m_size.y;
		if (content_delta_y > 0.0f) {
			m_size.y = content_size.y;
			m_outer_size.y += content_delta_y;
		}

		// clamp report to the constraint size
		if (m_outer_size.x > size_constraints.x)
			m_outer_size.x = size_constraints.x;
		if (m_outer_size.y > size_constraints.y)
			m_outer_size.y = size_constraints.y;

		// Compute tiling if necessary
		ApplyTextureSizing();

		if (m_size != old_size ||
			m_outer_size != old_outer ||
			m_content_position != old_content_pos ||
			m_content_size != old_content_size) {
			invalidateRender();

			// If size changed and clipping is active, update clipping rect
			if (m_size != old_size && m_clip) {
				// Update clipping rect to new size
				m_clipping_rect = { 0.0f, 0.0f, m_size.x, m_size.y };
				computeClipping();
			}
		}

		return m_outer_size;
	}

	Vector2 Widget::innerSize(Vector2 size_constraints)
	{
		Vector2 max_child_size(0.0f, 0.0f);
		// the default for widgets is to use the largest child (not a container)
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Widget* w = doCast<Widget>(it->second);
			if (w) {
				// get child size
				// this is guaranteed to be in the constraint
				Vector2 child_size = w->size(size_constraints);
				if (child_size.x > max_child_size.x)
					max_child_size.x = child_size.x;
				if (child_size.y > max_child_size.y)
					max_child_size.y = child_size.y;
			}
		}
		return max_child_size;
	}

	Vector2 Widget::getMinSize(Vector2 size_constraints)
	{
		Vector2 min_inner_size;
		Vector2 min_outer_size = size_constraints;
		Vector2 max_inner_size;

		// fill min_size by modifiers
		for (auto it = m_modifiers.begin(); it < m_modifiers.end(); ++it) {
			(*it)->essentialSize(this, min_inner_size, max_inner_size, min_outer_size);
		}
		Vector2 padding = min_outer_size - min_inner_size;
		// enlarge to inner size of myself
		innerGetMinSize(max_inner_size, min_inner_size);

		m_cached_min_size = min_inner_size + padding;
		m_cached_max_size = max_inner_size + padding;
		if (m_cached_min_size.x > m_cached_max_size.x)
			m_cached_min_size.x = m_cached_max_size.x;
		if (m_cached_min_size.y > m_cached_max_size.y)
			m_cached_min_size.y = m_cached_max_size.y;

		return m_cached_min_size;
	}

	void Widget::innerGetMinSize(Vector2 size_constraints, Vector2& min_size)
	{
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Widget* w = doCast<Widget>(it->second);
			if (w) {
				Vector2 child_min = w->getMinSize(size_constraints);
				if (child_min.x > min_size.x)
					min_size.x = child_min.x;
				if (child_min.y > min_size.y)
					min_size.y = child_min.y;
			}
		}
	}

	void Widget::RecalculateStyleTree(StyleEngine* style_engine)
	{
		RecalculateStyle(style_engine);
		// Recursively recalculate style for all child widgets.
		for (auto child : m_children) {
			if (doCast<Unit>(child.second)) {
				doCast<Unit>(child.second)->traverse([&](std::string name, rhea::Component* child)->bool {
					if (doCast<Widget>(child)) {
						doCast<Widget>(child)->RecalculateStyle(style_engine);
					}
					return true;
					});
			}
		}
	}

	void Widget::RecalculateStyle(StyleEngine* style_engine)
	{
#ifdef _DEBUG
		std::string  debug_msg = "RecalculateStyle() for " + getPath() + "/" + getPathName();
		ScopedTimer timer(debug_msg.c_str(), 5000.0f);
#endif

		if (m_style_dirty) {

			m_style_dirty = false;

			// Clear any previously computed styles.
			m_computed_properties.clear();

			// Inherit style properties from the parent widget if they are inherited
			if (doCast<Widget>(m_parent)) {
				for (const auto& prop : kCssProperties) {
					if (prop->inherited) {
						// Look for the property in the parent's computed styles.
						auto it = doCast<Widget>(m_parent)->m_computed_properties.find(prop->id);
						if (it != doCast<Widget>(m_parent)->m_computed_properties.end()) {
							// Copy the parent's value.
							m_computed_properties[prop->id] = it->second;
						}
						else {
							// If the parent doesn't define it, fallback to the default value.
							m_computed_properties[prop->id].push_back(CssProperty::GetDefaultValue(prop->id));
						}
					}
				}
			}
			else
			{
				// For the root widget, initialize inherited properties to their defaults.
				for (const auto& prop : kCssProperties) {
					if (prop->inherited) {
						m_computed_properties[prop->id].push_back(CssProperty::GetDefaultValue(prop->id));
					}
				}
			}

			// Get computed style properties from the style engine.
			auto computed_styles = style_engine->GetStylesForWidget(this);

			// Apply these computed styles.
			AppendStyles(computed_styles);

			// overwrite with my own property
			AppendStyles(m_properties);

			ApplyStyles();
		}
	}

	void Widget::AppendStyles(CssRuleSet& styles)
	{
		// For simplicity, we just merge (or override) properties.
		for (const auto& style : styles) {
			m_computed_properties[style.first] = style.second;
		}
	}

	void Widget::ApplyStyles()
	{
		Vector2 base_position;
		Vector2 base_size;
		CssValueId position_types[4] = { CssValueId::kString, CssValueId::kString, CssValueId::kString, CssValueId::kString };
		float prev_font_size = m_font_size;
		TextureSizingMode prev_sizing_mode_w = m_sizing_mode_w;
		TextureSizingMode prev_sizing_mode_h = m_sizing_mode_h;

		bool layout_changed = false;
		bool render_changed = false;

		for (const auto& property : m_computed_properties) {
			auto property_id = property.first;
			auto values = property.second;

			// make sure we have at least one value
			if (values.empty())
				continue;
			switch (property_id) {
			case StylePropertyId::kBackgroundImage:
				setBackgroundImage(values[0].value == "none" ? nullptr : doCast<ImageResource>(m_app->getUnitTree()->find(values[0].value.toString())));
				break;
			case StylePropertyId::kBackgroundColor:
				if (values.size() >= 3) {
					setBackgroundColor(values[0].value, values[1].value, values[2].value, values.size() > 3 ? values[3].value.toFloat() : 1.0f);
				}
				break;
			case StylePropertyId::kBackgroundSize:

				if (values[0].value == "contain") {
					m_sizing_mode_w = TextureSizingMode::kContain;
				}
				else if (values[0].value == "cover") {
					m_sizing_mode_w = TextureSizingMode::kCover;
				}
				if (values.size() >= 2) {
					if (values[1].value == "contain") {
						m_sizing_mode_h = TextureSizingMode::kContain;
					}
					else if (values[1].value == "cover") {
						m_sizing_mode_h = TextureSizingMode::kCover;
					}
				}
				else {
					m_sizing_mode_h = m_sizing_mode_w;
				}
				if (prev_sizing_mode_w != m_sizing_mode_w || prev_sizing_mode_h != m_sizing_mode_h)
					render_changed = true;
				break;
			case StylePropertyId::kLeft:
				base_position.x = values[0].value;
				position_types[0] = values[0].id;
				break;
			case StylePropertyId::kTop:
				base_position.y = values[0].value;
				position_types[1] = values[0].id;
				break;
			case StylePropertyId::kWidth:
				base_size.x = values[0].value;
				position_types[2] = values[0].id;
				break;
			case StylePropertyId::kHeight:
				base_size.y = values[0].value;
				position_types[3] = values[0].id;
				break;
			case StylePropertyId::kFontSize:
				m_font_size = calculateFontSize(values[0]);
				if (prev_font_size != m_font_size)
					layout_changed = true;
				break;
			case StylePropertyId::kOverflow:
			{
				bool old_clip = m_clip;
				ClipRect old_rect = m_clipping_rect;
				if (values[0].value == "hidden" || values[0].value == "scroll") {
					m_clip = true;
					m_clipping_rect = { 0.0f, 0.0f, m_size.x, m_size.y };
				}
				else {
					m_clip = false;
				}
				if (old_clip != m_clip || old_rect.x != m_clipping_rect.x || old_rect.y != m_clipping_rect.y || old_rect.w != m_clipping_rect.w || old_rect.h != m_clipping_rect.h) {
					render_changed = true;
					layout_changed = true;
					// Update VisualComponent clipping when overflow style changes
					computeClipping();
				}
				break;
			}
			case StylePropertyId::kAlignItems:
				{
				std::string align_items = values[0].value.toString();
				if (!m_alignment)
					m_alignment = std::make_unique<LayoutAlignment>(LayoutAlignment::JustifyContent::START, Alignment::AlignItems::START);
				if (align_items == "start") {
					if (m_alignment->getAlignItems() != Alignment::AlignItems::START) {
						m_alignment->setAlignItems(Alignment::AlignItems::START);
						layout_changed = true;
					}
				}
				else if (align_items == "center") {
					if (m_alignment->getAlignItems() != Alignment::AlignItems::CENTER) {
						m_alignment->setAlignItems(Alignment::AlignItems::CENTER);
						layout_changed = true;
					}
				}
				else if (align_items == "end") {
					if (m_alignment->getAlignItems() != Alignment::AlignItems::END) {
						m_alignment->setAlignItems(Alignment::AlignItems::END);
						layout_changed = true;
					}
				}
				else if (align_items == "stretch") {
					// Handle stretch case if needed
					if (m_alignment->getAlignItems() != Alignment::AlignItems::STRETCH) {
						m_alignment->setAlignItems(Alignment::AlignItems::STRETCH);
						layout_changed = true;
					}
				}
				break;
			}
			case StylePropertyId::kJustifyContent:
			{
				std::string justify_items = values[0].value.toString();
				if (!m_alignment)
					m_alignment = std::make_unique<LayoutAlignment>(LayoutAlignment::JustifyContent::START, Alignment::AlignItems::START);
				if (justify_items == "start") {
					if (m_alignment->getJustifyContent() != LayoutAlignment::JustifyContent::START) {
						m_alignment->setJustifyContent(LayoutAlignment::JustifyContent::START);
						layout_changed = true;
					}
				}
				else if (justify_items == "center") {
					if (m_alignment->getJustifyContent() != LayoutAlignment::JustifyContent::CENTER) {
						m_alignment->setJustifyContent(LayoutAlignment::JustifyContent::CENTER);
						layout_changed = true;
					}
				}
				else if (justify_items == "end") {
					if (m_alignment->getJustifyContent() != LayoutAlignment::JustifyContent::END) {
						m_alignment->setJustifyContent(LayoutAlignment::JustifyContent::END);
						layout_changed = true;
					}
				}
				else if (justify_items == "space-around") {
					// Handle space-around case if needed
					if (m_alignment->getJustifyContent() != LayoutAlignment::JustifyContent::SPACEAROUND) {
						m_alignment->setJustifyContent(LayoutAlignment::JustifyContent::SPACEAROUND);
						layout_changed = true;
					}
				}
				else if (justify_items == "stretch") {
					// Handle stretch case if needed
					if (m_alignment->getJustifyContent() != LayoutAlignment::JustifyContent::SPACEBETWEEN) {
						m_alignment->setJustifyContent(LayoutAlignment::JustifyContent::SPACEBETWEEN);
						layout_changed = true;
					}
				}
				else if (justify_items == "stretch") {
					// Handle stretch case if needed
					if (m_alignment->getJustifyContent() != LayoutAlignment::JustifyContent::SPACEEVENLY) {
						m_alignment->setJustifyContent(LayoutAlignment::JustifyContent::SPACEEVENLY);
						layout_changed = true;
					}
				}
				break;
			}
			}
		}

		ApplyTextureSizing();		

		recreateModifiers(base_position, base_size, position_types);

		if (layout_changed)
			invalidateLayout();
		if (render_changed)
			invalidateRender();
	}

	float Widget::calculateFontSize(const CssValue& value) const
	{
		float parent_font_size = doCast<Widget>(m_parent) ? doCast<Widget>(m_parent)->getFontSize() : CssProperty::GetDefaultValue(StylePropertyId::kFontSize).value.toFloat();
		switch (value.id) {
		case CssValueId::kNumeric:
			return value.value;
		case CssValueId::kDimensionEm:
			return parent_font_size * (float)value.value;
		case CssValueId::kPercentage:
			return parent_font_size * (float)value.value / 100.0f;
		case CssValueId::kString:
			return value.value == "inherit" ? parent_font_size : 16.0f;
		default:
			return 16.0f;
		}
	}

	void Widget::recreateModifiers(const Vector2& base_position, const Vector2& base_size, const CssValueId position_types[4])
	{
		std::vector<std::unique_ptr<Modifier>> new_modifiers;

		if (base_position.x || base_position.y) {
			new_modifiers.push_back(std::make_unique<OffsetModifier>(base_position));
		}

		// get my font size
		if (m_font_size <= 0.0f) {
			// if we don't have font size set try to inherit from parent
			if (doCast<Widget>(m_parent))
				m_font_size = doCast<Widget>(m_parent)->getFontSize();
			else {
				CssValue default_font_size_val = CssProperty::GetDefaultValue(StylePropertyId::kFontSize);
				if (default_font_size_val.id == CssValueId::kNumeric)
					m_font_size = (float)default_font_size_val.value;
				else
					// TODO parse size names
					m_font_size = 16;
			}
			if (m_font_size > 0.0f)
				invalidateLayout();
		}

		// size set

		if (position_types[2] != CssValueId::kString && position_types[2] != CssValueId::kPercentage && position_types[3] != CssValueId::kString && position_types[3] != CssValueId::kPercentage) {
			// both dimensions set and are absolute
			Vector2 size;
			switch (position_types[2]) {
			case CssValueId::kNumeric:
				size.x = base_size.x;
				break;
			case CssValueId::kDimensionEm:
				size.x = m_font_size * base_size.x;
				break;
			}
			switch (position_types[3]) {
			case CssValueId::kNumeric:
				size.y = base_size.y;
				break;
			case CssValueId::kDimensionEm:
				size.y = m_font_size * base_size.y;
				break;
			}
			new_modifiers.push_back(std::make_unique<SizeModifier>(size));
		}
		else {
			if (base_size.x > 0.0f) {
				switch (position_types[2]) {
				case CssValueId::kNumeric:
					new_modifiers.push_back(std::make_unique<SizeWidthModifier>(base_size.x));
					break;
				case CssValueId::kDimensionEm:
					new_modifiers.push_back(std::make_unique<SizeWidthModifier>(base_size.x * m_font_size));
					break;
				case CssValueId::kPercentage:
					if (base_size.x < 100.0f)
						new_modifiers.push_back(std::make_unique<SizePercentageWidthModifier>(base_size.x / 100.0f));
					else
						new_modifiers.push_back(std::make_unique<FillWidthModifier>());
					break;
				}

			}
			if (base_size.y > 0.0f) {
				switch (position_types[3]) {
				case CssValueId::kNumeric:
					new_modifiers.push_back(std::make_unique<SizeHeightModifier>(base_size.y));
					break;
				case CssValueId::kDimensionEm:
					new_modifiers.push_back(std::make_unique<SizeHeightModifier>(base_size.y * m_font_size));
					break;
				case CssValueId::kPercentage:
					if (base_size.y < 100.0f)
						new_modifiers.push_back(std::make_unique<SizePercentageHeightModifier>(base_size.y / 100.0f));
					else
						new_modifiers.push_back(std::make_unique<FillHeightModifier>());
					break;
				}
			}
		}

		// padding		
		std::array<float, 4> padding = { 0.0f, 0.0f, 0.0f, 0.0f };
		if (m_computed_properties.find(StylePropertyId::kPaddingLeft) != m_computed_properties.end())
			padding[0] = m_computed_properties[StylePropertyId::kPaddingLeft][0].value;
		if (m_computed_properties.find(StylePropertyId::kPaddingTop) != m_computed_properties.end())
			padding[1] = m_computed_properties[StylePropertyId::kPaddingTop][0].value;
		if (m_computed_properties.find(StylePropertyId::kPaddingRight) != m_computed_properties.end())
			padding[2] = m_computed_properties[StylePropertyId::kPaddingRight][0].value;
		if (m_computed_properties.find(StylePropertyId::kPaddingBottom) != m_computed_properties.end())
			padding[3] = m_computed_properties[StylePropertyId::kPaddingBottom][0].value;
		if (padding[0] > 0.0f || padding[1] > 0.0f || padding[2] > 0.0f || padding[3] > 0.0f)
			new_modifiers.push_back(std::make_unique<PaddingModifier>(padding[0], padding[1], padding[2], padding[3]));

		// fillsize
		if (getValue("fillsize", false)) {
			new_modifiers.push_back(std::make_unique<FillSizeModifier>());
		}
		if (getValue("fillwidth", false)) {
			new_modifiers.push_back(std::make_unique<FillWidthModifier>());
		}

		// check if modifiers changed
		bool modifers_changed = false;

		if (new_modifiers.size() != m_modifiers.size()) {
			modifers_changed = true;
		}
		else {
			for (size_t i = 0; i < new_modifiers.size(); ++i) {
				if (!new_modifiers[i]->equals(*m_modifiers[i])) {
					modifers_changed = true;
					break;
				}
			}
		}
		if (modifers_changed) {
			m_modifiers = std::move(new_modifiers);
			invalidateLayout();
		}
	}
	void Widget::setAlignment(std::string horizontal, std::string vertical)
	{
		LayoutAlignment::JustifyContent h_align = LayoutAlignment::JustifyContent::START;
		Alignment::AlignItems v_align = Alignment::AlignItems::START;

		if (horizontal == "start")
			h_align = LayoutAlignment::JustifyContent::START;
		else if (horizontal == "end")
			h_align = LayoutAlignment::JustifyContent::END;
		else if (horizontal == "center")
			h_align = LayoutAlignment::JustifyContent::CENTER;
		if (vertical == "top")
			v_align = Alignment::AlignItems::START;
		else if (vertical == "bottom")
			v_align = Alignment::AlignItems::END;
		else if (vertical == "center")
			v_align = Alignment::AlignItems::CENTER;

		m_alignment = std::make_unique<LayoutAlignment>(h_align, v_align);
	}

	void Widget::ApplyTextureSizing() {

		if (m_bg_image && m_bg_image->width > 0 && m_bg_image->height > 0 && (m_content_size.x > 1.0f || m_content_size.y > 1.0f))
		{
			// image and image size is set and content is not 1x1

			// tiling
			// assume background-repeat
			// if image is scaled to fit into the widget we can't also tile the image
			math::Vector2 tiling(1.0f, 1.0f);;
			if (m_sizing_mode_w == TextureSizingMode::kAuto)
				tiling.x = std::max(m_content_size.x / m_bg_image->width, 1.0f); // dont tile if image is larger than content
			if (m_sizing_mode_h == TextureSizingMode::kAuto)
				tiling.y = std::max(m_content_size.y / m_bg_image->height, 1.0f);
			if (m_tiling != tiling) {
				m_tiling = tiling;
				m_material_dirty = true;
				invalidateRender();
			}

			if (m_sizing_mode_w == TextureSizingMode::kContain) {
				LOG(LOG_DEBUG) << "ApplyTextureSizing: content=" << m_content_size.x << "/" << m_content_size.y << "; image=" << m_bg_image->width << "/" << m_bg_image->height;;
			}

			// scaling
			if (m_bg_image->width > m_content_size.x || m_bg_image->height > m_content_size.y)
			{
				math::Vector2 scaling(1.0f, 1.0f);
				switch (m_sizing_mode_w) {
				case TextureSizingMode::kContain:
				{
					// Use the smaller scale factor for both axes to maintain aspect ratio
					float aspect_scale = std::min(m_content_size.x / (float)m_bg_image->width, m_content_size.y / (float)m_bg_image->height);
					scaling.x = aspect_scale;
					scaling.y = aspect_scale;
					LOG(LOG_DEBUG) << "ApplyTextureSizing: aspect_scale=" << aspect_scale;
				}
				break;
				case TextureSizingMode::kCover:
				{
					float image_aspect = (float)m_bg_image->width / (float)m_bg_image->height;
					float widget_aspect = m_content_size.x / m_content_size.y;
					if (widget_aspect < image_aspect) {
						// Widget is wider than image, fit height, scale width
						scaling.x = widget_aspect / image_aspect;
					}
					else {
						// Widget is taller than image, fit width, scale height					
						scaling.y = image_aspect / widget_aspect;
					}

					break;
				}
				case TextureSizingMode::kFixed:
					// scaling = { 1.0f, 1.0f };
					break;
				}
				if (scaling != m_scaling) {
					m_scaling = scaling;
					m_material_dirty = true;
					invalidateRender();
				}
			}
		}
	}

	void Widget::setStyles(CssRuleSet& styles)
	{
		for (const auto& style : styles) {
			m_properties[style.first] = style.second;
		}
		m_style_dirty = true;
		markDirtyStyleChildren();
	}

	void Widget::setStyleValue(StylePropertyId id, std::string css_string)
	{
		CssRuleSet styles;
		simpleparser::CssParser parser;
		std::vector<simpleparser::CssParser::Property> properties;
		if (parser.ParsePropertyValue(css_string, properties)) {
			std::vector<CssValue> styles;
			if (StyleSheet::ParseValues(id, properties, styles)) {
				m_properties[id] = styles;
				m_style_dirty = true;
				markDirtyStyleChildren();
			}
		}
	}

	void Widget::setStyle(StylePropertyId id, std::vector<CssValue> value)
	{
		m_properties[id] = value;
		m_style_dirty = true;
		markDirtyStyleChildren();
	}

	const std::vector<CssValue>& Widget::getStyle(StylePropertyId style) const
	{
		auto it = m_computed_properties.find(style);
		if (it != m_computed_properties.end())
			return it->second;
		else {
			static const std::vector<CssValue> empty;
			return empty;
		}
	}

	void Widget::setStyle(StylePropertyId id, std::string value)
	{
		std::vector<CssValue> values;
		std::vector<simpleparser::CssParser::Property> content;
		content.push_back({ simpleparser::CssParser::PropertyId::kString, 0, value });
		if (StyleSheet::ParseValues(id, content, values)) {
			m_properties[id] = values;
			m_style_dirty = true;
			markDirtyStyleChildren();
		}
	}

	void Widget::setStyle(StylePropertyId id, int value)
	{
		std::vector<CssValue> values;
		std::vector<simpleparser::CssParser::Property> content;
		content.push_back({ simpleparser::CssParser::PropertyId::kNumeric, 0, value });
		if (StyleSheet::ParseValues(id, content, values)) {
			m_properties[id] = values;
			m_style_dirty = true;
			markDirtyStyleChildren();
		}
	}

	void Widget::setStyle(StylePropertyId id, int value, std::string dim)
	{
		std::vector<CssValue> values;
		std::vector<simpleparser::CssParser::Property> content;
		content.push_back({ simpleparser::CssParser::PropertyId::kDimension, 0, value, dim });
		if (StyleSheet::ParseValues(id, content, values)) {
			m_properties[id] = values;
			m_style_dirty = true;
			markDirtyStyleChildren();
		}
	}

	void Widget::onParented()
	{
		Item2D::onParented();
		markDirtyStyleChildren();
	}

	void Widget::show()
	{
		Item2D::show();
		invalidateRender();
		invalidateLayout();
		// recursively invalidate all children
		traverse([&](std::string name, rhea::Component* child)->bool {
			if (doCast<Widget>(child)) {
				doCast<Widget>(child)->invalidateRender();
			}
			return true;
			});
		Event e("show", this);
		bubbleEvent("show", &e);
	}

	void Widget::makeVisible()
	{
		Item::show();
	}

	void Widget::hide()
	{
		Item::hide();
		m_needs_update = true;
		Event e("hide", this);
		bubbleEvent("hide", &e);
		clearTransientInputState();
		// clear transient states for all children
		traverse([&](std::string name, rhea::Component* child)->bool {
			if (doCast<Widget>(child)) {
				doCast<Widget>(child)->clearTransientInputState();
			}
			return true;
			});
		invalidateLayout();
	}

	void Widget::makeInvisible()
	{
		Item::hide();
	}

	Widget* Widget::capture(int c_x, int c_y)
	{
		if (!getComponent<VisualComponent>() || !getComponent<VisualComponent>()->isVisible())
			return nullptr;

		if (m_position.x > c_x ||
			m_position.x + m_size.x < c_x ||
			m_position.y > c_y ||
			m_position.y + m_size.y < c_y) {
			return nullptr;
		}

		// is a child a better match?
		Widget* captured;
		for (auto it = m_ordered_children.rbegin(); it != m_ordered_children.rend(); ++it)
		{
			Widget* w = doCast<Widget>(*it);
			if (w) {
				captured = w->capture(c_x, c_y);
				if (captured) {
					return captured;
				}
			}
		}
		return this;
	}

	Widget* Widget::getFocusTarget()
	{
		// if we handle input return myself
		if (isFocusable())
			return this;

		// if we are not a top window we try our parents
		if (!m_top_window) {
			Widget* parent_widget = doCast<Widget>(m_parent);
			while(parent_widget) {
				if (parent_widget->isFocusable())
					return parent_widget;
				parent_widget = doCast<Widget>(parent_widget->m_parent);
			}
		}

		return nullptr;
	}

	void Widget::sendBoundEvent(std::string binding, Event* ev)
	{
		UnitTree::Binding event_binding;

		event_binding = m_app->getUnitTree()->getBinding(binding, m_attributes["basepath"]);

		if (event_binding.target)
			event_binding.target->sendEvent(event_binding.value, ev);
		else
			LOG(LOG_ERROR) << "Sending Event: Binding " << binding << " not found";
	}

	Vector2 Widget::getScreenPosition()
	{
		return m_position;
	}

	void Widget::invalidateLayout()
	{
		bool was_dirty = m_layout_dirty;
		m_layout_dirty = true;

		if (auto* p = doCast<Widget>(m_parent)) {
			if (!was_dirty || !p->isLayoutDirty()) {
				p->invalidateLayout();
			}
		}
	}

	void Widget::markDirtyStyleChildren()
	{
		//if (m_has_style_dirty_children)
		//	return;

		m_has_style_dirty_children = true;

		if (auto* p = doCast<Widget>(m_parent))
			p->markDirtyStyleChildren();
	}

	void Widget::clearLayoutDirtyRecursive()
	{
		m_layout_dirty = false;		

		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Widget* w = doCast<Widget>(it->second);
			if (w) {
				w->clearLayoutDirtyRecursive();
			}
		}
	}

	void Widget::registerDefaultInputHandlers()
	{
		// Mouse Enter/Leave for :hover

		registerEventHandler("mouse_enter", this, [this](Event* e, Unit* u)->bool {
			return handleMouseEnter(e, u);			
			});
		registerEventHandler("mouse_leave", this, [this](Event* e, Unit* u)->bool {
			return handleMouseLeave(e, u);
			});

		// Mouse Down/Up for :active

		registerEventHandler("mouse_btn_down", this, [this](Event* e, Unit* u)->bool {
			return handleMousePressed(e, u);
			});
		registerEventHandler("mouse_btn_up", this, [this](Event* e, Unit* u)->bool {
			return handleMouseReleased(e, u);			
			});

		registerEventHandler("mouse_click", this, [this](Event* e, Unit* u)->bool {			
			return onMouseClicked(e, u);
			});

		registerEventHandler("mouse_wheel", this, [this](Event* e, Unit* u)->bool {
			return onMouseWheel(e, u);
			});

		registerEventHandler("mouse_move", this, [this](Event* e, Unit* u)->bool {
			return onMouseMove(e, u);
			});

		// Focus/Blur for :focus

		registerEventHandler("focus", this, [this](Event* e, Unit* u)->bool {
			return handleFocusGained(e, u);
			});
		registerEventHandler("focus_lost", this, [this](Event* e, Unit* u)->bool {
			return handleFocusLost(e, u);
			});

		// keyboard / text events

		registerEventHandler("key_pressed", this, [this](Event* e, Unit* u)->bool {
			return onKeyPressed(e, u);
			});
		registerEventHandler("key_released", this, [this](Event* e, Unit* u)->bool {
			return onKeyReleased(e, u);
			});
		registerEventHandler("text_input", this, [this](Event* e, Unit* u)->bool {
			return onTextInput(e, u);
			});

		// drag events
		registerEventHandler("drag_start", this, [this](Event* e, Unit* u)->bool {
			return onDragStart(e, u);
			});
		registerEventHandler("drag_move", this, [this](Event* e, Unit* u)->bool {
			return onDragMove(e, u);
			});
		registerEventHandler("drag_end", this, [this](Event* e, Unit* u)->bool {
			return onDragEnd(e, u);
			});

	}

	void Widget::setWidgetState(WidgetState state, bool enable)
	{
		int state_bit = static_cast<int>(state);
		int old_states = m_widget_states;

		if (enable) {
			m_widget_states |= state_bit;
		}
		else {
			m_widget_states &= ~state_bit;
		}

		// Wenn sich der Zustand geändert hat, Styles neu berechnen
		if (old_states != m_widget_states) {
			m_style_dirty = true;
			markDirtyStyleChildren();
			invalidateRender();
		}
	}

	bool Widget::handleMouseEnter(Event* ev, Unit* sender)
	{
		setWidgetState(WidgetState::kHover, true);
		return onMouseEnter(ev, sender);
	}

	bool Widget::handleMouseLeave(Event* ev, Unit* sender)
	{
		setWidgetState(WidgetState::kHover, false);
		setWidgetState(WidgetState::kActive, false);

		return onMouseLeave(ev, sender);
	}

	bool rhea::Widget::handleMouseMove(Event* ev, Unit* sender)
	{
		return onMouseMove(ev, sender);
	}

	bool Widget::handleMousePressed(Event* ev, Unit* sender)
	{
		setWidgetState(WidgetState::kActive, true);
		return onMousePressed(ev, sender);
	}

	bool Widget::handleMouseReleased(Event* ev, Unit* sender)
	{
		setWidgetState(WidgetState::kActive, false);
		return onMouseReleased(ev, sender);
	}

	bool Widget::handleMouseWheel(Event* ev, Unit* sender)
	{
		return onMouseWheel(ev, sender);
	}

	bool Widget::handleFocusGained(Event* ev, Unit* sender)
	{
		setWidgetState(WidgetState::kFocus, true);
		return onFocusGained(ev, sender);
	}

	bool Widget::handleFocusLost(Event* ev, Unit* sender)
	{
		setWidgetState(WidgetState::kFocus, false);
		return onFocusLost(ev, sender);
	}

	void Widget::clearTransientInputState()
	{
		setWidgetState(WidgetState::kHover, false);
		setWidgetState(WidgetState::kActive, false);
	}

	void Widget::dumpDebugInfo(int indent)
	{
		std::string indent_str(indent, ' ');
		LOG(LOG_DEBUG) << indent_str << "Widget: " << getPathName() << " (" << getPath() << ")";
		LOG(LOG_DEBUG) << indent_str << "  pos=" << m_position.x << "/" << m_position.y << " size=" << m_size.x << "/" << m_size.y;
		for (const auto& modifier : m_modifiers) {
			LOG(LOG_DEBUG) << indent_str << "  Modifier: " << modifier->name();
		}
		if (m_alignment) {
			std::string h_align;
			std::string v_align;
			switch (m_alignment->getJustifyContent()) {
			case LayoutAlignment::JustifyContent::START:
				h_align = "start";
				break;
			case LayoutAlignment::JustifyContent::CENTER:
				h_align = "center";
				break;
			case LayoutAlignment::JustifyContent::END:
				h_align = "end";
				break;
			case LayoutAlignment::JustifyContent::SPACEBETWEEN:
				h_align = "space-between";
				break;
			case LayoutAlignment::JustifyContent::SPACEAROUND:
				h_align = "space-around";
				break;
			case LayoutAlignment::JustifyContent::SPACEEVENLY:
				h_align = "space-evenly";
				break;
			}
			switch (m_alignment->getAlignItems()) {
			case Alignment::AlignItems::START:
				v_align = "start";
				break;
			case Alignment::AlignItems::CENTER:
				v_align = "center";
				break;
			case Alignment::AlignItems::END:
				v_align = "end";
				break;
			case Alignment::AlignItems::STRETCH:
				v_align = "stretch";
				break;
			}
			LOG(LOG_DEBUG) << indent_str << "  Alignment: (" << h_align << "," << v_align << ")";
		}
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Widget* w = doCast<Widget>(it->second);
			if (w) {
				w->dumpDebugInfo(indent + 2);
			}
		}			
	}

}