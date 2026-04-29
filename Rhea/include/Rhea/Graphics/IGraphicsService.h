/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_IGRAPHICSSERVICE_H
#define RHEA_IGRAPHICSSERVICE_H

#include <Rhea/Service.h>

namespace rhea
{
    /**
     * Abstract interface for graphics services.     
     */
    class IGraphicsService : public Service
    {
        MECLASS_VIRTUAL(IGraphicsService, Service)
    public:
        IGraphicsService(std::string name, std::string type) :
            Service(name, type)
		{
		}

        virtual ~IGraphicsService() = default;
        
        // request fullscreen
        virtual void ToggleFullscreen() = 0;
        
        // Event registration (needed by GUIController)
        // Inherited from Service: registerEventHandler()
    };
}
#endif