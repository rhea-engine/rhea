/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef ORION_REALMREQUEST_H
#define ORION_REALMREQUEST_H

#include <Psio/Net/UrlRequest/UrlRequest.h>
#include <Orion/Realm.h>
#include <Orion/RequestError.h>

#include <string>
#include <deque>

namespace orion
{
	class Orion;

	class RealmRequest
	{
	public:

		RealmRequest(Orion* orion, std::deque<std::string>& iris);
		~RealmRequest();

		typedef std::function<void(Realm*, orion::Error error)> RealmHandler;

		// Lookup a realm - call handler with realm or error
		// the realm is not allocated and will be freed on destruction of the client
		// (the iris client is used by the cache system?)
		void Lookup(std::string realm_name, RealmHandler handler);

		// Cancel the current request
		void Cancel();

	protected:

		psio::UrlRequest* m_request;
		int m_error;
		std::string m_response;

		RealmHandler m_handler;

		Orion* m_orion;

		std::deque<std::string> m_iris;
	};
}

#endif // !ORION_REALMREQUEST_H
