#pragma once
#include <memory>
#include <variant>

#include "dog.h"
#include "map.h"
#include "collision_detector.h"

namespace model {
    class ItemGatherer : public ItemGathererProvider {
    public:
        using Gatherers = std::vector<Gatherer>;
        using Items = std::vector<Item>;

        void Add(Item&& item) { items_.push_back(item); }
        void Add(Gatherer&& gatherer) { gatherers_.push_back(gatherer); }
        [[nodiscard]] size_t ItemsCount() const override { return items_.size(); }
        [[nodiscard]] Item GetItem(size_t idx) const override { return items_[idx]; }
        [[nodiscard]] size_t GatherersCount() const override { return gatherers_.size(); }
        [[nodiscard]] Gatherer GetGatherer(size_t idx) const override { return gatherers_[idx]; }
    private:
        Items items_;
        Gatherers gatherers_;
    };
} // namespace model
