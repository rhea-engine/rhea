#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Rhea/Core/Scene.h>
#include <Rhea/UnitTree.h>
#include <SimpleParser/Variant.h>

#include "ProviderScene.h"
#include "User.h"
#include "AccountService.h"

class TestApp : public rhea::Application
{
public:
    void onScene(rhea::Scene* scene)
    {}
    ~TestApp() {}
};

class ProviderSceneTest : public ::testing::Test {
protected:
    std::unique_ptr<TestApp> app;
    rhea::Scene* scene = nullptr;

    void SetUp() override {
        // add an app
        app = std::make_unique<TestApp>();
        app->init();
        scene = new rhea::Scene("testscene", "//meworla/scene");
        app->getUnitTree()->addChild(scene);
        static bool initialized = false;

        if (!initialized) {
            AccountService::RegisterFabrics(app.get());
            initialized = true;
        }
    }

    void TearDown() override {
        delete scene;
    }
};

// test if getReference returns the avatarlist if it is given in setup
TEST_F(ProviderSceneTest, AvatarListConfig)
{
    ProviderScene provider;
    std::unordered_map<std::string, Variant> values;
    values["avatarlist"] = "//avatarlist";

    provider.Setup(app.get(), values);

    std::vector<rhea::Unit::Export> refs;
    refs = provider.getReferences();

    EXPECT_TRUE(refs[0].value == "//avatarlist");
}

// check no ref if avatarlist is not a string
TEST_F(ProviderSceneTest, AvatarListWrongConfig)
{
    ProviderScene provider;
    std::unordered_map<std::string, Variant> values;
    values["avatarlist"] = false;

    provider.Setup(app.get(), values);

    std::vector<rhea::Unit::Export> refs;
    refs = provider.getReferences();

    EXPECT_FALSE(refs[0].value == "//avatarlist");
}

// check no ref if avatarlist is not a string
TEST_F(ProviderSceneTest, AvatarListNoConfig)
{
    ProviderScene provider;
    std::unordered_map<std::string, Variant> values;

    provider.Setup(app.get(), values);

    std::vector<rhea::Unit::Export> refs;
    refs = provider.getReferences();

    EXPECT_EQ(refs.size(), 0);
}

// test if getUser returns the the user if avatarlist is loaded
TEST_F(ProviderSceneTest, GetUser)
{
    ProviderScene provider;
    std::unordered_map<std::string, Variant> values;
    values["avatarlist"] = "//avatarlist";
    rhea::Unit avatarlist("avatarlist", "//core/types/unit");
    app->getUnitTree()->addChild(&avatarlist);
    User* user_someone = new User("someone");
    avatarlist.addChild(user_someone);
    rhea::Avatar some_avatar("johndoe", "//meworla/avatar");
    user_someone->addChild(&some_avatar);

    provider.Setup(app.get(), values);

    std::vector<rhea::Unit::Export> refs;
    refs = provider.getReferences();

    EXPECT_TRUE(refs[0].value == "//avatarlist");

    AuthInfo auth_info;
	auth_info.user_id = "someone";

    User* user = provider.getUser(auth_info);
    EXPECT_EQ(user, user_someone);
    delete user_someone;
}

// test if getUser returns the null if the user is not defined
TEST_F(ProviderSceneTest, GetUserNoUser)
{
    ProviderScene provider;
    std::unordered_map<std::string, Variant> values;
    values["avatarlist"] = "//avatarlist";
    rhea::Unit avatarlist("avatarlist", "//core/types/unit");
    app->getUnitTree()->addChild(&avatarlist);
    User* user_someone = new User("someone");
    avatarlist.addChild(user_someone);
    rhea::Avatar some_avatar("johndoe", "//meworla/avatar");
    user_someone->addChild(&some_avatar);

    provider.Setup(app.get(), values);

    std::vector<rhea::Unit::Export> refs;
    refs = provider.getReferences();

    EXPECT_TRUE(refs[0].value == "//avatarlist");

	AuthInfo auth_info;
	auth_info.user_id = "nouser";

    User* user = provider.getUser(auth_info);
    EXPECT_EQ(user, nullptr);
    delete user_someone;
}
