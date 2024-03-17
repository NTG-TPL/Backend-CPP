#pragma once

#include "../model/loot.h"

namespace serialization {

using namespace model;
class LootRepr {
public:
    LootRepr() = default;

    explicit LootRepr(const model::Loot& loot) noexcept:
            id_(loot.GetId()),
            value_(loot.GetValue()),
            position_(loot.GetPosition()),
            type_(loot.GetType()) {
    }

    [[nodiscard]] model::Loot Restore() const {
        return Loot{id_, value_, position_, type_};
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar&* id_;
        ar& value_;
        ar& position_;
        ar& type_;
    }

private:
    Loot::Id id_{0};
    DimensionInt value_{};
    Point2d position_;
    size_t type_{};
};

} // namespace model