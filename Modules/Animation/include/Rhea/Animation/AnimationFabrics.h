#ifndef RHEA_ANIMATIONFABRICS_H
#define RHEA_ANIMATIONFABRICS_H

namespace Rhea
{
	class Application;

	/** Fabrics for animations
	*/
	class AnimationFabrics : public Rhea::IFabricModule
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "Animation"; }
	};
}

#endif // !RHEA_ANIMATIONFABRICS_H
