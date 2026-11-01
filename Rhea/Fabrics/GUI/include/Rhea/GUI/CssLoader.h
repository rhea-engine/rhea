/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_CSSLOADER_H
#define RHEA_CSSLOADER_H

#include <Rhea/Application.h>
#include <Rhea/Resources/Resource.h>
#include <Rhea/Resources/ResourceLoader.h>

namespace rhea
{
	class CssLoader
	{
	public:
		static void load(const std::string& name, const std::string& res_id, Application* app, orion::ResourceRequest* req, ResourceContext context, Resource::LoadedHandler handler);
	};
}
#endif // !RHEA_CSSLOADER_H
