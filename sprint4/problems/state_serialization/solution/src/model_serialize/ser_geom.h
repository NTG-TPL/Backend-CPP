#pragma once

#include "../model/geom.h"

namespace model {

template <typename Archive, typename T>
void serialize(Archive& ar, model::Point<T>& point, [[maybe_unused]] const unsigned version) {
    ar& point.x;
    ar& point.y;
}

template <typename Archive, typename T>
void serialize(Archive& ar, model::Velocity<T>& velocity, [[maybe_unused]] const unsigned version) {
    ar& velocity.dx;
    ar& velocity.dy;
}

} // namespace model