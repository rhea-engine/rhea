/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Resources/Resource.h>
#include <Rhea/Resources/ResourceList.h>
#include <Psio/Log.h>

namespace rhea {
	
	Resource::~Resource()
	{
		if (m_buf) {
			free(m_buf);
			m_buf = nullptr;
		}
	}

	void Resource::load(orion::ResourceRequest* req, ReadyHandler handler)
	{
		handler(ReqError::OK);
	}

	void Resource::buildInstance(const InstanceRequest&, InstanceHandler handler)
	{
		if (handler)
			handler(Error(Error::Code::UNAVAILABLE, "The requested operation is not implemented."), nullptr);
	}

	void Resource::ensureReady(InstanceHandler handler)
	{
		if (handler)
			handler(Error(), this);
	}
	
	void Resource::prepare(PrepareHandler handler)
	{
		m_prepared = true;
		handler(Error());
	}

	void Resource::setRaw(size_t buflen, char* buf)
	{
		// delete old content?
		if (m_buf) {
			free(m_buf);
			m_buf = nullptr;
		}

		// reset requested?
		if (buflen && !buf) {
			m_buflen = 0;
			m_buf = nullptr;
		}
		else {
			m_buflen = buflen;
			m_buf = buf;
		}
		m_status = Status::LOADED;
	}

	rhea::Error translateLoadError(orion::ResourceRequest::Error error, const std::string& rid)
	{
		using Error = rhea::Error;
		using Code = Error::Code;

		Code code = Code::RESOURCE_ERROR;
		std::string message;

		switch (error) {
		case ReqError::OK:
			return {};
		case ReqError::REALM_NOT_FOUND:
			code = Code::NOT_FOUND;
			message = "Realm could not be resolved.";
			break;
		case ReqError::RESOURCE_NOT_FOUND:
			code = Code::NOT_FOUND;
			message = "Resource was not found.";
			break;
		case ReqError::PATH_NOT_FOUND:
			code = Code::NOT_FOUND;
			message = "Resource path was not found.";
			break;
		case ReqError::NO_AUTH:
			code = Code::AUTHENTICATION_FAILED;
			message = "Authentication is required or has failed.";
			break;
		case ReqError::NO_ACCESS:
			code = Code::ACCESS_DENIED;
			message = "Access to the resource was denied.";
			break;
		case ReqError::CONFIG_ERR:
			code = Code::INVALID_DEFINITION;
			message = "Resource configuration is invalid.";
			break;
		case ReqError::DATA_INVALID:
			code = Code::INVALID_DATA;
			message = "Resource data is invalid.";
			break;
		case ReqError::NOT_IMPLEMENTED:
			code = Code::UNAVAILABLE;
			message = "The requested operation is not implemented.";
			break;
		default:
			message = "Resource loading did not complete successfully.";
			break;
		}

		Error result(code, message, rid);
		result.addContext("request_error", std::to_string(static_cast<int>(error)));
		return result;
	}

}
