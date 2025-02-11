#include "Widget.h"
#include <cassert>
#include <Psio/Log.h>

#include "Widget.h"
#include "WidgetEvent.h"

#include "Application.h"
#include "UnitTree.h"
#include "Fabrics/Core/Scene.h"
#include "Fabrics/Media/ImageResource.h"
#include <Rhea/Fabrics/Core/Shader.h>

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
			if (m_values["w"] != Variant() && m_values["h"] != Variant()) {
				m_base_size.x = getValue("w", 0.0f);
				m_base_size.y = getValue("h", 0.0f);
				m_modifiers.push_back(std::make_unique<SizeModifier>(m_base_size));
			}
			else if (m_values["w"] != Variant()) {
				m_base_size.x = getValue("w", 0.0f);
				m_modifiers.push_back(std::make_unique<SizeWidthModifier>(m_base_size.x));
			}
			else {
				m_base_size.y = getValue("h", 0.0f);
				m_modifiers.push_back(std::make_unique<SizeHeightModifier>(m_base_size.y));
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
			Alignment::Horizontal h_align = Alignment::Horizontal::START;
			Alignment::Vertical v_align = Alignment::Vertical::TOP;
			if (align.size() > 0) {
				if (align[0] == "start")
					h_align = Alignment::Horizontal::START;
				else if (align[0] == "end")
					h_align = Alignment::Horizontal::END;
				else if (align[0] == "center")
					h_align = Alignment::Horizontal::CENTER;
			}
			if (align.size() > 1) {
				if (align[1] == "top")
					v_align = Alignment::Vertical::TOP;
				else if (align[1] == "bottom")
					v_align = Alignment::Vertical::BOTTOM;
				else if (align[1] == "center")
					v_align = Alignment::Vertical::CENTER;
			}
			m_alignment = std::make_unique<Alignment>(h_align, v_align);
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

	void Widget::setOnClick(std::string target)
	{
		bool is_set = (m_onClick != "");
		bool is_new_set = (target != "");
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
				w->setPosition(m_position);
			}
		}
	}

	// compute widget size
	// apply all modifiers	
	// call internal size
	// compute and return outer_size
	Vector2 Widget::size(Vector2 size_constraints)
	{
		m_size = Vector2(0.0f, 0.0f);
		m_base_position = Vector2(0.0f, 0.0f);

		// reported size to parent
		m_outer_size = Vector2(0.0f, 0.0f);
		// constraints for children
		Vector2 inner_size = size_constraints;

		if (!getComponent<RenderComponent>() || !getComponent<RenderComponent>()->isVisible())
			return m_outer_size;

		// apply modifiers
		for (auto it = m_modifiers.begin(); it < m_modifiers.end(); ++it) {
			(*it)->apply(this, inner_size, m_size, m_outer_size);
		}
		Vector2 child_size = innerSize(inner_size);

		// enlarge size and m_outer_size if child is larger than size;
		float child_delta_x = child_size.x - m_size.x;
		if (child_delta_x > 0.0f) {
			m_size.x = child_size.x;
			m_outer_size.x += child_delta_x;
		}
		float child_delta_y = child_size.y - m_size.y;
		if (child_delta_y > 0.0f) {
			m_size.y = child_size.y;
			m_outer_size.y += child_delta_y;
		}

		// report only the constraint size
		if (m_outer_size.x > size_constraints.x)
			m_outer_size.x = size_constraints.x;
		if (m_outer_size.y > size_constraints.y)
			m_outer_size.y = size_constraints.y;

		return m_outer_size;
	}

	Vector2 Widget::innerSize(Vector2 size_constraints)
	{
		Vector2 max_child_size = Vector2(0.0f, 0.0f);
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
		for (auto it = m_computed_properties.begin(); it != m_computed_properties.end(); ++it) {
			if (it->first == StylePropertyId::kBackgroundImage && it->second.size()) {
				std::string resname = it->second[0].value;
				if (!resname.compare("none")) {
					if (m_material) {
						// remove background image
						m_material->sendEvent("delete", nullptr);
						delete m_material;
						m_material = nullptr;
						if (getComponent<RenderComponent>()) {
							Mesh* mesh = getComponent<RenderComponent>()->getMesh();
							if (mesh) {
								mesh->sendEvent("delete", nullptr);
								removeChild(mesh);
								delete mesh;
								getComponent<RenderComponent>()->setMesh(nullptr);
							}
						}
					}
				}
				else if (!m_material) {
					std::unordered_map<std::string, std::string> attr;
					std::unordered_map<std::string, Variant> val;

					Unit* tex = doCast<Unit>(m_app->getUnitTree()->find(resname));
					if (tex) {

						// create dummy material
						m_material = new Material(m_path + "_material");
						m_material->setup(m_app);
						m_material->setValue("shader", "widget");						

						// get the shader						
						if (m_values.find("shader") == m_values.end()) {
							// set a default
							m_values["shader"] = "//rhea/shaders/widget";
						}
						Shader* shader_res = doCast<Shader>(m_app->getUnitTree()->find(m_values["shader"]));
						if (shader_res) {
							m_material->setShaderRes(shader_res);
						}
						else {
							LOG(LOG_ERROR) << "item2d::ready(): Shader '" << (std::string)m_values["shader"] << "' for " << m_pathname << " not found";
							return;
						}
						m_material->setTexture(0, static_cast<ImageResource*>(tex));

						RenderComponent* render_component = getComponent<RenderComponent>();
						if (!render_component) {
							render_component = addComponentUnique<RenderComponent>("renderer");							
						}

						// add the mesh
						if (!render_component->getMesh()) {

							std::unordered_map<std::string, std::string> attributes;
							std::unordered_map<std::string, Variant> values;							
							Mesh* mesh = m_app->getUnitTree()->createUnit<Mesh>(m_pathname + "_mesh", attributes, values);
							mesh->setMaterial(m_material);
							addChild(mesh);
							render_component->setMesh(mesh);

							setMesh();
						}
						else
							render_component->getMesh()->setMaterial(m_material);
						sendEvent("update", nullptr);
					}
				}
				else if (m_values["texture"] == Variant()) {
					DLOG(LOG_DEBUG) << "TODO: Handle owverwriting of texture, if not set on widget";
				}
			}
		}

		// re-create modifiers

		if (m_modifiers.size())
			m_modifiers.clear();
		if (m_base_position.x || m_base_position.y) {
			m_modifiers.push_back(std::make_unique<OffsetModifier>(m_base_position));
		}

		// size set
		if (m_base_size.x > 0.0f && m_base_size.y > 0.0f) {
			m_modifiers.push_back(std::make_unique<SizeModifier>(m_base_size));
		}
		else if (m_base_size.x > 0.0f) {				
			m_modifiers.push_back(std::make_unique<SizeWidthModifier>(m_base_size.x));
		}
		else if (m_base_size.y > 0.0f) {				
			m_modifiers.push_back(std::make_unique<SizeHeightModifier>(m_base_size.y));
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
	}

	void Widget::setStyles(CssRuleSet& styles)
	{
		for (const auto& style : styles) {
			m_properties[style.first] = style.second;
		}
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

	void Widget::setStyle(StylePropertyId id, SimpleParser::Variant value)
	{
		StyleSheet css("intern");
		std::vector<CssValue> values;
		std::vector<SimpleParser::CssParser::Property> content;
		content.push_back({ SimpleParser::CssParser::PropertyId::kString, 0, value });
		if (css.ParseValues(id, content, values))
			m_properties[id] = values;
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
		for (auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			Widget* w = doCast<Widget>(it->second);
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