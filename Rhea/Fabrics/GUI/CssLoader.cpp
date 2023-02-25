#include "CssLoader.h"

#include <SimpleParser/CssParser.h>
#include "StyleSheet.h"

namespace Rhea {
	
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
				StyleSheet* css = static_cast<StyleSheet*>(SimpleParser::Type::create(name, "//meworla/gui/stylesheet"));
				std::unordered_map<std::string, std::string> attributes;
				std::unordered_map<std::string, Variant> values;
				css->setup(app, attributes, values);
				css->setStyle(entries);

				handler(ReqError::OK, css);
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
