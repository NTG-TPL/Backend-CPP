#pragma once
#include <string>
#include <optional>
#include "../tagged.h"

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
    std::optional<DimensionDouble > scale;
    std::optional<DimensionInt> value;
};

class Loot : public Object{
public:
    using Id = util::Tagged<size_t, Loot>;
    using IdHasher = util::TaggedHasher<Loot::Id>;

    Loot(Id id, LootType type, Point2d position, size_t loot_index) noexcept:
            id_(id), loot_type_(std::move(type)),
            Object(position, ObjectWidth::ITEM_WIDTH), loot_index_(loot_index) {
    }

    [[nodiscard]] const LootType& GetLootType() const noexcept { return loot_type_;}
    [[nodiscard]] size_t GetTypeIndex() const noexcept {return loot_index_;}
    [[nodiscard]] Id GetId() const noexcept {return id_;}
    [[nodiscard]] std::optional<DimensionInt> GetValue() const noexcept {return loot_type_.value;}

private:
    Id id_;
    LootType loot_type_;
    size_t loot_index_;
};

} // namespace model