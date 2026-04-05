#ifndef RHEA_ANIMATIONFABRIC_H
#define RHEA_ANIMATIONFABRIC_H

namespace rhea
{
	class Application;

	/** Fabrics for animations
	*/
	class AnimationFabric : public rhea::IFabric
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "Animation"; }
	};
}

#endif // !RHEA_ANIMATIONFABRIC_H
