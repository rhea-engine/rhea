#include "gtest/gtest.h"
#include "World.h"
#include <Rhea/Realm.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Core/Avatar.h>

class TestApp : public rhea::Application
{
public:
    void onScene(rhea::Scene* scene)
    {}
};


class UserInfoTest : public ::testing::Test {
protected:
    World *world;
    TestApp* app;
    rhea::Realm *realm;

    void SetUp() override {
        // add an app
		app = new TestApp();
        app->init();

        // add local Realm
        realm = new rhea::Realm ("local", "//meworla/realm");
		app->getUnitTree()->addChild(realm);

        // add world server item
        SimpleParser::Type::registerType<World>("//meworla/world");
        // user and avatar types
        SimpleParser::Type::registerTypeName<rhea::Unit>("//meworla/user");
        SimpleParser::Type::registerType<rhea::Avatar>("//meworla/avatar");

        // World
        world = new World("testworld", "//meworla/world");
    }

    void TearDown() override {
        delete world;
        delete realm;
        delete app;
    }
};
