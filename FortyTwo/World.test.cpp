#include "gtest/gtest.h"
#include "World.h"
#include <Rhea/Realm.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Fabrics/Core/Avatar.h>

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

TEST_F(UserInfoTest, GetUserTestNoData) {
    // get user before all is setup
    Rhea::Unit* user_info = world->getUserInfo("testuser");
    EXPECT_EQ(user_info, nullptr);
}

TEST_F(UserInfoTest, GetUserTest) {

    // data
	// add an avatarlist to the unit tree
    std::unordered_map<std::string, std::string> attributes;
    std::unordered_map<std::string, Variant> values;
    values["avatarlist"] = "//local/avatarlist";
	world->setup(app, attributes, values);   

	// add avatarlist to unit tree
	Rhea::Unit alist("avatarlist", "//core/types/unit");
    realm->addChild(&alist);
	Rhea::Unit user("testuser", "//meworla/user");
	alist.addChild(&user);  

	// User
    Rhea::Unit* user_info = world->getUserInfo("testuser");
	ASSERT_NE(user_info, nullptr);
	EXPECT_EQ(user_info->getTypeName(), "//core/types/unit"); 
}

TEST_F(UserInfoTest, GetUserTestNoSuchUser) {

    // data
    // add an avatarlist to the unit tree
    std::unordered_map<std::string, std::string> attributes;
    std::unordered_map<std::string, Variant> values;
    values["avatarlist"] = "//local/avatarlist";
    world->setup(app, attributes, values);

    // add avatarlist to unit tree
    Rhea::Unit alist("avatarlist", "//core/types/unit");
    realm->addChild(&alist);
    Rhea::Unit user("testuser", "//meworla/user");
    alist.addChild(&user);

    // User
    Rhea::Unit* user_info = world->getUserInfo("nouser");
    EXPECT_EQ(user_info, nullptr);
}