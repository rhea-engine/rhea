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
		~SceneResource() override;

		struct SceneDefinition {
			NodeDefinition root;
			std::vector<NodeDefinition> nodes;
		};

		using SpawnHandler = std::function<void(ReqError, Unit*)>;
		using MaterializeHandler = std::function<void(ReqError, Unit*)>;

		SceneResource(std::string name, std::string type_name) :
			Resource(name, type_name)
		{}

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

		// Build, attach and ready a runtime instance.
		void spawn(Unit* parent, const std::string& name, SpawnHandler handler);
		void spawn(Unit* parent, SpawnHandler handler)
		{
			spawn(parent, "", std::move(handler));
		}

		void ensureMaterialized(MaterializeHandler handler);
		void ensureReady(InstanceHandler handler) override
		{
			ensureMaterialized(std::move(handler));
		}

		// Build and retain a detached, non-ready prototype and load all
		// dependencies required for synchronous instantiation.
		void prepare(PrepareHandler handler) override;

		Unit* spawnPrepared(
			Unit* parent,
			const std::string& name,
			ReqError* error = nullptr);

		// Build a complete detached tree without calling ready().
		void buildInstance(const InstanceRequest& request, InstanceHandler handler) override;

	private:
		SceneDefinition m_definition;
		std::string m_scene_root;

		struct PendingSceneInstance {
			std::string resource_key;
			Unit* parent;
			std::string instance_name;
			Component::ResourceUsage usage = Component::ResourceUsage::Load;
		};
		using CompletionHandler = std::function<void(ReqError)>;

		void instantiatePendingScenes(
			ResourceList& resources,
			std::vector<PendingSceneInstance> pending,
			CompletionHandler complete);

		bool m_materializing = false;
		std::vector<MaterializeHandler> m_waiting_materialize_handlers;
		Unit* m_materialized_instance = nullptr;

		void completeMaterialization(ReqError error, Unit* instance);

		enum class PrepareState {
			Unprepared,
			Preparing,
			Prepared,
			Failed
		};

		PrepareState m_prepare_state = PrepareState::Unprepared;

		std::vector<PrepareHandler> m_prepare_handlers;
		rhea::Unit* m_prepared_prototype = nullptr;
	};
}

#endif // !RHEA_SCENERESOURCE_H
