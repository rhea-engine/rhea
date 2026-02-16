#include <Rhea/PoseidonGraphicsFabric.h>
#include <Rhea/Application.h>

#include <SimpleParser/Type.h>
#include <SimpleParser/Method.h>

// include whatever types you register:
#include <Rhea/Core/Material.h>
#include <Rhea/Core/Shader.h>
#include <Rhea/Core/Lighting.h>
#include <Rhea/Fabrics/3D/Mesh.h>
#include <Rhea/Fabrics/3D/Camera.h>
// plus GUI/widget includes, etc.

#include <SimpleParser/Type.h>

// If GraphicsService is now in RheaClient:
#include <Rhea/Graphics/GraphicsService.h> // via shim or new include path

namespace Rhea
{
    void PoseidonGraphicsFabric::Register(Rhea::Application* app)
    {




        // --- register types (moved from Application::init) ---
        SimpleParser::Type::registerType<Mesh>("//meworla/mesh");
        // ... etc ...

        // register components
        RenderComponent::_register_RenderComponent();
        // MediaFabrics::Register(app);

        // --- create/add graphics service (if you want it auto-created) ---
        auto* gfx = (GraphicsService*)SimpleParser::Type::create("GraphicsService", "//meworla/graphicsservice");
        gfx->setup(app);
        app->addService("graphics", gfx);  // or however you store services
        
    }
}