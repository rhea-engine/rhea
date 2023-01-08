#ifndef PSIO_NET_HTTP_HTTPHEADERS_H
#define PSIO_NET_HTTP_HTTPHEADERS_H

#include <string>
#include <vector>

#include <unordered_map>

#include "../UrlRequest/Url.h"

namespace Psio
{
	class HttpHeaders
	{
	public:
		struct HttpHeaderPair
		{
			HttpHeaderPair() = default;
			HttpHeaderPair(const std::string& key, const std::string& value) :
				key(key.data(), key.size()), value(value.data(), value.size()) 
			{}
			void AppendValue(const std::string& val) { value += val; }

			std::string key;
			std::string value;
		};
		HttpHeaders() :
			m_major(0),
			m_minor(0),
			m_res_code(0),
			m_code_valid(false),
			m_raw_method("GET")
		{}

		// set method
		void SetMethod(std::string mth);
		std::string GetMethod() { return m_raw_method; }

		std::string GenerateRequestString(std::string& url);

		// set the protocol version
		void SetVersion(int major, int minor);

		// get protocol version info
		int GetMajor() { return m_major; }
		int GetMinor() { return m_minor; }

		// set status code
		bool SetResponseCode(int code);
		//get status code
		int GetResponseCode() { return m_res_code; }

		// add a header
		void SetHeader(const std::string& key, const std::string& value);
		// add value to last added header (used for line folding)
		void AppendValue(const std::string& key, const std::string& value);

		std::string ToString();

		/* Create Headers from buffer fromatted like a response
		* 
		* The buffer needs to start with a status line - for http/0.9 he caller has to fake one
		* 
		* returns a new HttpHeader object. On error no object is created and NULL is returned.
		*/
		static HttpHeaders* CreateFromBuf(std::string input);		

		static std::unordered_map<int, std::string> Codes;		

		// access header value for header with name key
		std::string& operator[](const std::string key);

	protected:
		std::unordered_map<std::string, HttpHeaderPair> m_headers;

		std::string m_raw_headers;
		std::string m_raw_method;

		int m_res_code;
		bool m_code_valid;

		int m_major;
		int m_minor;
	};
}

#endif // !PSIO_NET_HTTP_HTTPHEADERS_H
