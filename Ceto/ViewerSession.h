#pragma once

#include <functional>
#include <vector>
#include <Rhea/Core/Scene.h>
#include <Rhea/Core/World.h>
#include <Rhea/Core/Camera.h>
#include <Rhea/Animation/AnimationComponent.h>

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

private:
    Mode m_mode = Mode::Empty;

    rhea::World* m_world = nullptr;
    rhea::Scene* m_standalone_scene = nullptr;

    // Actual content, excluding preview grid, lights and camera.
    rhea::Unit* m_content_root = nullptr;
	rhea::Unit* m_preview_root = nullptr;

    rhea::Scene* m_current_scene = nullptr;

	std::vector<CameraInfo> m_cameras;
	rhea::Camera* m_selected_camera = nullptr;
	std::vector<rhea::AnimationComponent*> m_animation_targets;
	rhea::AnimationComponent* m_selected_animation_target = nullptr;
	std::vector<ChangedHandler> m_changed_handlers;
};