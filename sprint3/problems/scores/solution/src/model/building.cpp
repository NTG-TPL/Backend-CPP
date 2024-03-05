#include "building.h"

namespace model {

/**
 * Получить границы Building
 * @return Rectangle
 */
const Rectangle&  Building::GetBounds() const noexcept {
    return bounds_;
}

} // namespace model