/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef ORION_COLLECTION_H
#define ORION_COLLECTION_H

#include <string>

#include <Orion/ResourceStream.h>

namespace orion
{
	class Collection : public ResourceStream
	{
	
	public:		
		struct Entry
		{
			std::string name;
			std::string mime;
			size_t length = 0;
			bool is_dir = false;

			// helper function to check if the entry has a specific extensions
			inline bool hasExtension(const std::string& ext) const {
				return name.size() >= ext.size() &&
					name.compare(name.size() - ext.size(), ext.size(), ext) == 0;
			}
		};

		// Set filter callback
		void setFilter(std::function<bool(const Entry&)> filter) {
			m_filter = filter;
		}

		bool IsDir() { return true; }

		virtual Entry* nextEntry() = 0;

	protected:
		std::function<bool(const Entry&)> m_filter = nullptr;

	};
}
#endif // ! ORION_COLLECTION_H
