#include <Rhea/Application.h>
#include <Rhea/Physics/Bullet/BtDebugDraw.h>

namespace Poseidon
{
	class Application : public Rhea::Application
	{
	public:
		Application() :
			core(nullptr),
			m_phys_debug(false),
			m_debug_draw(nullptr)
		{}

		// initialize
		void init();

		// setup
		void setup();

		// scene is loaded
		void onScene(Rhea::Scene *scene);

		// get the core
		Poseidon::Core* get_core() { return core; };

		// debug physics engine
		void toggleDebug();

	protected:
		Poseidon::Core* core;

		bool m_phys_debug;
		Rhea::BulletDebugDraw* m_debug_draw;
	};
}