#define NOMINMAX
#include "ViewerSession.h"
#include <cmath>
#include <Rhea/Application.h>
#include <Rhea/Controller/SpectatorController.h>
#include <Rhea/Components/MeshComponent.h>
#include <Rhea/Core/Mesh.h>
#include <Rhea/Graphics/IGraphicsService.h>
#include <Poseidon/Core.h>
#include <Poseidon/FrameRenderer.h>
#include <Poseidon/Viewport.h>

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
	m_preview_controller = nullptr;

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

		// controller?
		const auto controller_path = scene->getValueString("ceto_preview_controller", ""); // TODO: accept an array of controllers and allow selection
        m_preview_controller = controller_path.empty()
            ? nullptr
            : doCast<rhea::Controller>(scene->find(controller_path));

    }
    else {
        m_mode = ViewerSession::Mode::Scene;
        m_content_root = scene;
    }

	// animation targets and scene bounds
	m_scene_bounds = math::AABB();
	m_animation_targets.clear();
	m_content_root->traverse([this](std::string& name, rhea::Component* component) -> bool {
		if (auto* anim_target = doCast<rhea::AnimationComponent>(component)) {
			m_animation_targets.push_back(anim_target);
		}
		else if (auto* item = doCast<rhea::Item>(component)) {
			if (auto* mesh_component = item->getComponent<rhea::MeshComponent>()) {
				if (auto* mesh = mesh_component->getMesh()) {
					const math::Matrix4 item_world_matrix(item->getTransform()->getWorldMatrix());
					m_scene_bounds.expand(mesh->getLocalBounds().transformed(item_world_matrix));
				}
			}
		}
		return true;
		});

    if (m_mode == Mode::AssetPreview) {
        fitContent();
		float step_size = m_scene_bounds.size().length() * 0.5f;
        if (step_size < 3.0f)
            if (auto* basic_controller = doCast<rhea::BasicController>(m_preview_controller)) {
                basic_controller->setMoveStep(step_size);
                basic_controller->setTurnStep(step_size / 2.0f);
                basic_controller->setSideStep(step_size);
                basic_controller->setUpStep(step_size);
            }
    }

	if (m_changed_handlers.size() > 0) {
		for (auto& handler : m_changed_handlers) {
			handler(m_mode);
		}
	}
}

bool ViewerSession::fitContent()
{
    if (!m_preview_controller || !m_selected_camera || m_scene_bounds.isEmpty())
        return false;

	auto* app = m_selected_camera->getApplication();
	auto* graphics = app ? app->getService<rhea::IGraphicsService>() : nullptr;
    int width = 0;
	int height = 0;
    if (!graphics || !graphics->getViewSize("main", "default", "default", width, height) || width <= 0 || height <= 0)
		return false;

	const float aspect_ratio = static_cast<float>(width) / height;

    const auto center = m_scene_bounds.center();
    const float radius = m_scene_bounds.size().length() * 0.5f;

    const float half_fov_y = m_selected_camera->getVerticalFovDegrees() * math::degToRad * 0.5f;
    const float half_fov_x = std::atan(std::tan(half_fov_y) * aspect_ratio);
    const float half_fov = std::min(half_fov_x, half_fov_y);

    // Frame a sphere around the bounds with a little extra room.
	const float minimum_fit_distance = 0.02f; // Avoids camera being too close to the content.
    const float distance = std::max(minimum_fit_distance, 1.1f * radius / std::sin(half_fov));

    const auto rotation = m_selected_camera->getTransform()->getWorldPosition().getRotation();
    const auto forward = rotation * math::Vector3::UNIT_Z;

    rhea::Position desired_pose;
    desired_pose.setOrientation(rotation);
    desired_pose.setPosition(center - forward * distance);

    if (!m_initial_camera_position_set) {
        m_initial_camera_position = desired_pose;
		m_initial_camera_position_set = true;
	}

    return m_preview_controller->setCameraWorldPose(desired_pose);
}

bool ViewerSession::resetCamera()
{
	if (!m_preview_controller || !m_initial_camera_position_set || m_scene_bounds.isEmpty())
		return false;
    m_preview_controller->setCameraWorldPose(m_initial_camera_position);
    return true; 
}
