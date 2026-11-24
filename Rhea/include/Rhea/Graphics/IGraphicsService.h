/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_IGRAPHICSSERVICE_H
#define RHEA_IGRAPHICSSERVICE_H

#include <Rhea/Service.h>
#include <string>

namespace rhea
{
    class Camera;
    class View;

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

        virtual bool setCamera(const std::string& wind_id, const std::string& vp_id, const std::string& layer_id, Camera* camera) = 0;
		virtual bool getViewSize(const std::string& wind_id, const std::string& vp_id, const std::string& layer_id, int& width, int& height) = 0;

        // Event registration (needed by GUIController)
        // Inherited from Service: registerEventHandler()
    };
}
#endif