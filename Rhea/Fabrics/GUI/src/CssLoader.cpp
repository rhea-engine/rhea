#include <Rhea/GUI/CssLoader.h>

#include <SimpleParser/CssParser.h>
#include <Rhea/GUI/Styles/StyleSheet.h>
#include <Rhea/Resources/ResourceService.h>

namespace rhea {
	
	void CssLoader::load(std::string name, Application* app, Orion::ResourceRequest* req, Resource::LoadedHandler handler)
	{
		Psio::Stream* rs = req->getStream();
		Psio::IOBuffer* buf = new Psio::IOBuffer(64 * 1024);
		rs->onEnd([&, buf, handler](size_t buflen) {
			if (buflen) {
				std::unique_ptr<SimpleParser::CssParser> parser = std::make_unique<SimpleParser::CssParser>();
				std::vector<SimpleParser::CssParser::Entry> entries;

				int nof_entries = parser->parse(buf->reader(), buflen, entries);

				// how to find the target?
				StyleSheet* css = SimpleParser::Type::create<StyleSheet>(name);
				std::unordered_map<std::string, std::string> attributes;
				std::unordered_map<std::string, Variant> values;
				css->setup(app, attributes, values);
				css->setStyle(entries);

				// resource to load?
				ResourceList res_list(app);
				std::vector<Unit::Export> references = css->getReferences();
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
