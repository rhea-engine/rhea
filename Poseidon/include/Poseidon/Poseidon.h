#include <Rhea/Application.h>
#include "Core.h"
#include <Rhea/Physics/IPhysicsDebugDraw.h>

namespace poseidon
{
	class Application : public rhea::Application
	{
	public:
		Application() :
			core(nullptr),
			m_phys_debug(false),
			m_debug_draw(nullptr)
		{}

		// initialize
		void init(int argc, char* argv[]);

		// setup
		void setup();

		// scene is loaded
		void onScene(rhea::Scene *scene);

		// get the core
		poseidon::Core* get_core() { return core; };

		// debug physics engine
		void toggleDebug();

	protected:
		poseidon::Core* core;

		bool m_phys_debug;
		rhea::IPhysicsDebugDraw* m_debug_draw;
	};
}