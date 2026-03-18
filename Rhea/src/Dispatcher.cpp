#include "Rhea/Dispatcher.h"
#include <cassert>

namespace Rhea
{
	bool Dispatcher::dispatch(std::string action, std::shared_ptr<Psio::InputEvent> evt)
	{
		auto it = m_map.find(action);
		if (it == m_map.end())
			return false;

		bool ret = false;
		for (auto& cb : it->second.cbs) {
			if (cb(action, evt))
				ret = true;		
		}
		return ret;
	}

	CBID Dispatcher::subscribeEvent(std::string action, actionHandler handler)
	{
		auto it = m_map.find(action);
		if (it == m_map.end()) {
			m_map[action] = { action };
			it = m_map.find(action);
			assert(it != m_map.end());			
		}

		auto r_it = it->second.cbs.insert(it->second.cbs.end(), handler);
		return CBID(r_it);
	}
	
	void Dispatcher::unsubscribeEbent(std::string action, CBID& cbid)
	{
		if (!cbid.valid)
			return; 
		
		auto it = m_map.find(action);
		if (it == m_map.end())
			return;

		it->second.cbs.erase(cbid.iter);


	}
}