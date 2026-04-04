#include <Rhea/PoseidonGraphicsFabric.h>
#include <Rhea/Application.h>

#include <SimpleParser/Type.h>
#include <SimpleParser/Method.h>

// include whatever types you register:
#include <Rhea/Core/Material.h>
#include <Rhea/Core/Shader.h>
#include <Rhea/Core/Lighting.h>
#include <Rhea/Core/Mesh.h>
#include <Rhea/Core/Camera.h>
// plus GUI/widget includes, etc.

#include <SimpleParser/Type.h>

// If GraphicsService is now in RheaClient:
#include <Rhea/Graphics/GraphicsService.h> // via shim or new include path
#include <Rhea/Platform/PlatformService.h>
#include <Rhea/Components/RenderComponent.h>

namespace rhea::client {
    void PoseidonGraphicsFabric::Register(rhea::Application* app)
    {
        // register services
        SimpleParser::Type::registerType<GraphicsService>("//meworla/graphicsservice");
        /*
     // --- register types (moved from Application::init) ---
     SimpleParser::Type::registerType<rhea::Mesh>("//meworla/mesh");
     // ... etc ...
     */

        // --- create and add graphics service
        GraphicsService* g_service = (GraphicsService*)SimpleParser::Type::create("GraphicsService", "//meworla/graphicsservice");
        g_service->setup(app);
        app->registerService("graphics", g_service);

        rhea::PlatformService* psio = static_cast<rhea::PlatformService*>(app->getService("platform"));
        g_service->setPlatform(psio);
     
        // register components
        RenderComponent::_register_RenderComponent();
        // MediaFabrics::Register(app);       
        
    }
}