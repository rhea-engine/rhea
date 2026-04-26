#ifndef RHEA_FABRICS_TRANSFORM
#define RHEA_FABRICS_TRANSFORM

#include <string>

#include "SimpleParser/Variant.h"
#include "SimpleParser/Type.h"
#include "Rhea/Core/Position.h"

namespace rhea
{
	class Item;

	class Transform : public simpleparser::Variant
	{
	public:
		Transform(Item* parent, std::string type_name) :
			m_parent(parent),
			m_position(),
			m_derived_position(),
			m_coord_space(1),
			m_dirty(true),
			m_in_updating(false),
			simpleparser::Variant(type_name)
		{}
		Transform(Item* parent) :
			Transform(parent, "//meworla/transform")
		{}

		// update parent
		void setParent(Item* parent) { m_parent = parent; m_dirty = false; }

		// get position
		const Position* get_position() { if (m_dirty) update(); return &m_position; }
		const Position* get_derived_position() { if (m_dirty) update(); return &m_derived_position; }

		// update the position
		void update();
		void setPosition(const Position& position);

		// create a position of the given type
		void createPosition(int position_type);

		// set the coord space 0: world, 1: parent
		void setCoordSpace(int space) { m_coord_space = space; }

		// mark dirty
		void markDirty();

		// type functions (for creation and initialization)

		// create unit type
		static Transform* createType(std::string name, std::string type_name);

		// type initialization
		static void initializeType(simpleparser::Type* type);

	protected:
		Item* m_parent;
		Position m_position;
		Position m_derived_position;
		int m_coord_space;
		
		bool m_in_updating;
		bool m_dirty;
	};
}

#endif // !RHEA_FABRICS_TRANSFORM
