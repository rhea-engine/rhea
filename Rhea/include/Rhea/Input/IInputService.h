#ifndef RHEA_INPUT_IINPUTSERVICE_H
#define RHEA_INPUT_IINPUTSERVICE_H

#include "InputEvent.h"
#include <Rhea/Service.h>

namespace rhea
{
    /** Abstract interface for input services
    */
	class IInputService : public Service
    {
        MECLASS_VIRTUAL(IInputService, Service)
    public:
        IInputService(std::string name, std::string type_name) :
            Service(name, type_name)
        {
        }
        virtual ~IInputService() = default;

        virtual void update() = 0;
        virtual bool pollEvent(InputEvent& evt) = 0;
    };
}

#endif