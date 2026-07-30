/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_SCENERESOURCE_H
#define RHEA_SCENERESOURCE_H

#include <Rhea/Resources/Resource.h>

namespace rhea
{
    class ResourceList;

    class SceneResource : public Resource
    {
        RECLASS(SceneResource, Resource, "//meworla/sceneresource")

    public:
        struct NodeDefinition {
            std::string node_id;
            std::string parent_id;
            std::string name;
            std::string type_name;
            std::unordered_map<std::string, std::string> attributes;
            std::unordered_map<std::string, Variant> values;
        };

        struct SceneDefinition {
            NodeDefinition root;
            std::vector<NodeDefinition> nodes;
        };

        using SpawnHandler = std::function<void(ReqError, Unit*)>;

        SceneResource(std::string name, std::string type_name) :
			Resource(name, type_name)
        { }

        const SceneDefinition& getDefinition() const
        {
            return m_definition;
        }

        void setDefinition(SceneDefinition definition, const std::string& scene_root)
        {
            m_definition = std::move(definition);
			m_scene_root = scene_root;
        }

		std::string getSceneRoot() const
		{
			return m_scene_root;
		}

        // instantiate the scene resource into a unit tree under the given parent unit
        // if parent is nullptr, the scene will be instantiated as a root unit, the caller may need to add it to the unit tree manually
        void spawn(Unit* runtime_parent, SpawnHandler handler);

    private:
        void instantiate(rhea::Unit* definition_parent, SpawnHandler handler);

        SceneDefinition m_definition;
		std::string m_scene_root;

        enum class SceneReferenceMode {
            LoadOnly,
            Spawn,
            Materialize
        };

        struct PendingSceneInstance {
            std::string resource_key;
            Unit* parent;
            std::string instance_name;
			SceneReferenceMode mode;
        };
        std::vector<PendingSceneInstance> pending_instances;

		using CompletionHandler = std::function<void(ReqError)>;

        void instantiatePendingScenes(
            ResourceList& resources,
            std::vector<PendingSceneInstance> pending,
            CompletionHandler complete);
    };
}

#endif // !RHEA_SCENERESOURCE_H