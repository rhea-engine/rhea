#include "gtest/gtest.h"
#include "World.h"
#include <Rhea/Realm.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Core/Avatar.h>

class TestApp : public Rhea::Application
{
public:
    void onScene(Rhea::Scene* scene)
    {}
};


class UserInfoTest : public ::testing::Test {
protected:
    World *world;
    TestApp* app;
    Rhea::Realm *realm;

    void SetUp() override {
        // add an app
		app = new TestApp();
        app->init();

        // add local Realm
        realm = new Rhea::Realm ("local", "//meworla/realm");
		app->getUnitTree()->addChild(realm);

        // add world server item
        SimpleParser::Type::registerType<World>("//meworla/world");
        // user and avatar types
        SimpleParser::Type::registerTypeName<Rhea::Unit>("//meworla/user");
        SimpleParser::Type::registerType<Rhea::Avatar>("//meworla/avatar");

        // World
        world = new World("testworld", "//meworla/world");
    }

    void TearDown() override {
        delete world;
        delete realm;
        delete app;
    }
};
