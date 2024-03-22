#pragma once
#include "geom.h"

namespace model {

class Building {
public:
    explicit Building(Rectangle bounds) noexcept: bounds_{bounds} {}

    [[nodiscard]] const Rectangle& GetBounds() const noexcept;

private:
    Rectangle bounds_;
};

} // namespace model