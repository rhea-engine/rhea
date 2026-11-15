#include <Rhea/Core/SceneResource.h>
#include <Rhea/Core/SceneDefinitionComponent.h>
#include <Rhea/Core/Geometry.h>
#include <Rhea/Core/WorldService.h>
#include <memory>
#include "AssetViewer.h"

void AssetViewer::createAssetViewerScene(rhea::Application* app, const rhea::WorldService::OpenResourceContext& context, rhea::WorldService::AdaptResourceCompletion completion)
{
    using namespace rhea;

    if (!context.resource) {
        completion(Error(Error::Code::INVALID_DATA, "No resource provided to asset viewer",context.resource_id),nullptr);
        return;
    }

    // Prefer the complete scene when both capabilities exist.
    const bool has_scene = context.resource->getComponent<SceneDefinitionComponent>() != nullptr;

    const bool has_geometry = context.resource->getComponent<Geometry>() != nullptr;

    if (!has_scene && !has_geometry) {
        completion(Error(Error::Code::UNSUPPORTED_FORMAT, "Resource provides neither scene nor geometry", context.resource_id), nullptr);
        return;
    }

    // The loaded resource is already attached to the UnitTree.
    // Use its canonical address instead of the original input string.
    const std::string asset_id = context.resource->getPath();

    app->loadResource("//ceto/assetviewer.scene", "//meworla/scene", [this, asset_id, has_scene, app, completion = std::move(completion)](Component* component, ErrorReport report) mutable {
        if (report.isError()) {
            completion(std::move(report), nullptr);
            return;
        }

        auto* viewer_template = doCast<SceneResource>(component);
        if (!viewer_template) {
            completion(Error(Error::Code::INVALID_DATA, "Asset viewer template is not a SceneResource", "//ceto/assetviewer.scene"), nullptr);
            return;
        }

        // Copy: never modify the cached template.
        auto definition = viewer_template->getDefinition();

        // Avoid name collisions while the previous session still exists.
        const auto viewer_index = ++m_asset_viewer_index;
        const std::string viewer_name = "assetviewer_" + std::to_string(viewer_index);

        definition.root.name = viewer_name;

        auto append_node = [&definition](
            const std::string& id,
            const std::string& parent_id,
            const std::string& name,
            const std::string& type_name) -> SceneResource::NodeDefinition& {
                SceneResource::NodeDefinition node;
                node.node_id = id;
                node.parent_id = parent_id;
                node.name = name;
                node.type_name = type_name;
                definition.nodes.push_back(std::move(node));
                return definition.nodes.back();
            };

        if (has_scene) {
            auto& source = append_node(
                "asset_source", "", "asset_source",
                "//meworla/sceneresource");
            source.values["source"] = asset_id;

            auto& instance = append_node(
                "content/model", "content", "model",
                "//meworla/sceneinstance");
            instance.values["source"] = "asset_source";
        }
        else {
            auto& mesh = append_node(
                "asset_mesh", "", "asset_mesh",
                "//meworla/mesh");
            mesh.values["geometry"] = asset_id;

            auto& model = append_node(
                "content/model", "content", "model",
                "//meworla/item");
            model.values["mesh"] = "asset_mesh";
            model.values["position"] =
                std::vector<Variant>{ 0.0f, 0.0f, 0.0f };
        }

        auto viewer = std::make_unique<SceneResource>(viewer_name, "//meworla/sceneresource");
        viewer->setup(app);

        // Keep the template context for "./images/grid.png".
        // Use the asset identity for WorldService's active-source tracking
        // and its selection of the runtime parent directory.
        viewer->setDefinition(
            std::move(definition),
            viewer_template->getResourceContext(),
            asset_id);

        // Transfer ownership to the adapter caller.
        completion(ErrorReport(), viewer.release());
        });
}