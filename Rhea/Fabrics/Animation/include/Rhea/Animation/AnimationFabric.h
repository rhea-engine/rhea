#ifndef RHEA_ANIMATIONFABRIC_H
#define RHEA_ANIMATIONFABRIC_H

namespace Rhea
{
	class Application;

	/** Fabrics for animations
	*/
	class AnimationFabric : public Rhea::IFabric
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "Animation"; }
	};
}

#endif // !RHEA_ANIMATIONFABRIC_H
