/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_ERROR_H
#define RHEA_ERROR_H

#include <string>

namespace rhea
{
	class Error
	{
	public:
		enum struct Type {
			NONE,
			INVALID_DEFINITION,
			INVALID_DATA,
			RESOURCE_ERROR
		};
		Error(Type type, std::string text, int severness = 0) :
			m_type(type),
			m_text(text),
			m_severness(severness)
		{}

		// get error type
		Type getType() { return m_type; }

		// get text of error
		std::string getText() { return m_text; }
		
		// get how severe the error is
		int getSeverness() { return m_severness; }
	protected:
		Type m_type;
		std::string m_text;
		int m_severness;
	};
}

#endif // !RHEA_ERROR_H
