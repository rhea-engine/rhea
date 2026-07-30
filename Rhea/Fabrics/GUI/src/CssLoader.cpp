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
	
	void CssLoader::load(std::string name, Application* app, orion::ResourceRequest* req, Resource::LoadedHandler handler)
	{
		psio::Stream* rs = req->getStream();
		psio::IOBuffer* buf = new psio::IOBuffer(64 * 1024);
		rs->onEnd([&, buf, handler](size_t buflen) {
			if (buflen) {
				std::unique_ptr<simpleparser::CssParser> parser = std::make_unique<simpleparser::CssParser>();
				std::vector<simpleparser::CssParser::Entry> entries;

				int nof_entries = parser->parse(buf->reader(), buflen, entries);

				// how to find the target?
				StyleSheet* css = simpleparser::Type::create<StyleSheet>(name);
				std::unordered_map<std::string, std::string> attributes;
				std::unordered_map<std::string, Variant> values;
				css->setup(app, attributes, values);
				css->setStyle(entries);

				// resource to load?
				ResourceList res_list(app);
				std::vector<Unit::ResourceReference> references = css->getReferences();
				for (auto ref_it = references.begin(); ref_it != references.end(); ++ref_it) {					
					res_list.push(ref_it->name, ref_it->value.toString(), ref_it->type);
				}
				static_cast<ResourceService*>(app->getService("resources"))->loadAsync(res_list, [css, handler](ResourceList* rh, Error* err) {
					handler(ReqError::OK, css);
					});			
			}			
			});
		rs->onError([buf, handler](int err) {
			if (err) {
				delete buf;
				handler(ReqError::DATA_INVALID, nullptr);
			}
			});
		rs->ReadAsync(buf);
	}
}
