#include "ViewerSession.h"

void ViewerSession::setWorld(rhea::World* world, rhea::Scene* standalone_scene)
{
	m_world = world;
	m_standalone_scene = standalone_scene;
}

const std::vector<ViewerSession::CameraInfo>& ViewerSession::getCameras() const
{
    // TODO: hier return-Anweisung eingeben
	return m_cameras;
}

const std::vector<rhea::AnimationComponent*>& ViewerSession::getAnimationTargets() const
{
    // TODO: hier return-Anweisung eingeben
	return  m_animation_targets;
}

rhea::Camera* ViewerSession::getSelectedCamera() const
{
    return m_selected_camera;
}

rhea::AnimationComponent* ViewerSession::getSelectedAnimationTarget() const
{
    return m_selected_animation_target;
}

bool ViewerSession::selectCamera(rhea::Camera* camera)
{
    return false;
}

bool ViewerSession::selectAnimationTarget(rhea::AnimationComponent* target)
{
    return false;
}

void ViewerSession::activateScene(rhea::Scene* scene)
{
    m_current_scene = scene;
	m_cameras.clear();

    if (scene->getValueString("ceto_view_mode", "") == "asset") {
        m_mode = ViewerSession::Mode::AssetPreview;

        const auto root_path = scene->getValueString("ceto_content_root", "");

        m_content_root = doCast<rhea::Unit>(scene->find(root_path));

        // camera?
		const auto camera_path = scene->getValueString("ceto_preview_camera", ""); // TODO: accept an array of cameras and allow selection
        if (!camera_path.empty()) {
            CameraInfo cam_info;

            cam_info.camera = doCast<rhea::Camera>(scene->find(camera_path));
            if (cam_info.camera) {
                cam_info.name = cam_info.camera->getPathName();
                cam_info.is_preview_camera = true;
                m_selected_camera = cam_info.camera;
                m_cameras.push_back(cam_info);
            }
        }
    }
    else {
        m_mode = ViewerSession::Mode::Scene;
        m_content_root = scene;
    }

	// animation targets
	m_animation_targets.clear();
	m_content_root->traverse([this](std::string& name, rhea::Component* component) -> bool {
		if (auto* anim_target = doCast<rhea::AnimationComponent>(component)) {
			m_animation_targets.push_back(anim_target);
		}
		return true;
		});

	if (m_changed_handlers.size() > 0) {
		for (auto& handler : m_changed_handlers) {
			handler(m_mode);
		}
	}
}
