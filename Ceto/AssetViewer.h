#pragma once

#include <Rhea/Core/WorldService.h>
#include <Rhea/Core/Scene.h>
#include <cstdint>
#include <Rhea/Application.h>
#include <Rhea/Error.h>

class AssetViewer
{
public:
	void createAssetViewerScene(rhea::Application* app, const rhea::WorldService::OpenResourceContext& context, rhea::WorldService::AdaptResourceCompletion completion);
	rhea::ErrorReport prepareAnimations(rhea::Application* app,	rhea::Scene* scene);

protected:
	uint64_t m_asset_viewer_index = 0;
};