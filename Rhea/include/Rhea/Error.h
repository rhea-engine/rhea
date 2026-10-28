/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_ERROR_H
#define RHEA_ERROR_H

#include <string>
#include <memory>
#include <vector>
#include <utility>

namespace rhea
{

	class Error
	{
	public:
		enum struct Code {
			NONE,
			ACCESS_DENIED,
			AUTHENTICATION_FAILED,
			NOT_FOUND,
			UNSUPPORTED_FORMAT,
			INVALID_DEFINITION,
			INVALID_DATA,
			INVALID_ARGUMENT,
			UNKNOWN_TYPE,
			DEPENDENCY_FAILED,
			CANCELED,
			UNAVAILABLE,
			RESOURCE_ERROR
		};

		struct Context
		{
			std::string key;
			std::string value;
		};

		Error() = default;

		Error(Code code, std::string message, const std::string& resource_id = {}, int severness = 0)
			: m_code(code),
			m_message(message),
			m_resource_id(resource_id),
			m_severness(severness)
		{}

		bool isError() const { return m_code != Code::NONE; }

		// get error type
		Code getCode() const { return m_code; }

		// get message of error
		const std::string& getMessage() { return m_message; }

		// get how severe the error is
		int getSeverness() { return m_severness; }

		// get the resource id that caused the error
		const std::string& getResourceId() { return m_resource_id; }

		void setResourceId(const std::string& rid) { m_resource_id = rid; }

		const std::vector<Context>& getContext() { return m_context; }
		void addContext(const std::string& key, const std::string& value) { m_context.push_back({ key, value }); }

		// get the cause of the error
		std::shared_ptr<const Error> getCause() { return m_cause; }
		void setCause(std::shared_ptr<const Error> cause) { m_cause = cause; }

	protected:
		Code m_code = Code::NONE;
		std::string m_message;
		int m_severness = 0;
		std::string m_resource_id;
		std::vector<Context> m_context;
		std::shared_ptr<const Error> m_cause;
	};

	enum struct ErrorStatus {
		SUCCESS,
		FAILURE,
		CANCELED,
	};

	struct ErrorReport
	{
		ErrorReport() = default;
		ErrorReport(Error error) {
			addError(error);
		}

		ErrorStatus status = ErrorStatus::SUCCESS;
		std::vector<Error> errors;

		void addError(const Error& error) {
			if (!error.isError())
				return;
			errors.push_back(error);
			if (error.getCode() == Error::Code::CANCELED) {
				if (status == ErrorStatus::SUCCESS)
					status = ErrorStatus::CANCELED;
			}
			else {
				status = ErrorStatus::FAILURE;
			}
		}

		bool isError() const {
			return status != ErrorStatus::SUCCESS;
		}

		bool isCanceled() const {
			return status == ErrorStatus::CANCELED;
		}

		void merge(ErrorReport report)
		{
			// Failure takes precedence over cancellation.
			if (report.status == ErrorStatus::FAILURE)
				status = ErrorStatus::FAILURE;
			else if (report.status == ErrorStatus::CANCELED
				&& status == ErrorStatus::SUCCESS)
				status = ErrorStatus::CANCELED;

			for (auto& error : report.errors)
				errors.push_back(std::move(error));
		}
	};
}

#endif // !RHEA_ERROR_H
