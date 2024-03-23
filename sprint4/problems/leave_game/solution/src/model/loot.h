#pragma once
#include <string>
#include <optional>
#include "../util/tagged.h"

#include "geom.h"

namespace model {

/**
 * Структура для хранения типа клада
 */
struct LootType {
    std::optional<std::string> name;
    std::optional<std::string> file;
    std::optional<std::string> type;
    std::optional<DimensionInt> rotation;
    std::optional<std::string> color;
    std::optional<DimensionDouble> scale;
    DimensionInt value = 0;
};

class Loot : public Object{
public:
    using Id = util::Tagged<size_t, Loot>;
    using IdHasher = util::TaggedHasher<Loot::Id>;

    Loot(Id id, DimensionInt value, Point2d position, size_t type) noexcept:
            id_(id), value_(value),
            Object(position, ObjectWidth::ITEM_WIDTH), type_(type) {
    }

    [[nodiscard]] size_t GetType() const noexcept { return type_; }
    [[nodiscard]] Id GetId() const noexcept {return id_;}
    [[nodiscard]] DimensionInt GetValue() const noexcept {return value_;}
private:
    Id id_;
    DimensionInt value_;
    size_t type_;
};

} // namespace model