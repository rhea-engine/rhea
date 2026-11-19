#pragma once

#include <functional>
#include <vector>
#include <Rhea/Core/Scene.h>
#include <Rhea/Core/World.h>
#include <Rhea/Core/Camera.h>
#include <Rhea/Core/Controller.h>
#include <Rhea/Animation/AnimationComponent.h>
#include <Math/AABB.h>
#include <Rhea/Core/Position.h>

class ViewerSession
{
public:
    enum class Mode
    {
        Empty,
        Scene,
        AssetPreview
    };

	struct CameraInfo
	{
		rhea::Camera* camera;
		std::string name;
		bool is_preview_camera = false;
	};

	ViewerSession() = default;

    void setWorld(rhea::World* world, rhea::Scene* standalone_scene);
	void setMode(Mode mode) { m_mode = mode; }
	Mode getMode() const { return m_mode; }
    void clear();

    // Available cameras and animated instances.
    const std::vector<CameraInfo>& getCameras() const;
    const std::vector<rhea::AnimationComponent*>& getAnimationTargets() const;

    rhea::Camera* getSelectedCamera() const;
    rhea::AnimationComponent* getSelectedAnimationTarget() const;

    bool selectCamera(rhea::Camera* camera);
    bool selectAnimationTarget(rhea::AnimationComponent* target);

    using ChangedHandler = std::function<void(Mode)>;
	void addChangedHandler(ChangedHandler handler) { m_changed_handlers.push_back(handler); }

	void activateScene(rhea::Scene* scene);

    bool hasActiveContent() const { return m_mode != Mode::Empty; }

    bool fitContent();
	bool resetCamera();

private:
    Mode m_mode = Mode::Empty;

    rhea::World* m_world = nullptr;
    math::AABB m_scene_bounds;
    rhea::Scene* m_standalone_scene = nullptr;

    // Actual content, excluding preview grid, lights and camera.
    rhea::Unit* m_content_root = nullptr;
	rhea::Unit* m_preview_root = nullptr;
	rhea::Controller* m_preview_controller = nullptr;

    rhea::Scene* m_current_scene = nullptr;

	std::vector<CameraInfo> m_cameras;
	rhea::Camera* m_selected_camera = nullptr;
	std::vector<rhea::AnimationComponent*> m_animation_targets;
	rhea::AnimationComponent* m_selected_animation_target = nullptr;
	std::vector<ChangedHandler> m_changed_handlers;

	rhea::Position m_initial_camera_position;
	bool m_initial_camera_position_set = false;
};
