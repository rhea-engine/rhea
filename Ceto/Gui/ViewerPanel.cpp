#include "ViewerPanel.h"
#include "../ViewerSession.h"

#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Animation/AnimationState.h>
#include <Rhea/GUI/Button.h>

void ViewerPanel::ready()
{
	// search the playlist parent
	Widget::ready();

	m_playlist = doCast<rhea::Widget>(find("content/animations/playlist/entries"));
}

void ViewerPanel::onChange()
{
	if (!m_session || !m_playlist)
		return;

    m_playlist->clearWidgetChildren();

    if (!m_session || m_session->getMode() != ViewerSession::Mode::AssetPreview) {
        hide();
        m_playlist->invalidateLayout();
        return;
    }

	if (m_session->getAnimationTargets().empty()) {
		hide();
		m_playlist->invalidateLayout();
		return;
	}
    show();

    auto* tree = m_app->getUnitTree();
    size_t target_index = 0;

    for (auto* target : m_session->getAnimationTargets()) {
        if (!target)
            continue;

        auto* animator = target->getAnimator();
        if (!animator)
            continue;

        // Collect the playable states of this target.
        std::vector<std::string> state_names;
        animator->children([&state_names](std::string& name, rhea::Component* component) {
                if (doCast<rhea::AnimationState>(component))
                    state_names.push_back(name);
                return true;
            },
            true);

        if (state_names.empty())
            continue;

        auto* group = tree->createUnit(
            "target_" + std::to_string(target_index++),
            "//meworla/gui/column",
            {},
            { { "fillwidth", true }, { "cssclass", "viewer-animation-group" } });

        m_playlist->addChild(group);

        const std::string target_name = target->getParent()
            ? target->getParent()->getPathName()
            : target->getPathName();

        auto* title = tree->createUnit(
            "title",
            "//meworla/gui/label",
            {},
            { { "text", target_name }, { "fillwidth", true }, { "cssclass", "viewer-animation-target" } });

        group->addChild(title);

        size_t clip_index = 0;
        for (const auto& state_name : state_names) {
			std::string display_name = state_name;
            if (state_name.empty())
				display_name = "Animation " + std::to_string(clip_index+1);
            auto* row = tree->createUnit(
                "clip_" + std::to_string(clip_index++),
                "//meworla/gui/row",
                {},
                { { "fillwidth", true }, { "cssclass", "viewer-animation-row" } });

            group->addChild(row);

            auto* label = tree->createUnit(
                "name",
                "//meworla/gui/label",
                {},
                { { "text", display_name }, { "cssclass", "viewer-animation-name" } });

            row->addChild(label);

            auto* play_button = doCast<rhea::Button>(tree->createUnit(
                "play",
                "//meworla/gui/button",
                {},
                { { "text", "Play" }, { "cssclass", "viewer-animation-play" } }));

            row->addChild(play_button);

			if (clip_index == 1 && m_current_animation_targets.find(target) == m_current_animation_targets.end()) {
				// if this is the first clip and no animation is currently playing, set it to play
				m_current_animation_targets[target] = { state_name, true, play_button };
				play_button->setText("Pause");
			}


            play_button->registerEventHandler(
                "clicked", this,
                [this, target, state_name, play_button](rhea::Event* ev, rhea::Unit*) {
                    ev->stopPropagation();
                    if (m_current_animation_targets.find(target) != m_current_animation_targets.end()) {
						if (m_current_animation_targets[target].state_name != state_name) {
							// stop the previous animation
							target->pause();
							m_current_animation_targets[target].play_button->setText("Play");
							m_current_animation_targets[target] = { state_name, true, play_button };
							target->play(state_name);
                            play_button->setText("Pause");
						}
                        else {
                            if (m_current_animation_targets[target].is_playing) {
                                target->pause();
                                m_current_animation_targets[target].is_playing = false;
                                play_button->setText("Play");
                            }
                            else {
                                m_current_animation_targets[target].is_playing = true;
                                target->play(state_name);
                                play_button->setText("Pause");
                            }
                        }
					}
					else {
                        m_current_animation_targets[target] = {state_name, true, play_button};
                        target->play(state_name);
						play_button->setText("Pause");
                    }
                    return false;
                });
        }

        // Finalize the complete group, including its children.
        group->ready();
    }

    m_playlist->invalidateLayout();
}
