#pragma once

#include "string_view"

namespace http_handler {
using namespace std::string_literals;
using namespace std::string_view_literals;
/**
 * Структура для хранения текста ошибок
 */
struct ErrorResponse {
    ErrorResponse() = delete;
    constexpr static std::string_view MAP_NOT_FOUND    = R"({"code": "mapNotFound", "message": "Map not found"})"sv;
    constexpr static std::string_view FILE_NOT_FOUND   = R"({"code": "NotFound", "message": "File not found"})"sv;
    constexpr static std::string_view BAD_PARSE_JOIN   = R"({"code": "invalidArgument", "message": "Join game request parse error"})"sv;
    constexpr static std::string_view BAD_PARSE_ACTION = R"({"code": "invalidArgument", "message": "Failed to parse action"})"sv;
    constexpr static std::string_view BAD_PARSE_TICK   = R"({"code": "invalidArgument", "message": "Failed to parse tick request JSON"})"sv;
    constexpr static std::string_view USERNAME_EMPTY   = R"({"code": "invalidArgument", "message": "Invalid name"})"sv;
    constexpr static std::string_view INVALID_TOKEN    = R"({"code": "invalidToken", "message": "Authorization header is missing"})"sv;
    constexpr static std::string_view UNKNOWN_TOKEN    = R"({"code": "unknownToken", "message": "Player token has not been found"})"sv;
    constexpr static std::string_view INVALID_POST     = R"({"code": "invalidMethod", "message": "Only POST method is expected"})"sv;
    constexpr static std::string_view INVALID_GET      = R"({"code": "invalidMethod", "message": "Invalid method"})"sv;
    constexpr static auto BAD_REQ = [](const std::string& message = "Bad request"s){
        return R"({"code": "badRequest", "message": ")"s + message  +"\"}"s;
    };
    constexpr static auto SERVER_ERROR = [](const std::string& message){
        return R"({"code": "Server error", "message": ")"s + message  +"\"}"s;
    };
};
}  // namespace http_handler