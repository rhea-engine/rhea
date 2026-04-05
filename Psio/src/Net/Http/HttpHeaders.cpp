#include <Psio/Net/Http/HttpHeaders.h>
#include <Psio/Util.h>


namespace psio
{
	std::unordered_map<int, std::string> HttpHeaders::Codes = {
		{200, "OK"},
		{301, "Moved Permanently"},
		{400, "Bad Request"},
		{401, "Unauthorized"},
		{403, "Forbidden"},
		{404, "Not Found"},
		{500, "Internal Server Error"}

	};

	void HttpHeaders::SetMethod(std::string mth)
	{
		m_raw_method = mth;
	}

	void HttpHeaders::SetVersion(int major, int minor)
	{
		m_major = major;
		m_minor = minor;
	}

	bool HttpHeaders::SetResponseCode(int code)
	{
		m_res_code = code;
		auto it = Codes.find(code);
		if (it == Codes.end()) {
			m_raw_headers.append("HTTP/" + std::to_string(m_major) + "." + std::to_string(m_minor) + " " + std::to_string(m_res_code) + " Code not implemented\n");
			return false;
		}
		m_code_valid = true;
		m_raw_headers.append("HTTP/" + std::to_string(m_major) + "." + std::to_string(m_minor) + " " + std::to_string(m_res_code) + " " + it->second + "\n");
		return true;
	}

	void HttpHeaders::SetHeader(const std::string& key, const std::string& value)
	{
		m_headers[str_tolower(key)] = HttpHeaderPair(key, value);
	}

	void HttpHeaders::AppendValue(const std::string& key, const std::string& value)
	{
		m_headers[str_tolower(key)].AppendValue(value);
	}
	
	std::string& HttpHeaders::operator[](const std::string key) {	
		std::string key_low = str_tolower(key);
		auto it = m_headers.find(key_low);
		if (it != m_headers.end())
			return it->second.value;		
		else {
			m_headers[key_low] = { key, std::string("") };
			return m_headers[key_low].value;
		}
	}

	std::vector<std::string> HttpHeaders::GetHeaderList()
	{
		std::vector<std::string> headers;

		for (auto it = m_headers.begin(); it != m_headers.end(); ++it) {
			headers.push_back(it->second.key);
		}
		return headers;
	}

	std::string HttpHeaders::ToString()
	{
		std::string output;
		for (auto it = m_headers.begin(); it != m_headers.end(); ++it) {
			output += it->second.key.c_str() + std::string(": ") + it->second.value.c_str() + std::string("\r\n");
		}
		output.append("\r\n");
		return output;
	}
	
	HttpHeaders* HttpHeaders::CreateFromBuf(std::string input)
	{
		HttpHeaders* headers = new HttpHeaders();
		headers->m_raw_headers.reserve(input.size());

		enum State {
			STATUS_NONE,
			STATUS_MAJOR,
			STATUS_MINOR,
			STATUS_CODE,
			STATUS_TEXT,

			HTTP_HEADER_NONE,
			HTTP_HEADER_KEY,
			HTTP_HEADER_VALUE_START,
			HTTP_HEADER_VALUE
		};

		// find end of status line and start of headers
		size_t i;
		size_t end_of_status = input.find_first_of("\r\n");
		if (end_of_status == std::string::npos) {
			end_of_status = input.length();
			i = end_of_status;
		}
		else {
			i = end_of_status + 2;
		}

		// parse status line

		// check: starts with http?
		if (end_of_status < 4 || strncmp_i(input.data(), "http", 4)) {
			delete headers;
			return NULL;
		}

		int state = State::STATUS_NONE;
		int j = 4;
		int major = 0;
		int minor = 0;
		int code = 0;
		int tok_start;
		std::string restext;
		do {
			switch (state) {
			case State::STATUS_NONE:
				if (input[j] != '/') {
					// no version! perhaps response code?
					if (input[j] == ' ') {
						state = State::STATUS_CODE;
						break;
					}
					delete headers;
					return NULL;
				}
				state = State::STATUS_MAJOR;
				break;
			case State::STATUS_MAJOR:
				// skip ws and 0
				if (input[j] == ' ' || input[j] == 0)
					break;
				if (!(input[j] >= '0' && input[j] <= '9')) {
					state = State::STATUS_MINOR;
					break;
				}
				major *= 10;
				major += (input[j] - '0');
				break;
			case State::STATUS_MINOR:
				// skip 0
				if (input[j] == 0)
					break;
				if (!(input[j] >= '0' && input[j] <= '9')) {
					state = State::STATUS_CODE;
					break;
				}
				minor *= 10;
				minor += (input[j] - '0');
				break;
			case State::STATUS_CODE:
				// skip ws and 0
				if (input[j] == ' ')
					break;
				if (!(input[j] >= '0' && input[j] <= '9')) {
					state = State::STATUS_TEXT;
					tok_start = j;
					break;
				}
				code *= 10;
				code += (input[j] - '0');
				break;
			case State::STATUS_TEXT:
				// read up to line feed
				if (input[j] == '\r') {
					restext = std::string(input.data() + tok_start, (size_t)j - 1);
					j++;
				}
				break;
			}
			j++;
		} while (j < end_of_status);
		headers->SetVersion(major, minor);
		headers->SetResponseCode(code);

		// now parse headers
		ParseHeaders(input, i, headers);
		
		return headers;
	}
	
	void HttpHeaders::ParseHeaders(std::string input, size_t offset, HttpHeaders* headers)
	{
		enum State {
			STATUS_NONE,
			STATUS_MAJOR,
			STATUS_MINOR,
			STATUS_CODE,
			STATUS_TEXT,

			HTTP_HEADER_NONE,
			HTTP_HEADER_KEY,
			HTTP_HEADER_VALUE_START,
			HTTP_HEADER_VALUE
		};

		HttpHeaderPair* current_header = nullptr;

		size_t key_start = 0;
		size_t key_len = 0;
		size_t val_start = 0;
		size_t val_len = 0;
		int	state = State::HTTP_HEADER_NONE;
		bool continuation = false;
		size_t i = offset;

		while (i < input.length()) {

			switch (state) {
			case State::HTTP_HEADER_NONE:
				// if its a whitespace we have a header line continuation
				if (input[i] == ' ') {
					continuation = true;
					state = State::HTTP_HEADER_VALUE_START;
					break;
				}
				if (input[i] == '\n')
					break;
				key_start = i;
				state = State::HTTP_HEADER_KEY;
				// intentional drop-through
			case State::HTTP_HEADER_KEY:
				// find colon
				if (input[i] != ':')
					break;
				key_len = i - key_start;
				state = State::HTTP_HEADER_VALUE_START;
				break;
			case State::HTTP_HEADER_VALUE_START:
				// skip whitespace
				if (input[i] == ' ')
					break;
				state = State::HTTP_HEADER_VALUE;
				val_start = i;
				// intentional drop-through
			case State::HTTP_HEADER_VALUE:
				// read up to line-feed
				if (input[i - 1] != '\r' || input[i] != '\n')
					break;
				val_len = i - 1 - val_start;
				if (!continuation) {
					headers->SetHeader(input.substr(key_start, key_len), input.substr(val_start, val_len));
				}
				else {
					continuation = false;
					headers->AppendValue(input.substr(key_start, key_len), input.substr(val_start, val_len));
				}
				state = State::HTTP_HEADER_NONE;
			default:
				break;
			}
			i++;
			
		}
	}

	std::string HttpHeaders::GenerateRequestString(std::string& url)
	{
		std::string request_line = m_raw_method + " " + url + " HTTP/1.1\r\n";
		return request_line;
	}
}