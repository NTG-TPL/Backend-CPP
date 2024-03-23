#pragma once

#include "string_view"

namespace http_handler {
using namespace std::string_view_literals;
/**
 * Структура для хранения эндпоинтов
 */
struct EndPoint {
    EndPoint() = delete;
    constexpr const static std::string_view API        = "/api/"sv;
    constexpr const static std::string_view MAPS       = "/api/v1/maps"sv;
    constexpr const static std::string_view MAP_PREFIX = "/api/v1/maps/"sv;
    constexpr const static std::string_view GAME       = "/api/v1/game"sv;
    constexpr const static std::string_view JOIN       = "/api/v1/game/join"sv;
    constexpr const static std::string_view PLAYERS    = "/api/v1/game/players"sv;
    constexpr const static std::string_view STATE      = "/api/v1/game/state"sv;
    constexpr const static std::string_view ACTION     = "/api/v1/game/player/action"sv;
    constexpr const static std::string_view TICK       = "/api/v1/game/tick"sv;
    constexpr const static std::string_view RECORDS    = "/api/v1/game/records"sv;
    constexpr const static std::string_view EMPTY      = "/"sv;
    constexpr const static std::string_view INDEX      = "/index.html"sv;
};
}  // namespace http_handler