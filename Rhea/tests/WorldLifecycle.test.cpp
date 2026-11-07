/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <gtest/gtest.h>

#include <Rhea/Application.h>
#include <Rhea/Core/SceneResource.h>
#include <Rhea/Core/WorldService.h>
#include <Rhea/Resources/ResourceService.h>
#include <Rhea/UnitTree.h>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// Application.cpp expects this entry point. The fixture registers only core types.
std::vector<std::unique_ptr<rhea::IFabric>> createFabrics()
{
    return {};
}

namespace
{
    class TestApplication : public rhea::Application
    {
    public:
        void initializeTree()
        {
            m_unit_tree = std::make_unique<rhea::UnitTree>("/", "//core/unittree");
            m_unit_tree->setup(this);
        }

        void onScene(rhea::Scene*) override {}
    };

    class TestWorldService : public rhea::WorldService
    {
    public:
        using WorldService::WorldService;

        uint64_t activeSession() const { return m_session_generation; }
        uint64_t pendingGeneration() const { return m_open_attempt.generation; }

        size_t managedScenesIn(uint64_t session) const
        {
            return std::count_if(m_managed_scenes.begin(), m_managed_scenes.end(),
                [session](const auto& entry) {
                    return entry.second.session_generation == session;
                });
        }

        bool hasCandidateScenes(uint64_t generation) const
        {
            return m_candidate_scenes.count(generation) != 0;
        }
    };

    enum class RootForm { Standalone, Embedded, External };

    struct Completion
    {
        int calls = 0;
        rhea::ErrorReport report;

        rhea::WorldService::OpenHandler handler()
        {
            return [this](rhea::ErrorReport result) {
                ++calls;
                report = std::move(result);
            };
        }

        bool hasError(rhea::Error::Code code) const
        {
            return std::any_of(report.errors.begin(), report.errors.end(),
                [code](const auto& error) { return error.getCode() == code; });
        }
    };

    class WorldLifecycleTest : public testing::TestWithParam<RootForm>
    {
    protected:
        void SetUp() override
        {
            using namespace rhea;
            static const bool registered = [] {
                simpleparser::Type::registerType<Component>("//core/types/component");
                simpleparser::Type::registerType<Unit>("//core/types/unit");
                simpleparser::Type::registerType<UnitTree>("//core/unittree");
                simpleparser::Type::registerType<Resource>("//meworla/resource");
                simpleparser::Type::registerType<Scene>("//meworla/scene");
                simpleparser::Type::registerType<WorldService>("//meworla/worldservice");
                simpleparser::Type::registerType<ResourceService>("//meworla/resourceservice");
                SceneResource::_register_SceneResource();
                World::_register_World();
                return true;
            }();
            (void)registered;

            app.initializeTree();
            auto* resources = new ResourceService("resources", "//meworla/resourceservice");
            resources->setup(&app);
            app.registerService("resources", resources);

            worlds = new TestWorldService("world", "//meworla/worldservice");
            worlds->setup(&app);
            app.registerService("world", worlds);
            worlds->onInit();

            auto realm = std::make_unique<Unit>("lifecycle-test", "//core/types/unit");
            realm->setup(&app);
            ASSERT_TRUE(app.getUnitTree()->addChild(realm.get()));
            parent = realm.release();

            addDefinition("active.scene", "active_world", RootForm::Embedded);
            addDefinition("candidate.scene", "candidate", GetParam());
            if (GetParam() == RootForm::External)
                addDefinition("initial.scene", "initial", RootForm::Standalone);

            worlds->registerEventHandler("world_session_changed", this,
                [this](Event*, Unit*) { ++session_events; return false; });
            worlds->registerEventHandler("primary_scene_changed", this,
                [this](Event*, Unit*) { ++primary_events; return false; });

            Completion opened;
            worlds->openWorld("realm://lifecycle-test/active.scene", opened.handler());
            ASSERT_EQ(opened.calls, 1);
            ASSERT_FALSE(opened.report.isError());
            ASSERT_NE(worlds->getWorld(), nullptr);
            ASSERT_NE(worlds->getPrimaryScene(), nullptr);
            ASSERT_EQ(worlds->pendingGeneration(), 0u);
        }

        void addDefinition(const std::string& filename, const std::string& name, RootForm form)
        {
            rhea::SceneResource::SceneDefinition definition;
            definition.root.name = name;
            definition.root.type_name =
                form == RootForm::Standalone ? "//meworla/scene" : "//meworla/world";

            if (form == RootForm::Embedded) {
                definition.root.values.emplace("startscene", std::string("initial"));
                rhea::SceneResource::NodeDefinition initial;
                initial.node_id = "initial";
                initial.name = "initial";
                initial.type_name = "//meworla/scene";
                definition.nodes.push_back(std::move(initial));
            } else if (form == RootForm::External) {
                definition.root.values.emplace("startscene", std::string("/initial.scene"));
            }

            // Seed parsed resources under their real cache addresses. Application::loadResource,
            // SceneResource::spawn and all lifecycle code still execute normally.
            auto resource = std::make_unique<rhea::SceneResource>(
                filename, "//meworla/sceneresource");
            resource->setup(&app);
            resource->setDefinition(std::move(definition),
                "realm://lifecycle-test/", "realm://lifecycle-test/",
                "//lifecycle-test/" + filename);
            ASSERT_TRUE(parent->addChild(resource.get()));
            resource.release();
        }

        TestApplication app;
        TestWorldService* worlds = nullptr;
        rhea::Unit* parent = nullptr;
        int session_events = 0;
        int primary_events = 0;
    };

    TEST_P(WorldLifecycleTest, RejectsOverlapAndRetriesAfterPrepareFailure)
    {
        const auto* previous_world = worlds->getWorld();
        const auto* previous_scene = worlds->getPrimaryScene();
        const auto previous_session = worlds->activeSession();
        const auto previous_session_events = session_events;
        const auto previous_primary_events = primary_events;

        Completion failed;
        Completion rejected;
        Completion retried;
        rhea::WorldService::PrepareCompletion paused;
        bool pause_once = true;
        uint64_t candidate_session = 0;
        worlds->addSceneTransitionPrepareHandler(
            [&](rhea::WorldService::SceneTransitionContext* context,
                rhea::WorldService::PrepareCompletion completion) {
                EXPECT_NE(context->target_scene, nullptr);
                if (std::exchange(pause_once, false)) {
                    candidate_session = previous_session + 1;
                    paused = std::move(completion);
                    return;
                }
                completion({});
            });

        const std::string rid = "realm://lifecycle-test/candidate.scene";
        worlds->openWorld(rid, failed.handler());
        ASSERT_TRUE(paused);
        ASSERT_EQ(failed.calls, 0);
        ASSERT_NE(parent->find("candidate", false), nullptr);
        const auto pending_generation = worlds->pendingGeneration();
        ASSERT_NE(pending_generation, 0u);
        EXPECT_EQ(worlds->managedScenesIn(candidate_session), 1u);
        EXPECT_EQ(worlds->getWorld(), previous_world);
        EXPECT_EQ(worlds->getPrimaryScene(), previous_scene);
        EXPECT_EQ(worlds->activeSession(), previous_session);

        worlds->openWorld(rid, rejected.handler());
        EXPECT_EQ(rejected.calls, 1);
        EXPECT_TRUE(rejected.hasError(rhea::Error::Code::UNAVAILABLE));
        EXPECT_EQ(worlds->pendingGeneration(), pending_generation);
        EXPECT_EQ(failed.calls, 0);

        // Release the actual transition prepare callback only after both requests.
        paused(rhea::Error(rhea::Error::Code::INVALID_DATA, "Injected prepare failure"));
        ASSERT_EQ(failed.calls, 1);
        EXPECT_TRUE(failed.hasError(rhea::Error::Code::INVALID_DATA));
        EXPECT_EQ(worlds->pendingGeneration(), 0u);
        EXPECT_FALSE(worlds->hasCandidateScenes(pending_generation));
        EXPECT_EQ(worlds->managedScenesIn(candidate_session), 0u);
        EXPECT_EQ(parent->find("candidate", false), nullptr);
        EXPECT_EQ(parent->find("initial", false), nullptr);
        EXPECT_NE(parent->find("candidate.scene", false), nullptr);
        EXPECT_EQ(worlds->getWorld(), previous_world);
        EXPECT_EQ(worlds->getPrimaryScene(), previous_scene);
        EXPECT_EQ(worlds->activeSession(), previous_session);
        EXPECT_EQ(session_events, previous_session_events);
        EXPECT_EQ(primary_events, previous_primary_events);

        worlds->openWorld(rid, retried.handler());
        ASSERT_EQ(retried.calls, 1);
        ASSERT_FALSE(retried.report.isError());
        EXPECT_EQ(worlds->pendingGeneration(), 0u);
        EXPECT_NE(worlds->activeSession(), previous_session);
        ASSERT_NE(worlds->getPrimaryScene(), nullptr);
        EXPECT_NE(worlds->getPrimaryScene(), previous_scene);
        EXPECT_EQ(session_events, previous_session_events + 1);
        EXPECT_EQ(primary_events, previous_primary_events + 1);

        const auto* committed_scene = worlds->getPrimaryScene();
        paused({});
        EXPECT_EQ(failed.calls, 1);
        EXPECT_EQ(retried.calls, 1);
        EXPECT_EQ(worlds->getPrimaryScene(), committed_scene);
        EXPECT_EQ(primary_events, previous_primary_events + 1);
    }

    INSTANTIATE_TEST_SUITE_P(
        RootForms, WorldLifecycleTest,
        testing::Values(RootForm::Standalone, RootForm::Embedded, RootForm::External),
        [](const testing::TestParamInfo<RootForm>& info) -> std::string {
            switch (info.param) {
            case RootForm::Standalone: return "Standalone";
            case RootForm::Embedded: return "Embedded";
            case RootForm::External: return "External";
            }
            return "Unknown";
        });
}
