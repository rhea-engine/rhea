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
			m_need_update(true),
			simpleparser::Variant(type_name)
		{}
		Transform(Item* parent) :
			Transform(parent, "//meworla/transform")
		{}

		// update parent
		void setParent(Item* parent) { m_parent = parent; m_derived_valid = false; }

		// get position
		Position* get_position() { if (m_need_update) update(); return &m_position; }
		Position* get_derived_position() { if (!m_derived_valid) update(); return &m_derived_position; }

		// update the position
		void update();

		// create a position of the given type
		void createPosition(int position_type);

		// set the coord space 0: world, 1: parent
		void setCoordSpace(int space) { m_coord_space = space; }

		// type functions

		// create unit type
		static Transform* createType(std::string name, std::string type_name);

		// type initialization
		static void initializeType(simpleparser::Type* type);

	protected:
		Item* m_parent;
		Position m_position;
		Position m_derived_position;
		int m_coord_space;
		bool m_need_update;
		bool m_in_updating = false;
		bool m_derived_valid = false;

	};
}

#endif // !RHEA_FABRICS_TRANSFORM
