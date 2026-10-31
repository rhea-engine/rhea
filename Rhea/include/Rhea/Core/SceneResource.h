/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_SCENERESOURCE_H
#define RHEA_SCENERESOURCE_H

#include <Rhea/Resources/Resource.h>
#include <Rhea/Error.h>
#include <unordered_map>
#include <vector>
#include <functional>
#include <string>

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

		using SpawnHandler = std::function<void(ErrorReport, Unit*)>;
		using MaterializeHandler = std::function<void(ErrorReport, Unit*)>;

		SceneResource(std::string name, std::string type_name) :
			Resource(name, type_name)
		{}

		const SceneDefinition& getDefinition() const
		{
			return m_definition;
		}

		void setDefinition(SceneDefinition definition, const std::string& scene_root, const std::string& context_root, const std::string& source_resource_id)
		{
			m_definition = std::move(definition);
			m_scene_root = scene_root;
			m_context_root = context_root;
			m_source_resource_id = source_resource_id;
			// get base of the resource id for relative references
			if (source_resource_id.empty()) {
				m_base_id = "";
			}
			else {
				size_t pos = source_resource_id.find_last_of("/");
				if (pos == std::string::npos) {
					m_base_id = "";
				}
				else {
					m_base_id = source_resource_id.substr(0, pos);
				}
			}
		}

		const std::string& getSourceResourceId() const { return m_source_resource_id; }

		std::string getSceneRoot() const
		{
			return m_scene_root;
		}

		std::string getContextRoot() const
		{
			return m_context_root;
		}

		std::string getBaseId() const
		{
			return m_base_id;
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
			ErrorReport* error = nullptr);

		// Build a complete detached tree without calling ready().
		void buildInstance(const InstanceRequest& request, InstanceHandler handler) override;

	private:
		SceneDefinition m_definition;
		std::string m_source_resource_id; // Resolved source file, independent of instance names.
		std::string m_scene_root; // Directory of this scene file
		std::string m_context_root; // Root inherited from the entry context
		std::string m_base_id; // Base resource id for relative resource references

		struct PendingSceneInstance {
			std::string resource_key;
			Unit* parent;
			std::string instance_name;
			Component::ResourceUsage usage = Component::ResourceUsage::Load;
		};
		using CompletionHandler = std::function<void(ErrorReport)>;

		void instantiatePendingScenes(
			ResourceList& resources,
			std::vector<PendingSceneInstance> pending,
			CompletionHandler complete);

		bool m_materializing = false;
		std::vector<MaterializeHandler> m_waiting_materialize_handlers;
		Unit* m_materialized_instance = nullptr;

		void completeMaterialization(ErrorReport error, Unit* instance);

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
