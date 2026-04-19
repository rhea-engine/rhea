#ifndef RHEA_INTEGRATION_PSIOINPUTSERVICE_H
#define RHEA_INTEGRATION_PSIOINPUTSERVICE_H

#include "Rhea/Input/IInputService.h"
#include "RheaClient/Input/PsioInputAdapter.h"

#include <Psio/Input/Input.h>
#include <queue>
#include <memory>

namespace rhea::client
{
    class WindowRegistryService;

    class PsioInputService : public IInputService
    {
        RECLASS(PsioInputService, IInputService, "//rhea/psioinputservice")
    public:
        PsioInputService(std::string name, std::string type_name);

		void setPsioInput(psio::Input* psioInput) { m_psioInput = psioInput; }
		void setWindowRegistryService(WindowRegistryService* service) { PsioInputAdapter::m_window_service = service; }

        void update() override;
        bool pollEvent(InputEvent& evt) override;
    protected:
        psio::Input* m_psioInput = nullptr;
        std::queue<InputEvent> m_queue;        
    };
}

#endif