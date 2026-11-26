#include <Rhea/Core/SceneResource.h>
#include <Rhea/Core/SceneDefinitionComponent.h>
#include <Rhea/Core/Geometry.h>
#include <Rhea/Core/WorldService.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Core/Mesh.h>
#include <Rhea/Components/MeshComponent.h>
#include <Rhea/Animation/AnimationComponent.h>
#include <Rhea/Animation/Animation.h>
#include <Rhea/Animation/Skeleton.h>
#include <Rhea/Animation/Animator.h>
#include <Rhea/Animation/AnimationState.h>
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
            mesh.values["obj_change_handedness"] = false;

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

rhea::ErrorReport AssetViewer::prepareAnimations(rhea::Application* app, rhea::Scene* scene)
{
    if (scene->getValueString("ceto_view_mode", "") != "asset")
        return rhea::ErrorReport();

    const auto root_path = scene->getValueString("ceto_content_root", "");
    rhea::Unit* content_root = doCast<rhea::Unit>(scene->find(root_path));
	if (!content_root) {
		return rhea::ErrorReport(rhea::Error(rhea::Error::Code::INVALID_DATA, "Asset viewer content root not found: " + root_path, scene->getPathName()));
	}

	// Prepare all animations in the content root.

    // gather all mesh items with skeleton and animations
    struct AnimationTarget
    {
        rhea::Item* item;
        rhea::Mesh* mesh;
        std::vector<rhea::Animation*> clips;
    };

    std::vector<AnimationTarget> targets;

    content_root->traverse([app, &targets](std::string&, rhea::Component* component) {
        auto* item = doCast<rhea::Item>(component);
        if (!item)
            return true;

        // Preserve animation behaviour already supplied by the scene.
        if (item->getComponent<rhea::AnimationComponent>())
            return true;

        auto* mesh_component = item->getComponent<rhea::MeshComponent>();
        if (!mesh_component)
            return true;

        auto* mesh = mesh_component->getMesh();
        if (!mesh)
            mesh = app->getUnitTree()->resolve(mesh_component->meshRef());

        if (!mesh || !mesh->getComponent<rhea::Skeleton>())
            return true;

        AnimationTarget target{ item, mesh,{} };

        mesh->children([&target](std::string&, rhea::Component* child) {
            if (auto* clip = doCast<rhea::Animation>(child))
                target.clips.push_back(clip);

            return true;
            },
            true);

        if (!target.clips.empty())
            targets.push_back(std::move(target));

        return true;
        });

    for (const auto& target : targets) {
        // Keep the animator local to this item instance.
        auto* animator = new rhea::Animator("preview_animator", "//meworla/animator");

        if (!target.item->addChild(animator)) {
            animator->destroy();
            return rhea::Error(rhea::Error::Code::INVALID_DEFINITION, "Could not attach preview animator", target.item->getPath());
        }

        animator->setup(app, {}, {
            { "mesh", target.mesh->getPath() },
            { "initialState", target.clips.front()->getPathName() }
            });

        for (auto* clip : target.clips) {
            auto* state = new rhea::AnimationState(clip->getPathName(), "//meworla/animationstate");

            if (!animator->addChild(state)) {
                state->destroy();
                return rhea::Error(rhea::Error::Code::INVALID_DEFINITION, "Could not attach animation state", animator->getPath());
            }

            state->setup(app, {}, {});
            state->setAnimation(clip);
        }

        auto* animation = new rhea::AnimationComponent("preview_animation", "//meworla/animationcomponent");

        if (!target.item->addChild(animation)) {
            animation->destroy();
            return rhea::Error(rhea::Error::Code::INVALID_DEFINITION, "Could not attach animation component", target.item->getPath());
        }

        animation->setup(app, {}, {
            { "initialState", target.clips.front()->getPathName() }
            });

        animation->setAnimatorRef(rhea::Ref<rhea::Animator>(animator->getPath()));

        // Initialize only the newly created objects.
        animator->ready();
        animation->ready();
    }

    return rhea::ErrorReport();
}
