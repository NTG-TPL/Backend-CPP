#ifndef GAME_SERVER_ENDPOINT_H
#define GAME_SERVER_ENDPOINT_H
#include "string_view"

namespace http_handler {
    using namespace std::string_view_literals;
    struct EndPoint {
        EndPoint() = delete;
        constexpr const static std::string_view API = "/api/"sv;
        constexpr const static std::string_view MAPS = "/api/v1/maps"sv;
        constexpr const static std::string_view MAP = "/api/v1/maps/"sv;
        constexpr const static std::string_view GAME = "/api/v1/game"sv;
        constexpr const static std::string_view JOIN = "/api/v1/game/join"sv;
        constexpr const static std::string_view PLAYERS = "/api/v1/game/players"sv;
        constexpr const static std::string_view EMPTY = "/"sv;
        constexpr const static std::string_view INDEX = "/index.html"sv;
    };
}
#endif //GAME_SERVER_ENDPOINT_H
