#pragma once
#include <stdexcept>
#include <unordered_set>

#include "map.h"
#include "dog.h"
#include "../loot_generator/loot_generator.h"
#include "item_gatherer.h"

namespace model {

class GameSession {
public:
    using Id = util::Tagged<uint64_t, GameSession>;
    using IdHasher = util::TaggedHasher<Id>;
    using Dogs = std::unordered_map<Dog::Id, std::shared_ptr<model::Dog>, Dog::IdHasher>;
    using Loots = std::unordered_map<Loot::Id, Loot, Loot::IdHasher>;

    GameSession(Id id, std::shared_ptr<const Map> map, loot_gen::LootGenerator gen):
            id_(id), map_(std::move(map)), loot_generator_(std::move(gen)), limit_(map_->GetLimitPlayers()) {}

    const Map::Id& GetMapId() const noexcept;
    std::shared_ptr<const Map> GetMap() const noexcept;
    const Loots& GetLoots() const noexcept;
    Id GetId() const noexcept;

    size_t GetActivityPlayers() const noexcept;
    size_t GetLimitPlayers() const noexcept;
    size_t AmountAvailableSeats() const noexcept;
    bool IsFull() const noexcept;
    std::optional<std::shared_ptr<model::Dog>> FindDog(const Dog::Id& id);
    std::shared_ptr<model::Dog> AddDog(const model::Dog& dog);
    void DeleteDog(const Dog::Id& dog_id);
    size_t EraseDog(const Dog::Id& id);
    [[nodiscard]] const Dogs& GetDogs() const noexcept;
    [[nodiscard]] loot_gen::LootGenerator::TimeInterval GetLootTimeInterval() const noexcept;
    [[nodiscard]] double GetLootProbability() const noexcept;

    Point2d GenerateNewPosition(bool enable = true) const;
    void GenerateLoot(std::chrono::milliseconds tick, bool enable = true);
    void Update(std::chrono::milliseconds tick);
    const Loot& AddLoot(const Loot& loot);

private:
    void DetectCollisionWithRoadBorders(const std::shared_ptr<model::Dog>& dog, Point2d current_position, Point2d new_position);
    void CollectingAndReturningLoot(ItemGatherer& item_gatherer, std::unordered_map<size_t, std::shared_ptr<model::Dog>>& gather_by_index);

private:
    Id id_;
    std::shared_ptr<const Map> map_;
    loot_gen::LootGenerator loot_generator_;
    size_t limit_;
    Dogs dogs_;
    Loots loots_;
    size_t loot_id_ = 0;
};

class DogDropOffGenerator {
public:
    static Point2d GenerateDogPosition(const GameSession& session, bool enable = true);
};

} // namespace model