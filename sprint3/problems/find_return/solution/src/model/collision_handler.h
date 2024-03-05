#pragma once
#include <memory>
#include <variant>

#include "dog.h"
#include "map.h"
#include "collision_detector.h"

namespace model {
    class ItemGatherer : public ItemGathererProvider {
    public:
        using Items = std::vector<Item>;
        using Gatherers = std::vector<Gatherer>;

        void Set(const Gatherers& gatherers) {
            gatherers_ = gatherers;
        }
        void Set(const Items& items) {
            items_ = items;
        }
        void Add(Item&& item) {
            items_.push_back(std::move(item));
        }
        void Add(Gatherer&& gatherer) {
            gatherers_.push_back(std::move(gatherer));
        }
        virtual size_t ItemsCount() const override {
            return items_.size();
        }
        virtual Item GetItem(size_t idx) const override {
            return items_[idx];
        }
        virtual size_t GatherersCount() const override {
            return gatherers_.size();
        }
        virtual Gatherer GetGatherer(size_t idx) const override {
            return gatherers_[idx];
        }
    private:
        Items items_;
        Gatherers gatherers_;
    };
} // namespace model
