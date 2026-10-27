#include "AlertDialog.h"

#include <Rhea/GUI/GUIController.h>
#include <Psio/Log.h>

#include <algorithm>
#include <unordered_set>
#include <utility>

void AlertDialog::ready()
{
    rhea::Dialog::ready();
    setModal(true);

    m_title = doCast<rhea::Label>(find("content/title"));
    m_message = doCast<rhea::Label>(find("content/message"));
    m_buttons.clear();
    for (int index = 0; index < 3; ++index) {
        auto* button = doCast<rhea::Button>(find("content/actions/action" + std::to_string(index)));
        m_buttons.push_back(button);
    }

    registerEventHandler("alert_action", this,
        std::bind(&AlertDialog::onAction, this, std::placeholders::_1, std::placeholders::_2));

    if (!m_title || !m_message || std::find(m_buttons.begin(), m_buttons.end(), nullptr) != m_buttons.end())
        LOG(LOG_ERROR) << "AlertDialog: required labels or action buttons are missing";

    updateWidgets();
}

bool AlertDialog::configure(Content content, ActionHandler handler)
{
    if (isVisible() || !m_title || !m_message || content.actions.empty()
        || content.actions.size() > m_buttons.size()
        || std::find(m_buttons.begin(), m_buttons.end(), nullptr) != m_buttons.end())
        return false;

    std::unordered_set<std::string> action_ids;
    for (const auto& action : content.actions) {
        if (action.id.empty() || action.text.empty() || !action_ids.insert(action.id).second)
            return false;
    }

    if ((!content.default_action.empty() && !action_ids.count(content.default_action))
        || (!content.cancel_action.empty() && !action_ids.count(content.cancel_action)))
        return false;

    m_content = std::move(content);
    m_action_handler = std::move(handler);
    m_action_completed = false;
    updateWidgets();
    return true;
}

void AlertDialog::updateWidgets()
{
    if (!m_title || !m_message)
        return;

    m_title->setText(m_content.title);
    m_message->setText(m_content.message);
    setDefaultButton(nullptr);
    setInitialFocusWidget(nullptr);

    rhea::Button* first_button = nullptr;
    for (std::size_t index = 0; index < m_buttons.size(); ++index) {
        auto* button = m_buttons[index];
        if (!button)
            continue;

        const bool used = index < m_content.actions.size();
        button->setEnabled(used);
        if (!used) {
            button->makeInvisible();
            continue;
        }

        const auto& action = m_content.actions[index];
        button->setText(action.text);
        button->makeVisible();
        if (!first_button)
            first_button = button;
        if (action.id == m_content.default_action) {
            setDefaultButton(button);
            setInitialFocusWidget(button);
        }
    }

    if (!getDefaultButton() && first_button)
        setInitialFocusWidget(first_button);
    invalidateLayout();
}

void AlertDialog::submit()
{
    invokeAction(m_content.default_action);
}

void AlertDialog::cancel()
{
    invokeAction(m_content.cancel_action);
}

void AlertDialog::invokeAction(const std::string& action_id)
{
    if (m_action_completed || action_id.empty() || !isVisible())
        return;

    auto action_it = std::find_if(m_content.actions.begin(), m_content.actions.end(),
        [&action_id](const Action& action) { return action.id == action_id; });
    if (action_it == m_content.actions.end())
        return;

    // The callback may reconfigure or destroy this dialog.
    const std::string selected_id = action_it->id;
    auto handler = m_action_handler;
    if (action_it->closes_dialog) {
        rhea::GUIController* controller = nullptr;
        for (auto* parent = getParent(); parent; parent = parent->getParent()) {
            controller = doCast<rhea::GUIController>(parent);
            if (controller)
                break;
        }
        if (!controller) {
            LOG(LOG_ERROR) << "AlertDialog: no owning GUIController found";
            return;
        }

        m_action_completed = true;
        controller->closeDialog(this);
    }

    if (handler)
        handler(selected_id);
}

bool AlertDialog::onAction(rhea::Event* event, rhea::Unit* unit)
{
    if (!event)
        return false;

    auto* target = event->getTarget();
    for (std::size_t index = 0; index < m_buttons.size() && index < m_content.actions.size(); ++index) {
        if (target == m_buttons[index]) {
            event->stopPropagation();
            invokeAction(m_content.actions[index].id);
            return true;
        }
    }
    return false;
}
