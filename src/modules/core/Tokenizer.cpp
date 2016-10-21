#include "Tokenizer.h"

namespace core {

Tokenizer::Tokenizer(const char* s, std::size_t len, const char *sep) :
		_posIndex(0u), _len((int32_t)len) {
	bool lastCharIsSep = false;
	for (;;) {
		char c = skip(&s);
		if (c == '\0') {
			if (lastCharIsSep) {
				_tokens.push_back("");
			}
			break;
		}
		lastCharIsSep = false;
		std::string token;
		if (c == '"') {
			++s;
			--_len;
			for (;;) {
				c = *s++;
				--_len;
				if (c == '"' || c == '\0' || _len <= 0) {
					_tokens.push_back(token);
					break;
				} else if (c == '\\') {
					const char next = *s;
					if (next == 'n') {
						c = '\n';
					} else if (next == 't') {
						c = '\t';
					} else if (next == '"') {
						c = '"';
					}
					++s;
				}
				token.push_back(c);
			}
			continue;
		}

		lastCharIsSep = isSeparator(c, sep);
		if (lastCharIsSep) {
			_tokens.push_back("");
			++s;
			--_len;
			continue;
		}
		token.push_back(c);
		for (;;) {
			++s;
			--_len;
			c = *s;
			if (c < ' ' || _len <= 0) {
				break;
			}
			lastCharIsSep = isSeparator(c, sep);
			if (lastCharIsSep) {
				++s;
				--_len;
				break;
			}
			token.push_back(c);
		}
		_tokens.push_back(token);
	}

	_size = _tokens.size();
}

bool Tokenizer::isSeparator(char c, const char *sep) {
	const char *sepPtr = sep;
	while (*sepPtr != '\0') {
		if (c == *sepPtr) {
			return true;
		}
		++sepPtr;
	}
	return false;
}

char Tokenizer::skip(const char **s) {
	if (_len <= 0) {
		return '\0';
	}
	char c;
	while ((c = **s) <= ' ') {
		if (c == '\0' || _len <= 0) {
			return '\0';
		}
		const size_t cl = core::string::utf8LengthChar(c);
		_len -= cl;
		*s += cl;
	}

	// skip multiline and singleline comments
	if (c == '/') {
		const char next = (*s)[1];
		if (next == '*') {
			int l = 0;
			_len -= 2;
			*s += 2;
			const char* data = *s;
			while (data[l] != '\0' && data[l] != '*' && data[l + 1] != '\0' && data[l + 1] != '/') {
				++l;
			}
			*s += l + 2;
			_len -= l + 2;
			return skip(s);
		} else if (next == '/') {
			while (**s != '\0' && **s != '\n') {
				(*s)++;
				if (--_len <= 0) {
					return '\0';
				}
			}
			return skip(s);
		}
	}

	return c;
}

}
