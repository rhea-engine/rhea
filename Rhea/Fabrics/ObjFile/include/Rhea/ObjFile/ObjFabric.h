#ifndef RHEA_OBJ_FABRIC_H
#define RHEA_OBJ_FABRIC_H

#include <Rhea/Application.h>
#include <Rhea/ModuleSystem.h>

namespace Rhea
{
	class Application;

	/** Fabrics for basic image loading
	*/
	class ObjFabric : public Rhea::IFabric
	{
	public:
		void Register(Application* app) override;
		const char* getName() const override { return "Obj"; }
	};
}

#endif // !RHEA_OBJ_FABRIC_H
