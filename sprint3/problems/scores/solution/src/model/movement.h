#pragma once
#include <unordered_map>
#include <string_view>

#include "geom.h"

namespace model {
using namespace std::string_view_literals;
/**
* Структура для хранения движения собак
*/
struct Movement {
    using Function = Velocity2d (*const)(DimensionDouble);

    Movement() = delete;
    constexpr const static std::string_view UP    = "U"sv;
    constexpr const static std::string_view DOWN  = "D"sv;
    constexpr const static std::string_view LEFT  = "L"sv;
    constexpr const static std::string_view RIGHT = "R"sv;
    constexpr const static std::string_view STOP  = ""sv;

    static Velocity2d MoveUp(DimensionDouble speed){ return {0.0, -speed}; }
    static Velocity2d MoveDown(DimensionDouble speed){ return {0.0, speed}; }
    static Velocity2d MoveLeft(DimensionDouble speed){ return {-speed, 0.0}; }
    static Velocity2d MoveRight(DimensionDouble speed){ return {speed, 0.0}; }
    static Velocity2d Stand(DimensionDouble _ = 0.0){ return {0.0, 0.0};}

    static std::unordered_map<std::string_view, Function> InitMovement() {
        return {{Movement::UP,    MoveUp},
                {Movement::DOWN,  MoveDown},
                {Movement::LEFT,  MoveLeft},
                {Movement::RIGHT, MoveRight},
                {Movement::STOP,  Stand}};
    }

    static inline const std::unordered_map<std::string_view, Function> MOVEMENT = InitMovement();
};
} // namespace model