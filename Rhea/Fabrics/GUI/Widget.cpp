#include "Widget.h"
#include "Widget.h"
#include "Widget.h"
#include <cassert>
#include <Psio/Log.h>

#include "WidgetEvent.h"

#include "Application.h"
#include "UnitTree.h"
#include "Core/Scene.h"
#include "Fabrics/Media/ImageResource.h"
#include <Rhea/Core/Shader.h>

#include "../Canvas/Item2D.h"

#include "Modifier/FillSize.h"
#include "Modifier/Offset.h"
#include "Modifier/Size.h"
#include "Modifier/SizeWidth.h"
#include "Modifier/SizeHeight.h"
#include "Modifier/Padding.h"
#include "Modifier/FillWidth.h"
#include "Styles/Property.h"
#include "Styles/StyleSheet.h"
#include "Modifier/SizePercentageWidth.h"
#include "Modifier/FillHeight.h"
#include "Modifier/SizePercentageHeight.h"

namespace Rhea
{
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
		}

		// size set
		if (m_values["w"] != Variant() || m_values["h"] != Variant()) {
			Vector2 m_base_size;
			if (m_values["w"] != Variant() && m_values["h"] != Variant()) {
				m_base_size.x = getValue("w", 0.0f);
				m_base_size.y = getValue("h", 0.0f);
				m_modifiers.push_back(std::make_unique<SizeModifier>(m_base_size));
				//setStyle(StylePropertyId::kWidth, m_values["w"]);
				//setStyle(StylePropertyId::kHeight, m_values["h"]);
				m_properties[StylePropertyId::kWidth].push_back({ CssValueId::kNumeric, m_values["w"] });
				m_properties[StylePropertyId::kHeight].push_back({ CssValueId::kNumeric, m_values["h"] });
			}
			else if (m_values["w"] != Variant()) {
				m_base_size.x = getValue("w", 0.0f);
				m_modifiers.push_back(std::make_unique<SizeWidthModifier>(m_base_size.x));
				//setStyle(StylePropertyId::kWidth, m_values["w"]);
				m_properties[StylePropertyId::kWidth].push_back({ CssValueId::kNumeric, m_values["w"] });
			}
			else {
				m_base_size.y = getValue("h", 0.0f);
				m_modifiers.push_back(std::make_unique<SizeHeightModifier>(m_base_size.y));
				//setStyle(StylePropertyId::kHeight, m_values["h"]);
				m_properties[StylePropertyId::kHeight].push_back({ CssValueId::kNumeric, m_values["h"] });
			}
		}

		// padding
		Variant padding = getValue("padding");
		if (padding != Variant()) {
			if (!padding.isArray()) {
				m_modifiers.push_back(std::make_unique<PaddingModifier>(padding));
				CssValue css_padding = { CssValueId::kNumeric, SimpleParser::Variant(padding) };
				m_properties[StylePropertyId::kPaddingLeft].push_back(css_padding);
				m_properties[StylePropertyId::kPaddingTop].push_back(css_padding);
				m_properties[StylePropertyId::kPaddingRight].push_back(css_padding);
				m_properties[StylePropertyId::kPaddingBottom].push_back(css_padding);
			}
			else if (padding.length() == 4) {
				CssValue css_padding_left = { CssValueId::kNumeric, SimpleParser::Variant(padding[0])};
				CssValue css_padding_top = { CssValueId::kNumeric, SimpleParser::Variant(padding[1])};
				CssValue css_padding_right = { CssValueId::kNumeric, SimpleParser::Variant(padding[2])};
				CssValue css_padding_bottom = { CssValueId::kNumeric, SimpleParser::Variant(padding[3])};
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
				setAlignment(align[0], align[1]);
			else if (align.size() > 0)
				setAlignment(align[0], "top");

			
		}

		// set styles based on values
		if (m_values.find("font") != m_values.end())
			m_properties[StylePropertyId::kFont].push_back({ CssValueId::kString, m_values["font"] });
		if (m_values.find("texture") != m_values.end())
			m_properties[StylePropertyId::kBackgroundImage].push_back({ CssValueId::kString, m_values["texture"] });

		std::unordered_map<std::string, std::string> attr;
		std::unordered_map<std::string, Variant> val;

		if (m_values.find("onClick") != m_values.end())
			setOnClick(m_values["onClick"]);
	}

	void Widget::ready()
	{
		// prepare material
		Item2D::ready();

		RenderComponent* render_component = getComponent<RenderComponent>();
		//  widgets always need a render component
		if (!render_component) {
			render_component = addComponentUnique<RenderComponent>("renderer");			
		}

		// if we are hidden at the beggining - send the event
		if (!render_component || !render_component->isVisible()) {
			hide();
		}
	}

	void Widget::setOnClick(const std::string& target)
	{
		bool is_set = !m_onClick.empty();
		bool is_new_set = !target.empty();
		m_onClick = target;
		if (!is_set && is_new_set)
			registerEventHandler("mouse_btn_down", this, [&](Event* e, Unit* u)->bool {
			return onClicked(e, u);
				});
		if (is_set && !is_new_set)
			unregisterEventHandler("mouse_btn_down", this);
	}

	bool Widget::onClicked(Event* ev, Unit* unit)
	{
		if (m_values.find("onClick") != m_values.end()) {
			sendBoundEvent(m_values["onClick"], ev);
		}
		return false;
	}

	void Widget::setPosition(Vector2 position)
	{
		if (!getComponent<RenderComponent>() || !getComponent<RenderComponent>()->isVisible())
			return;

		// set the screen coords
		m_position = position + m_base_position;

		onSetPosition();
		if (m_material) {
		/*	DLOG(LOG_DEBUG) << "GUI::Widget: widget " << m_pathname <<
				": screen(x,y)=(" << m_position.x << ", " << m_position.y <<
				"); size(w,h)=(" << m_size.x << ", " << m_size.y << ")";
				*/
			update();
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
		m_size = Vector2(-1.0f, -1.0f);
		m_base_position = Vector2(0.0f, 0.0f);
		m_content_position = Vector2(0.0f, 0.0f);
		Vector2 margin;
		Vector2 outer_size;

		// constraints for children
		Vector2 inner_size = size_constraints;

		if (!getComponent<RenderComponent>() || !getComponent<RenderComponent>()->isVisible())
			return m_outer_size;

		// apply modifiers
		for (auto it = m_modifiers.begin(); it < m_modifiers.end(); ++it) {
			(*it)->apply(this, inner_size, m_size, outer_size, margin);
		}				

		// get the content without modifiers from the concrete widget
		Vector2 content_size = innerSize(inner_size);

		// content size 
		// is the resulting inner size (TODO handle overflow?)
		m_content_size = inner_size;

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

	void Widget::RecalculateStyle(StyleEngine* style_engine)
	{
		if (m_style_dirty) {

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

			m_style_dirty = false;
		}

		// Recursively recalculate style for all child widgets.
		children([&](std::string name, Rhea::Component* child)->bool {
			if (doCast<Widget>(child))
				doCast<Widget>(child)->RecalculateStyle(style_engine);
			return true;
			});
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
		m_font_size = -1.0f;
		m_sizing_mode_w = TextureSizingMode::kAuto;
		m_sizing_mode_h = TextureSizingMode::kAuto;

		for (const auto& property : m_computed_properties) {          
			auto property_id = property.first;  
            auto values = property.second;

			// make sure we have at least one value
			if (values.empty())
				continue;
			switch (property_id) {
			case StylePropertyId::kBackgroundImage:
				setBackgroundImage(values[0].value == "none" ? nullptr : doCast<ImageResource>(m_app->getUnitTree()->find(values[0].value)));
				break;
			case StylePropertyId::kBackgroundColor:
				if (values.size() >= 3) {
					setBackgroundColor(values[0].value, values[1].value, values[2].value, values.size() > 3 ? values[3].value : 1.0f);
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
				break;			
			}
		}
		
		ApplyTextureSizing();
		UpdateMaterial();
		sendEvent("update", nullptr);

		recreateModifiers(base_position, base_size, position_types);

		
	}

	float Widget::calculateFontSize(const CssValue& value) const
	{
		float parent_font_size = doCast<Widget>(m_parent) ? doCast<Widget>(m_parent)->getFontSize() : CssProperty::GetDefaultValue(StylePropertyId::kFontSize).value;
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
		size_t num_old_modifiers = m_modifiers.size();
		m_modifiers.clear();

		if (m_base_position.x || m_base_position.y) {
			m_modifiers.push_back(std::make_unique<OffsetModifier>(m_base_position));
		}

		// get my font size
		if (m_font_size < 0) {
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
			m_modifiers.push_back(std::make_unique<SizeModifier>(size));
		}
		else {
			if (base_size.x > 0.0f) {
				switch (position_types[2]) {
				case CssValueId::kNumeric:
					m_modifiers.push_back(std::make_unique<SizeWidthModifier>(base_size.x));
					break;
				case CssValueId::kDimensionEm:
					m_modifiers.push_back(std::make_unique<SizeWidthModifier>(base_size.x * m_font_size));
					break;
				case CssValueId::kPercentage:
					if (base_size.x < 100.0f)
						m_modifiers.push_back(std::make_unique<SizePercentageWidthModifier>(base_size.x / 100.0f));
					else
						m_modifiers.push_back(std::make_unique<FillWidthModifier>());
					break;
				}

			}
			if (base_size.y > 0.0f) {
				switch (position_types[3]) {
				case CssValueId::kNumeric:
					m_modifiers.push_back(std::make_unique<SizeHeightModifier>(base_size.y));
					break;
				case CssValueId::kDimensionEm:
					m_modifiers.push_back(std::make_unique<SizeHeightModifier>(base_size.y * m_font_size));
					break;
				case CssValueId::kPercentage:
					if (base_size.y < 100.0f)
						m_modifiers.push_back(std::make_unique<SizePercentageHeightModifier>(base_size.y / 100.0f));
					else
						m_modifiers.push_back(std::make_unique<FillHeightModifier>());
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
			m_modifiers.push_back(std::make_unique<PaddingModifier>(padding[0], padding[1], padding[2], padding[3]));

		// fillsize
		if (getValue("fillsize", false)) {
			m_modifiers.push_back(std::make_unique<FillSizeModifier>());
		}
		if (getValue("fillwidth", false)) {
			m_modifiers.push_back(std::make_unique<FillWidthModifier>());
		}

		// if we had or now have size modifiers we need a resize
		if (num_old_modifiers || !m_modifiers.empty()) {
			Event resize_ev("need_resize", this);
			bubbleEvent("need_resize", &resize_ev);
		}
	}
	void Widget::setAlignment(std::string horizontal, std::string vertical)
	{
		Alignment::Horizontal h_align = Alignment::Horizontal::START;
		Alignment::Vertical v_align = Alignment::Vertical::TOP;

		if (horizontal == "start")
			h_align = Alignment::Horizontal::START;
		else if (horizontal == "end")
			h_align = Alignment::Horizontal::END;
		else if (horizontal == "center")
			h_align = Alignment::Horizontal::CENTER;

		if (vertical == "top")
			v_align = Alignment::Vertical::TOP;
		else if (vertical == "bottom")
			v_align = Alignment::Vertical::BOTTOM;
		else if (vertical == "center")
			v_align = Alignment::Vertical::CENTER;

		m_alignment = std::make_unique<Alignment>(h_align, v_align);
	}

	void Widget::ApplyTextureSizing() {

		if (m_bg_image && m_bg_image->width > 0 && m_bg_image->height > 0 && (m_content_size.x > 1.0f || m_content_size.y>1.0f) ) 
		{
			// image and image size is set and content is not 1x1

			// tiling
			// assume background-reapat
			// if image is scaled to fit into the widget we can't also tile the image
			Math::Vector2 tiling;
			if (m_sizing_mode_w == TextureSizingMode::kAuto)
				tiling.x = max(m_content_size.x / m_bg_image->width, 1.0f); // dont tile if image is larger than content
			if (m_sizing_mode_h == TextureSizingMode::kAuto)
				tiling.y = max(m_content_size.y / m_bg_image->height, 1.0f);
			if (m_tiling != tiling) {
				m_tiling = tiling;
				m_material_dirty = true;
			}

			if (m_sizing_mode_w == TextureSizingMode::kContain) {
				LOG(LOG_DEBUG) << "ApplyTextureSizing: content=" << m_content_size.x << "/" << m_content_size.y << "; image=" << m_bg_image->width << "/" << m_bg_image->height;;
			}

			// scaling
			if (m_bg_image->width > m_content_size.x || m_bg_image->height > m_content_size.y)
			{
				Math::Vector2 scaling(1.0f, 1.0f);
				switch (m_sizing_mode_w) {
				case TextureSizingMode::kContain:
				{
					// Use the smaller scale factor for both axes to maintain aspect ratio
					float aspect_scale = min(m_content_size.x / (float)m_bg_image->width, m_content_size.y / (float)m_bg_image->height);
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
				}
			}

			UpdateMaterial();
		}		
	}

	void Widget::setStyles(CssRuleSet& styles)
	{
		for (const auto& style : styles) {
			m_properties[style.first] = style.second;
		}
		m_style_dirty = true;
	}

	void Widget::setStyle(StylePropertyId id, std::vector<CssValue> value)
	{
		m_properties[id] = value;
		m_style_dirty = true;
	}

	std::vector<CssValue>& Widget::getStyle(StylePropertyId style)
	{
		auto it = m_computed_properties.find(style);
		if (it != m_computed_properties.end())
			return it->second;
		else {
			static std::vector<CssValue> empty;
			return empty;
		}
	}
	
	void Widget::setStyle(StylePropertyId id, std::string value)
	{
		std::vector<CssValue> values;
		std::vector<SimpleParser::CssParser::Property> content;
		content.push_back({ SimpleParser::CssParser::PropertyId::kString, 0, value });
		if (StyleSheet::ParseValues(id, content, values)) {
			m_properties[id] = values;
			m_style_dirty = true;
		}
	}

	void Widget::setStyle(StylePropertyId id, int value)
	{
		std::vector<CssValue> values;
		std::vector<SimpleParser::CssParser::Property> content;
		content.push_back({ SimpleParser::CssParser::PropertyId::kNumeric, 0, value });
		if (StyleSheet::ParseValues(id, content, values)) {
			m_properties[id] = values;
			m_style_dirty = true;
		}
	}

	void Widget::setStyle(StylePropertyId id, int value, std::string dim)
	{
		std::vector<CssValue> values;
		std::vector<SimpleParser::CssParser::Property> content;
		content.push_back({ SimpleParser::CssParser::PropertyId::kDimension, 0, value, dim });
		if (StyleSheet::ParseValues(id, content, values)) {
			m_properties[id] = values;
			m_style_dirty = true;
		}
	}

	void Widget::show()
	{
		Item::show();
		Event e("show", this);
		bubbleEvent("show", &e);
	}

	void Widget::makeVisble()
	{
		Item::show();
	}

	void Widget::hide()
	{
		Item::hide();
		Event e("hide", this);
		bubbleEvent("hide", &e);
	}

	void Widget::makeInvisble()
	{
		Item::hide();
	}

	Widget* Widget::capture(int c_x, int c_y)
	{
		if (!getComponent<RenderComponent>() || !getComponent<RenderComponent>()->isVisible())
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

	Widget* Widget::getInputTarget()
	{
		// if we handle input return myself
		if (m_handle_input)
			return this;

		// if we are not a top window we have a Widget parent - check it.
		if (!m_top_window)
			return static_cast<Widget*>(m_parent)->getInputTarget();

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
}