#pragma once

#include <Rhea/GUI/Dialog.h>
#include <Rhea/GUI/Button.h>
#include <Rhea/GUI/Label.h>

#include <functional>
#include <string>
#include <vector>

class AlertDialog : public rhea::Dialog
{
    RECLASS(AlertDialog, rhea::Dialog, "//ceto/gui/alert")

public:
    struct Action
    {
        std::string id;
        std::string text;
        bool closes_dialog = true;
    };

    struct Content
    {
        std::string title;
        std::string message;
        std::vector<Action> actions;

        std::string default_action;
        std::string cancel_action;
    };

    using ActionHandler = std::function<void(std::string)>;

    AlertDialog(std::string name, std::string type_name)
        : rhea::Dialog(name, type_name)
    {}

    void ready() override;

    // Configure while closed. Returns false for invalid configuration.
    bool configure(Content content, ActionHandler handler);

    void submit() override;
    void cancel() override;

private:
    void updateWidgets();
    void invokeAction(const std::string& action_id);
    bool onAction(rhea::Event* event, rhea::Unit* unit);

    Content m_content;
    ActionHandler m_action_handler;

    rhea::Label* m_title = nullptr;
    rhea::Label* m_message = nullptr;
    std::vector<rhea::Button*> m_buttons;

    bool m_action_completed = false;
};