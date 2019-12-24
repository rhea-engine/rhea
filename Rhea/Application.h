#ifndef RHEA_H
#define RHEA_H

#include "Poseidon/Core.h"
#include "Orion/Orion.h"

using Poseidon::Core;

namespace Rhea
{	
	class Application
	{
	protected:
		Core* core = 0;
		Orion::Orion* m_fs = 0;
	public:
		void init();
		Core* get_core() { return core; };
		virtual void setup();
		virtual int loop();
		virtual void close();
		void loadScene(const std::string filename);
	};
}
#endif // !RHEA_H
