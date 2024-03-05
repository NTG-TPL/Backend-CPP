#include "office.h"

namespace model {

/**
* Получить индекс Office
* @return индекс
*/
const Office::Id& Office::GetId() const noexcept {
    return id_;
}

/**
* Получить смещение объекта Office
* @return смещение
*/
Offset Office::GetOffset() const noexcept {
    return offset_;
}

} // namespace model