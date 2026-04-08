#ifndef RHEA_GUI_WIDGET_H
#define RHEA_GUI_WIDGET_H

#include <Rhea/Unit.h>
#include <Rhea/Core/Item2D.h>
#include <Math/Vector2.h>
#include <SimpleParser/CssParser.h>
#include "Modifier/Modifier.h"
#include "Layout/Alignment.h"
#include "Styles/Property.h"
#include "Styles/StyleEngine.h"

using math::Vector2;

namespace rhea
{
	class Event;

	/**
	 * @class Widget
	 * @brief Represents a GUI widget that can handle events, manage styles, and interact with the application.
	 */
	class Widget : public Item2D {
		MECLASS(Widget, Item2D)

	public:
		/**
		 * @brief Constructs a Widget with a given path name.
		 * @param path_name The name of the widget.
		 */
		Widget(std::string path_name) :
			Widget(path_name, "//meworla/gui/widget")
		{
		}

		/**
		 * @brief Constructs a Widget with a given path name and type name.
		 * @param path_name The name of the widget.
		 * @param type_name The widget type name.
		 */
		Widget(std::string path_name, std::string type_name) :
			Item2D(path_name, type_name),
			m_top_window(false),
			m_handle_input(false),
			m_user_ptr(nullptr),
			m_base_position(0.0f, 0.0f),
			m_cur_position(0.0f, 0.0f),
			m_anchor_position(0.0f, 0.0f),
			m_font_size(0.0f),
			m_sizing_mode_w(TextureSizingMode::kAuto),
			m_sizing_mode_h(TextureSizingMode::kAuto),
			m_screen_position_valid(false),
			m_style_dirty(true),
			m_layout_dirty(true),
			m_justify_mode(0),
			m_is_rtl(false),
			m_essential_size_valid(false)
		{
		}

		/**
		 * @brief Sets up the widget with the given application, attributes, and values.
		 * @param app The application instance.
		 * @param attributes A map of attribute names and values.
		 * @param values A map of attribute names and Variant values.
		 */
		virtual void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		/**
		 * @brief Event handler called when the widget is ready.
		 * This function is triggered by the engine's ready event.
		 */
		void ready();		

		/**
		 * @brief Sets the onClick event handler target.
		 * @param target The target for the onClick event.
		 */
		void setOnClick(const std::string& target);

		/**
		 * @brief Handles the onClick event.
		 * @param ev The event instance.
		 * @param unit The unit instance.
		 * @return True if the event was handled, false otherwise.
		 */
		virtual bool onClicked(Event* ev, Unit* unit);

		/**
		 * @brief Sets the position of the widget.
		 * @param position The new position.
		 */
		void setPosition(Vector2 position);

		/**
		 * @brief Hook for subclasses called when the position is set.
		 * This function can be overridden by subclasses to perform additional actions when the widget's position is set.
		 */
		virtual void onSetPosition();

		/**
		 * @brief Calculates the size of the widget based on size constraints.
		 * @param size_constraints The size constraints.
		 * @return The calculated size.
		 */
		Vector2 size(Vector2 size_constraints);

		/**
		 * @brief Measures the inner size of the widget based on size constraints.
		 * @param size_constraints The size constraints.
		 * @return The measured inner size.
		 */
		virtual Vector2 innerSize(Vector2 size_constraints);

		/**
		 * @brief Gets the current size of the widget.
		 * @return The current size.
		 */
		Vector2 getSize() { return m_size; }

		/**
		 * @brief Sets the current size of the widget.
		 * @param size The new size.
		 */
		void setSize(Vector2 size) { m_size = size; }

		/**
		 * @brief Gets the minimal size for container layouts based on size constraints.
		 * @param size_constraints The size constraints.
		 * @return The minimal size.
		 */
		Vector2 getMinSize(Vector2 size_constraints);

		/**
		 * @brief Measures the minimal inner size of the widget based on size constraints.
		 * @param size_constraints The size constraints.
		 * @param min_size The measured minimal size.
		 */
		virtual void innerGetMinSize(Vector2 size_constraints, Vector2& min_size);

		/**
		 * @brief Gets the outer size of the widget.
		 * @return The outer size.
		 */
		Vector2 getOuterSize() { return m_outer_size; }

		/**
		 * @brief Gets the containing block size of the widget.
		 * @return The containing block size.
		 */
		Vector2 getContainingBlock() { return m_content_size; }

		/**
		 * @brief Gets the base position of the widget (relative to parent).
		 * @return The base position.
		 */
		Vector2 getBasePosition() { return m_base_position; }

		/**
		 * @brief Sets the base position of the widget (relative to parent).
		 * @param pos The new base position.
		 */
		void setBasePosition(Vector2 pos) { m_base_position = pos; }

		/**
		 * @brief Gets the content position of the widget (relative to base position).
		 * @return The content position.
		 */
		Vector2 getContentPosition() { return m_content_position; }

		/**
		 * @brief Sets the content position of the widget (relative to base position).
		 * @param pos The new content position.
		 */
		void setContentPosition(Vector2 pos) { m_content_position = pos; }

		// Invalidate the layout, causing it to be recalculated on the next render.
		virtual void invalidateLayout();

		/**
		 * @brief Recalculates the style of the widget using the given style engine.
		 * @param style_engine The style engine instance.
		 */
		void RecalculateStyle(StyleEngine* style_engine);

		/**
		 * @brief Appends styles to the widget.
		 * @param styles The styles to append.
		 */
		void AppendStyles(CssRuleSet& styles);

		/**
		 * @brief Applies the styles to the widget.
		 */
		virtual void ApplyStyles();		

		/**
		 * @brief Sets the alignment of the widget.
		 * @param horizontal The horizontal alignment.
		 * @param vertical The vertical alignment.
		 */
		void setAlignment(std::string horizontal, std::string vertical);

		 /**
		  * @brief Sets the justify mode of the widget.
		  * @param mode The justify mode (0 for no justification, 1 for space evenly).
		  */void setJustifyMode(int mode) { m_justify_mode = mode; }

		/**
		 * @brief Sets the styles of the widget.
		 * @param styles The styles to set.
		 */
		void setStyles(CssRuleSet& styles);

		/**
		 * @brief Sets a style property of the widget.
		 * @param id The style property ID.
		 * @param value The style property value.
		 */
		void setStyle(StylePropertyId id, std::vector<CssValue> value);

		/**
		 * @brief Gets a style property of the widget.
		 * @param style The style property ID.
		 * @return The style property value.
		 */
		std::vector<CssValue>& getStyle(StylePropertyId style);

		/**
		 * @brief Sets a style property of the widget using a string value.
		 * @param id The style property ID.
		 * @param value The string value.
		 */
		void setStyle(StylePropertyId id, std::string value);

		/**
		 * @brief Sets a style property of the widget using a numeric value.
		 * @param id The style property ID.
		 * @param value The numeric value.
		 */
		void setStyle(StylePropertyId id, int value);

		/**
		 * @brief Sets a style property of the widget using a dimension value.
		 * @param id The style property ID.
		 * @param value The numeric value.
		 * @param dim The dimension unit.
		 */
		void setStyle(StylePropertyId id, int value, std::string dim);

		/**
		 * @brief Gets the CSS selector for the widget.
		 * @return The CSS selector.
		 */
		virtual const std::string getSelector() { return "Widget"; }

		/**
		 * @brief Gets the font size of the widget.
		 * @return The font size.
		 */
		float getFontSize() { return m_font_size; }

		/**
		 * @brief Sets the widget to be at the top level (not a child of another widget).
		 * @param top True if the widget is at the top level, false otherwise.
		 */
		void setToplevel(bool top) { m_top_window = top; }

		/**
		 * @brief Shows the widget.
		 */
		void show();

		/**
		 * @brief Makes the widget visible without sending an event.
		 */
		void makeVisble();

		/**
		 * @brief Hides the widget.
		 */
		void hide();

		/**
		 * @brief Makes the widget invisible without sending an event.
		 */
		void makeInvisble();

		/**
		 * @brief Captures the lowest widget at the given position.
		 * @param x The x-coordinate.
		 * @param y The y-coordinate.
		 * @return The captured widget.
		 */
		Widget* capture(int x, int y);

		/**
		 * @brief Gets the input target widget.
		 * @return The input target widget.
		 */
		Widget* getInputTarget();

		/**
		 * @brief Sets whether the widget handles input events.
		 * @param handle True if the widget handles input events, false otherwise.
		 */
		void setHandleInput(bool handle) { m_handle_input = handle; }

		/**
		 * @brief Sends a bound event to the widget.
		 * @param binding The event binding.
		 * @param ev The event instance.
		 */
		void sendBoundEvent(std::string binding, Event* ev);

		/**
		 * @brief Sets a user pointer for the widget.
		 * @param ptr The user pointer.
		 */
		void setUserPtr(void* ptr) { m_user_ptr = ptr; }

		/**
		 * @brief Gets the user pointer of the widget.
		 * @return The user pointer.
		 */
		void* getUserPtr() { return m_user_ptr; }

	protected:

		/**
		 * @brief Gets the screen position of the widget.
		 * @return The screen position.
		 */
		Vector2 getScreenPosition();

		/**
		 * @brief calculates the font size.
		 */
		float calculateFontSize(const CssValue& value) const;

		/**
		 * @brief Recalculates the modifiers of the widget.
		 */
		void recreateModifiers(const Vector2& base_position, const Vector2& base_size, const CssValueId position_types[4]);

		// Whether this is a top window
		bool m_top_window;

		// Does widget handle input events
		bool m_handle_input;

		// Defined coordinates

		// Layout authored/style/modifier result relative to parent
		Vector2 m_base_position;

		// Parent-assigned position before widget-local adjustments
		Vector2 m_anchor_position;

		// Position of content area relative to parent
		Vector2 m_content_position;
		// Size of content area
		Vector2 m_content_size;

		// Outer size of element including margin
		Vector2 m_outer_size;

		// Cached font size
		float m_font_size;

		// Recursively clear the layout dirty flag for this widget and all its children.
		virtual void clearLayoutDirtyRecursive();

		// Scaling the background-image
		enum class TextureSizingMode {
			kAuto,      // No scaling
			kContain,   // Keep aspect ratio, fit inside widget
			kCover,		// Scale to fit widget without preserving aspect ratio
			kFixed      // No scaling, only tiling applies
		};
		// Cached sizing mode
		TextureSizingMode m_sizing_mode_w;
		TextureSizingMode m_sizing_mode_h;
		void ApplyTextureSizing();

		Vector2 m_cached_min_size;
		Vector2 m_cached_max_size;
		bool m_essential_size_valid;

		std::vector<std::unique_ptr<Modifier>> m_modifiers;

		// Justify mode
		int m_justify_mode;

		// Current coordinates

		// Current screen position in pixel
		Vector2 m_cur_position;

		// Screen space
		bool m_screen_position_valid;

		// Layout
		std::unique_ptr<Alignment> m_alignment;

		// A user pointer
		void* m_user_ptr;

		// My styles
		CssRuleSet m_properties;
		// Holds the computed style properties for this widget.
		CssRuleSet m_computed_properties;
		// Store if style is changed
		bool m_style_dirty;
		// Store if layout is changed
		bool m_layout_dirty;

		// RTL layout
		bool m_is_rtl;

		// Event handler cache
		std::string m_onClick;
	};
}

#endif // !RHEA_GUI_WINDOW_H
