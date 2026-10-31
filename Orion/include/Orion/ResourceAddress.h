#ifndef ORION_RESOURCEADDRESS_H
#define ORION_RESOURCEADDRESS_H

#include <string>
#include <string_view>

namespace orion
{
	enum class AddressScheme {
		File,
		Http,
		Https,
		Realm
	};

	enum class AddressErrorCode {
		InvalidSyntax,
		UnsupportedScheme,
		InvalidAuthority,
		InvalidRealmName,
		InvalidEncoding,
		UnsupportedQuery,
		UnsupportedFragment
	};

	struct AddressError {
		AddressErrorCode code = AddressErrorCode::InvalidSyntax;
		std::string message;
		size_t position = 0; // Byte offset into the input of the failing operation.
	};

	struct AddressResult;

	class ResourceAddress
	{
	public:
		static AddressResult parse(std::string_view text);

		AddressScheme scheme() const;
		std::string toString() const;

		std::string authority() const { return m_authority; }
		std::string path() const { return m_path; }
		std::string query() const { return m_query; }

		AddressResult parent() const;
		AddressResult resolveRelative(std::string_view reference) const;

		bool operator==(const ResourceAddress& other) const;

	private:
		// An AddressResult contains a placeholder address on failure.
		// Only successful results expose a usable address.
		ResourceAddress() = default;
		friend struct AddressResult;

		AddressScheme m_scheme = AddressScheme::File;
		std::string m_authority; // Realm name, HTTP host[:port], or file host.
		std::string m_path;      // Percent-encoded absolute path.
		std::string m_query;     // Percent-encoded; HTTP(S) only.
		bool m_has_query = false; // Preserve the difference between / and /?.
	};

	struct AddressResult {
		bool success = false;
		ResourceAddress address;
		AddressError error;
	};
}

#endif // !ORION_RESOURCEADDRESS_H