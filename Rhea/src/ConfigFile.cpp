/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/ConfigFile.h>
#include <Rhea/Application.h>
#include <Orion/FileStream.h>
#include <SimpleParser/IniFileParser.h>
#include <Psio/Log.h>

namespace rhea
{
	bool ConfigFile::loadFromFile(std::string filename)
	{
		auto fs = m_app->getFS();
		if (fs->getFs()->fileExists(filename))
		{
			// file exists		
			std::unique_ptr<orion::FileResourceStream> stream = std::make_unique<orion::FileResourceStream>(filename);
			std::unique_ptr<psio::IOBuffer> buf = std::make_unique<psio::IOBuffer>(64 * 1024);
			if (stream->Read(buf.get())) {
				const char* ptr = buf->reader();
				size_t len = buf->Remaining();
				simpleparser::IniFileParser* parser = new simpleparser::IniFileParser();
				std::vector<simpleparser::IniFileParser::Section> sections;
				int err = parser->parse((char*)ptr, len, sections);
				if (err == (int)simpleparser::IniFileParser::Error::PARSE_OK) {
					for (auto& section : sections) {
						std::string key = section.section_name;
						// attributes have no meaning for the config file yet
						// for (auto& attr : section.attributes) {
						//    m_attributes[attr.first] = attr.second;
						// }
						for (auto& value : section.values) {
							key = section.section_name + "." + value.first;
							// only overwrite if not already set by command line args
							if (m_values.find(key) == m_values.end())
								m_values[key] = value.second;
						}
					}
					return true;
				}
				else {
					LOG(LOG_ERROR) << "Failed to parse config file " << filename << ", " << parser->getErrMsg((simpleparser::IniFileParser::Error)err);
				}
			}
		}
		return false;
	}

	bool ConfigFile::fromCommandLineArgs(int argc, char* argv[])
	{
		for (int i = 1; i < argc; ++i)
		{
			std::string arg = argv[i];

			// Generic version
			if (arg == "--set")
			{
				if (i + 1 < argc)
				{
					std::string kv = argv[++i]; // consume next argument

					auto pos = kv.find('=');
					if (pos != std::string::npos)
					{
						std::string key = kv.substr(0, pos);
						std::string value = kv.substr(pos + 1);
						m_values[key] = value;
					}
				}
			}
			else if (arg ==  "--help" || arg == "-h")
			{
				LOG(LOG_INFO) << "Usage: " << argv[0] << " [options]\n"
					<< "Options:\n"
					<< "  -d directory       Set application directory (maps to //local)\n"
					<< "  --set key=value    Set a configuration value\n"
					<< "  --key=value        Set a configuration value\n"
					<< "  --key value        Set a configuration value\n"
					<< "  --help, -h         Show this help message";
				return false; // indicate that we should exit after showing help
			}
			else if (arg == "-d") {
				if (i + 1 < argc)
				{
					std::string dir = argv[++i]; // consume next argument
					m_values["appdir"] = dir; // set //local to the directory
				}
			}
			else if (arg.rfind("--", 0) == 0) // starts with "--"
			{
				std::string key = arg.substr(2); // remove "--"
				std::string value;
				// check if key contains '='
				if (key.find('=') != std::string::npos)
				{
					auto pos = key.find('=');
					value = key.substr(pos + 1);
					key = key.substr(0, pos);
					m_values[key] = value;
				}
				// check if next argument is a value (doesn't start with "--")
				else if (i + 1 < argc && std::string(argv[i + 1]).rfind("--", 0) != 0)
				{
					value = argv[++i]; // consume next argument
					m_values[key] = value;
				}
				else {
					m_values[key] = "true"; // default value for flags
				}
			}
			// last element is scene name
			else if (i == argc - 1)
			{
				m_values["scene"] = arg;
			}
			else {
				LOG(LOG_WARNING) << "Unrecognized command line argument: " << arg;
			}
		}
		return true;
	}
}