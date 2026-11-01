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

		// get the various components
		const std::string& scheme() { return m_scheme; }
		const std::string& hostname() { return m_hostname; }
		bool hasExplicitPort() const;
		int port() { return m_port; }
		const std::string& path() { return m_path; }
		const std::string& username() { return m_username; }
		const std::string& password() { return m_password; }
		const std::string& query() { return m_query; }
		const std::string& fragment() { return m_fragment; }

		// information about the url
		// empty url?
		bool isEmpty() { return m_is_empty; }
		// valid url?
		bool isValid() { return m_is_valid; }
		// has query
		bool hasQuery() { return m_has_query; }
		// has fragment
		bool hasFragment() { return m_has_fragment; }

		bool isCryptographic() { return m_is_cryptographic; }

		// composed getters
		std::string toString() const;         // including fragment
		std::string withoutFragment() const;
		std::string requestTarget() const;    // Path + Query; empty Path → "/"
		std::string hostHeader() const;       // Host[:Port], IPv6 with brackets

		bool resolveReference(const std::string& reference, Url& result) const;


	protected:
		bool m_is_empty = true;
		bool m_is_valid = false;

		std::string m_hostname;
		std::string m_scheme;
		int m_port = 0;
		bool m_has_explicit_port = false;
		std::string m_password;
		std::string m_username;
		std::string m_path;
		std::string m_query;
		std::string m_fragment;

		bool m_has_query = false;
		bool m_has_fragment = false;

		bool m_is_cryptographic = false;
	};
}

#endif // !PSIO_NET_URL_H
