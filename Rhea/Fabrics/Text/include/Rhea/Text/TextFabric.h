#ifndef RHEA_TEXTFABRIC_H
#define RHEA_TEXTFABRIC_H

#include <Rhea/Application.h>
#include <Rhea/FabricSystem.h>

namespace rhea
{
	/** Fabrics for text handling
	*/
	class TextFabric : public rhea::IFabric
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "Text"; }
	};
}

#endif // !RHEA_TEXTFABRIC_H
