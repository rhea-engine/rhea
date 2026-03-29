#include <Psio/Net/UrlRequest/Url.h>
#include <vector>

namespace Psio
{
	Url::Url(const std::string& url) :
		Url()
	{
		m_is_empty = url.length() == 0;

		enum State {
			PARSE_NONE,
			PARSE_SCHEME,
			PARSE_ROOT,
			PARSE_HOSTNAME,
			PARSE_PORT,
			PARSE_PATH_START,
			PARSE_PATH
		};
		int state = State::PARSE_NONE;

		size_t tok; 
		std::string port;
		
		enum class TokenType {
			TOKEN_STRING,
			TOKEN_COLON,
			TOKEN_AT,
			TOKEN_QUESTION,
			TOKEN_SEPARATOR,
			TOKEN_LEFT_BRACKET,
			TOKEN_RIGHT_BRACKET
		};

		struct Token
		{
			TokenType type = TokenType::TOKEN_STRING;
			std::string value;
		};

		size_t i = 0;
		std::vector<Token> tokens;

		// tokenizer
		tok = 0;
		while (i < url.length()) {
			if (url[i] == ':' || url[i] == '/' || url[i] == '@' || url[i] == '?' || url[i] == '[' || url[i] == ']') {
				// was a token started?
				if (i - tok > 0)
					tokens.push_back({ TokenType::TOKEN_STRING, url.substr(tok, (i - tok)) });
				tok = i + 1;
				switch (url[i]) {
				case ':':
					tokens.push_back({ TokenType::TOKEN_COLON });
					break;
				case '/':
					tokens.push_back({ TokenType::TOKEN_SEPARATOR });
					break;
				case '@':
					tokens.push_back({ TokenType::TOKEN_AT });
					break;
				case '?':
					tokens.push_back({ TokenType::TOKEN_QUESTION });
					break;
				case '[':
					tokens.push_back({ TokenType::TOKEN_LEFT_BRACKET });
					break;
				case ']':
					tokens.push_back({ TokenType::TOKEN_RIGHT_BRACKET });
					break;

				}
			}
			i++;
		}
		// was a token started?
		if (i - tok > 0)
			tokens.push_back({ TokenType::TOKEN_STRING, url.substr(tok, (i - tok)) });

		state = State::PARSE_SCHEME;
		for (auto it = tokens.begin(); it < tokens.end(); ++it) {
			switch (state) {
			case State::PARSE_SCHEME:
				if ((*it).type != TokenType::TOKEN_STRING)
					return;
				m_scheme = (*it).value;
				if (m_scheme.length() == 4 && !m_scheme.compare("http"))
					m_port = 80;
				if (m_scheme.length() == 5 && !m_scheme.compare("https")) {
					m_port = 443;
					m_is_cryptographic = true;
				}
				state = State::PARSE_ROOT;
				break;
			case State::PARSE_ROOT:
				if ((*it).type == TokenType::TOKEN_SEPARATOR ||
					(*it).type == TokenType::TOKEN_COLON)
					break;
				state = State::PARSE_HOSTNAME;
				// intentional fall-through
			case State::PARSE_HOSTNAME:
				if ((*it).type != TokenType::TOKEN_STRING)
					return;
				m_hostname = (*it).value;
				state = State::PARSE_PORT;
				break;
			case State::PARSE_PORT:
				switch ((*it).type) {
				case TokenType::TOKEN_AT:
					m_username = m_hostname;
					m_hostname = "";
					state = State::PARSE_HOSTNAME;
					break;
				case TokenType::TOKEN_COLON:
					break;
				case TokenType::TOKEN_SEPARATOR:
					// allow multiple separators
					state = State::PARSE_PATH_START;
					m_path = "/";
					break;
				case TokenType::TOKEN_QUESTION:
					state = State::PARSE_PATH_START;
					m_path = "/?";
					break;
				case TokenType::TOKEN_STRING:
					port = (*it).value;
					state = State::PARSE_PATH_START;
					break;
				default:
					return;
				}
				break;
			case State::PARSE_PATH_START:
				switch ((*it).type) {
				case TokenType::TOKEN_AT:
					m_username = m_hostname;
					m_hostname = "";
					m_password = port;
					state = State::PARSE_HOSTNAME;
					break;
				case TokenType::TOKEN_SEPARATOR:
					state = State::PARSE_PATH;
					m_path = "/";
					break;
				case TokenType::TOKEN_QUESTION:
					state = State::PARSE_PATH;
					m_path = "/?";
					break;
				case TokenType::TOKEN_STRING:
					// this can only happen after some separator
					state = State::PARSE_PATH;
					m_path += (*it).value;
					break;
				default:
					return;
				}
				break;
			case State::PARSE_PATH:
				switch ((*it).type) {
				case TokenType::TOKEN_STRING:
					m_path += (*it).value;;
					break;
				case TokenType::TOKEN_AT:
					m_path += "@";
					break;
				case TokenType::TOKEN_COLON:
					m_path += ":";
					break;

				case TokenType::TOKEN_QUESTION:
					m_path += "?";
					break;
				case TokenType::TOKEN_SEPARATOR:
					m_path += "/";
					break;
				case TokenType::TOKEN_LEFT_BRACKET:
					m_path += "[";
					break;
				case TokenType::TOKEN_RIGHT_BRACKET:
					m_path += "]";
					break;
				}
				break;
			}
		}

		if (port != "") {
			m_port = 0;
			for (int j = 0; j < port.length(); j++)
				m_port = m_port * 10 + port[j] - '0';
		}

		if (!m_scheme.compare("file") && m_port > 0)
			return;
		if (!m_hostname.length())
			return;

		if (!m_path.length())
			m_path = "/";

		m_is_valid = true;
	}
}