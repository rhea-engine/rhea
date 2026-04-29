/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef PSIO_NET_URL_H
#define PSIO_NET_URL_H

#include <string>

namespace psio
{
	class Url {
	public:
		Url() :
			m_is_valid(false),
			m_is_empty(true),
			m_port(0),
			m_is_cryptographic(false)
		{}
		Url(const std::string& url);



		// empty url?
		bool IsEmpty() { return m_is_empty;	}

		// valid url?
		bool IsValid() { return m_is_valid; }

		// get the varous components
		std::string Hostname() { return m_hostname; }
		std::string Scheme() { return m_scheme; }
		int Port() { return m_port; }
		std::string Username() { return m_username; }
		std::string Password() { return m_password; }
		std::string Path() { return m_path; }
		bool IsCryptographic() { return m_is_cryptographic; }

	protected:
		bool m_is_empty;
		bool m_is_valid;

		std::string m_hostname;
		std::string m_scheme;
		int m_port;
		std::string m_password;
		std::string m_username;
		std::string m_path;

		bool m_is_cryptographic;
	};
}

#endif // !PSIO_NET_URL_H
