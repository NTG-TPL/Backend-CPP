#pragma once
#include <unordered_map>
#include <stdexcept>

#include "geom.h"
#include "road.h"
#include "building.h"
#include "office.h"
#include "loot.h"

namespace model {
class Map {
public:
    using Id = util::Tagged<std::string, Map>;
    using IdHasher = util::TaggedHasher<Map::Id>;
    using Roads = std::vector<Road>;
    using Buildings = std::vector<Building>;
    using Offices = std::vector<Office>;
    using LootTypes = std::vector<LootType>;

    Map(Id id, std::string name, DimensionDouble dog_speed, size_t bag_capacity, size_t limit_players = 100) noexcept:
            id_(std::move(id)), name_(std::move(name)),
            dog_speed_(dog_speed), bag_capacity_(bag_capacity),
            limit_players_(limit_players) {
    }

    const Id& GetId() const noexcept;
    const std::string& GetName() const noexcept;
    const Buildings& GetBuildings() const noexcept;
    const Roads& GetRoads() const noexcept;
    const Offices& GetOffices() const noexcept;
    DimensionDouble GetDogSpeed() const noexcept;
    const LootTypes& GetLootTypes() const noexcept;
    size_t GetBagCapacity() const noexcept;
    void AddRoad(const Road& road);
    void AddBuilding(const Building& building);
    void AddOffice(const Office& office);
    void AddLootType(const LootType& office);
    size_t GetLimitPlayers() const noexcept;

private:
    using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

    Id id_;
    std::string name_;
    DimensionDouble dog_speed_;
    size_t bag_capacity_;

    Roads roads_;
    Buildings buildings_;

    OfficeIdToIndex warehouse_id_to_index_;
    Offices offices_;
    LootTypes loot_types_;
    size_t limit_players_;
};
} // namespace model