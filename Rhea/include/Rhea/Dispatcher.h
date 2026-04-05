#ifndef REHA_DISPATCHER_H
#define REHA_DISPATCHER_H

#include <functional>
#include <string>
#include <memory>
#include <list>
#include <unordered_map>

namespace psio
{
	class InputEvent;
}

namespace rhea
{
    class CBID;

	class Dispatcher
	{
	public:
        typedef std::function<bool(std::string, std::shared_ptr<psio::InputEvent>)> actionHandler;
        typedef std::vector<actionHandler> handler_v;

        struct action_list {
            std::string action;
            std::list<actionHandler> cbs;
        };

        // dispatch an event
        bool dispatch(std::string action, std::shared_ptr<psio::InputEvent> evt);

		CBID subscribeEvent(std::string action, actionHandler handler);
        void unsubscribeEbent(std::string action, CBID& cbid);

    protected:
        std::unordered_map<std::string, action_list> m_map;
	};

    class CBID
    {
    public:
        CBID() : valid(false) {}
        
    private:
        friend class Dispatcher;
        
        CBID(std::list<Dispatcher::actionHandler>::iterator i)
            : iter(i), valid(true)
        {}
        std::list<Dispatcher::actionHandler>::iterator iter;
        bool valid;
    };

}

#endif // !REHA_DISPATCHER_H
