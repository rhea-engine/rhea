#include "Realm.h"

namespace Rhea
{
	void Realm::setRealm(Orion::Realm* realm)
	{
		m_realm = realm;
		m_status = Status::LOADED;
	}

	void Realm::load(ReadyHandler handler)
	{
		// TODO
	}

	Unit* Realm::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new Realm(name);
		return ret;
	}
}