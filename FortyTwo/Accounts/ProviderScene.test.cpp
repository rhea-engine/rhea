#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Rhea/Fabrics/Core/Scene.h>
#include <Rhea/UnitTree.h>
#include <SimpleParser/Variant.h>

#include "ProviderScene.h"
#include "User.h"
#include "AccountService.h"

class TestApp : public Rhea::Application
{
public:
    void onScene(Rhea::Scene* scene)
    {}
    ~TestApp() {}
};

class ProviderSceneTest : public ::testing::Test {
protected:
    std::unique_ptr<TestApp> app;
    Rhea::Scene* scene = nullptr;

    void SetUp() override {
        // add an app
        app = std::make_unique<TestApp>();
        app->init();
        scene = new Rhea::Scene("testscene", "//meworla/scene");
        app->getUnitTree()->addChild(scene);
        static bool initialized = false;

        if (!initialized) {
            AccountService::RegisterAccountFabrics(app.get());
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

    std::vector<Rhea::Unit::Export> refs;
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

    std::vector<Rhea::Unit::Export> refs;
    refs = provider.getReferences();

    EXPECT_FALSE(refs[0].value == "//avatarlist");
}

// check no ref if avatarlist is not a string
TEST_F(ProviderSceneTest, AvatarListNoConfig)
{
    ProviderScene provider;
    std::unordered_map<std::string, Variant> values;

    provider.Setup(app.get(), values);

    std::vector<Rhea::Unit::Export> refs;
    refs = provider.getReferences();

    EXPECT_EQ(refs.size(), 0);
}

// test if getUser returns the the user if avatarlist is loaded
TEST_F(ProviderSceneTest, GetUser)
{
    ProviderScene provider;
    std::unordered_map<std::string, Variant> values;
    values["avatarlist"] = "//avatarlist";
    Rhea::Unit avatarlist("avatarlist", "//core/types/unit");
    app->getUnitTree()->addChild(&avatarlist);
    User* user_someone = new User("someone");
    avatarlist.addChild(user_someone);
    Rhea::Avatar some_avatar("johndoe", "//meworla/avatar");
    user_someone->addChild(&some_avatar);

    provider.Setup(app.get(), values);

    std::vector<Rhea::Unit::Export> refs;
    refs = provider.getReferences();

    EXPECT_TRUE(refs[0].value == "//avatarlist");

    User* user = provider.getUser("someone");
    EXPECT_EQ(user, user_someone);
    delete user_someone;
}

// test if getUser returns the null if the user is not defined
TEST_F(ProviderSceneTest, GetUserNoUser)
{
    ProviderScene provider;
    std::unordered_map<std::string, Variant> values;
    values["avatarlist"] = "//avatarlist";
    Rhea::Unit avatarlist("avatarlist", "//core/types/unit");
    app->getUnitTree()->addChild(&avatarlist);
    User* user_someone = new User("someone");
    avatarlist.addChild(user_someone);
    Rhea::Avatar some_avatar("johndoe", "//meworla/avatar");
    user_someone->addChild(&some_avatar);

    provider.Setup(app.get(), values);

    std::vector<Rhea::Unit::Export> refs;
    refs = provider.getReferences();

    EXPECT_TRUE(refs[0].value == "//avatarlist");

    User* user = provider.getUser("nouser");
    EXPECT_EQ(user, nullptr);
    delete user_someone;
}
