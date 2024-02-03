#ifndef GAME_SERVER_ERROR_RESPONSE_H
#define GAME_SERVER_ERROR_RESPONSE_H
#include "string_view"

namespace http_handler {
    using namespace std::string_view_literals;
    struct ErrorResponse {
        ErrorResponse() = delete;
        constexpr static std::string_view MAP_NOT_FOUND  = R"({"code": "mapNotFound", "message": "Map not found"})"sv;
        constexpr static std::string_view BAD_REQ        = R"({"code": "badRequest", "message": "Bad request"})"sv;
        constexpr static std::string_view FILE_NOT_FOUND = R"({"code": "NotFound", "message": "File not found"})"sv;
        constexpr static std::string_view BAD_PARSE      = R"({"code": "invalidArgument", "message": "Join game request parse error"})"sv;
        constexpr static std::string_view USERNAME_EMPTY = R"({"code": "invalidArgument", "message": "Invalid name"})"sv;
        constexpr static std::string_view INVALID_TOKEN  = R"({"code": "invalidToken", "message": "Authorization header is missing"})"sv;
        constexpr static std::string_view UNKNOWN_TOKEN  = R"({"code": "unknownToken", "message": "Player token has not been found"})"sv;
        constexpr static std::string_view INVALID_POST   = R"({"code": "invalidMethod", "message": "Only POST method is expected"})"sv;
        constexpr static std::string_view INVALID_GET    = R"({"code": "invalidMethod", "message": "Invalid method"})"sv;
    };
}

#endif //GAME_SERVER_ERROR_RESPONSE_H
