#pragma once

#include <Rhea/Core/WorldService.h>

class AssetViewer
{
public:
	void createAssetViewerScene(rhea::Application* app, const rhea::WorldService::OpenResourceContext& context, rhea::WorldService::AdaptResourceCompletion completion);

protected:
	uint64_t m_asset_viewer_index = 0;
};