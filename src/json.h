#pragma once

#include "logger.h"
#include <stdexcept>

#define RAPIDJSON_NOEXCEPT_ASSERT(x) assert(x)

#ifdef RAPIDJSON_ASSERT
#undef RAPIDJSON_ASSERT
#endif
#define RAPIDJSON_ASSERT(x)	do { \
	if (!(x)) { \
		LOG(warning) << RAPIDJSON_STRINGIFY(x); \
		throw JsonException(RAPIDJSON_STRINGIFY(x)); \
	} } while(0)


#undef RAPIDJSON_HAS_CXX11_RVALUE_REFS
#define RAPIDJSON_HAS_CXX11_RVALUE_REFS 1
#undef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1


struct JsonException : std::runtime_error {
    using runtime_error::runtime_error;
};


#undef RAPIDJSON_PARSE_ERROR_NORETURN
#define RAPIDJSON_PARSE_ERROR_NORETURN(parseErrorCode,offset) \
   throw JsonParseException(parseErrorCode, #parseErrorCode, offset)

#include "third-party/rapidjson/error/error.h"

struct JsonParseException : JsonException, rapidjson::ParseResult {
    JsonParseException(rapidjson::ParseErrorCode code, const char* msg, size_t offset)
        : JsonException(msg), ParseResult(code, offset) {
    }
};

#include "third-party/rapidjson/document.h"
#include "third-party/rapidjson/writer.h"
#include "third-party/rapidjson/stringbuffer.h"
#include "third-party/rapidjson/error/en.h"

inline std::string toString(rapidjson::Value& v) {
    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    v.Accept(writer);
    return strbuf.GetString();
}
