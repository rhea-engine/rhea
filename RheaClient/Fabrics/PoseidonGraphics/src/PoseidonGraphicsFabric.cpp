#include <Rhea/PoseidonGraphicsFabric.h>
#include <Rhea/Application.h>

#include <SimpleParser/Type.h>
#include <SimpleParser/Method.h>

#include <Rhea/Core/Material.h>
#include <Rhea/Core/Shader.h>
#include <Rhea/Core/Lighting.h>
#include <Rhea/Core/Mesh.h>
#include <Rhea/Core/Camera.h>

#include <RheaClient/Graphics/GraphicsService.h>
#include <Rhea/Platform/PlatformService.h>
#include <RheaClient/Components/RenderComponent.h>

#include <RheaClient/Input/PsioInputService.h>
#include <RheaClient/Window/WindowService.h>
#include <Rhea/Resources/ResourceList.h>
#include <Rhea/Resources/ResourceService.h>

namespace rhea::client {
    void PoseidonGraphicsFabric::Register(rhea::Application* app)
    {
        // register services
        simpleparser::Type::registerType<GraphicsService>("//meworla/graphicsservice");
    
        // --- create and add graphics service
        GraphicsService* g_service = (GraphicsService*)simpleparser::Type::create("GraphicsService", "//meworla/graphicsservice");
        g_service->setup(app);
        app->registerService("graphics", g_service);

        rhea::PlatformService* psio = static_cast<rhea::PlatformService*>(app->getService("platform"));
        g_service->setPlatform(psio);
     
        // register components
        RenderComponent::_register_RenderComponent();
        // MediaFabrics::Register(app);       
        
        // register input service
        PsioInputService* input_service = simpleparser::Type::create<PsioInputService>("PsioInputService");
		input_service->setup(app);
		app->registerService("input", input_service);

		// register window service
		WindowRegistryService* window_service = simpleparser::Type::create<WindowRegistryService>("WindowRegistryService");
		window_service->setup(app);
		app->registerService("window_registry", window_service);
		
		input_service->setPsioInput(psio->getEngine()->getInput());
        input_service->setWindowRegistryService(window_service);		
    }
}