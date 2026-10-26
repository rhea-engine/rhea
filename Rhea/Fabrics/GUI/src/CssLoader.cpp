/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/GUI/CssLoader.h>

#include <SimpleParser/CssParser.h>
#include <Rhea/GUI/Styles/StyleSheet.h>
#include <Rhea/Resources/ResourceService.h>

namespace rhea {

	void CssLoader::load(std::string name, Application* app, orion::ResourceRequest* req, ResourceContext& context, Resource::LoadedHandler handler)
	{
		psio::Stream* rs = req->getStream();
		psio::IOBuffer* buf = new psio::IOBuffer(64 * 1024);
		std::string res_id = "//" + req->getRealmName() + req->getPath();
		rs->onEnd([app, name, buf, context, handler, res_id](size_t buflen) {
			StyleSheet* css = simpleparser::Type::create<StyleSheet>(name);
			std::unordered_map<std::string, std::string> attributes;
			std::unordered_map<std::string, Variant> values;
			css->setup(app, attributes, values);

			if (!buflen) {
				delete buf;
				if (handler)
					handler(Error(), css);
				return;
			}

			std::unique_ptr<simpleparser::CssParser> parser = std::make_unique<simpleparser::CssParser>();
			std::vector<simpleparser::CssParser::Entry> entries;

			int nof_entries = parser->parse(buf->reader(), buflen, entries);
			if (!nof_entries)
			{
				// TODO check if parse error or empty file
				delete buf;
				if (handler)
					handler(Error(), css);
				return;
			}

			// how to find the target?

			css->setStyle(entries);

			// resource to load?
			ResourceList res_list(app, context);
			std::vector<Unit::ResourceReference> references = css->getReferences();
			for (auto ref_it = references.begin(); ref_it != references.end(); ++ref_it) {
				res_list.push(ref_it->name, ref_it->value.toString(), ref_it->type);
			}
			delete buf;
			static_cast<ResourceService*>(app->getService("resources"))->loadAsync(res_list, [css, handler](ResourceList* rh, ErrorReport err) {
				if (err.isError()) {
					delete css;
					if (handler)
						handler(err, nullptr);
				}
				else {
					if (handler)
						handler(err, css);
				}
				});
			});
		rs->onError([buf, handler, res_id](int err) {
			if (err) {
				delete buf;
				Error error(Error::Code::RESOURCE_ERROR, "Could not read CSS resource", res_id);
				error.addContext("streamerror", std::to_string(err));
				if (handler)
					handler(error, nullptr);
			}
			});
		rs->ReadAsync(buf);
	}
}
