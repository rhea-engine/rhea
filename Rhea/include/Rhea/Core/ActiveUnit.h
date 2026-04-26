#ifndef RHEA_ACTIVEUNIT_H
#define RHEA_ACTIVEUNIT_H

#include <Rhea/Unit.h>

namespace rhea
{
	/** ActiveUnit is the base class for all units that can be receive updates	
	*/
	class ActiveUnit : public Unit
	{
		RECLASS_VIRTUAL(ActiveUnit, Unit, "//meworla/activeunit")
	public:
		ActiveUnit(std::string name, std::string type_name) :			
			Unit(name, type_name)
		{}

		virtual void onFrame(float delta) = 0;
	};
}
#endif // !RHEA_ACTIVEUNIT_H
