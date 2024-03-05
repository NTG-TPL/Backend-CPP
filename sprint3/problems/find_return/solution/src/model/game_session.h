#pragma once
#include <stdexcept>
#include <unordered_set>

#include "map.h"
#include "dog.h"
#include "../loot_generator/loot_generator.h"
#include "collision_handler.h"
namespace model {

class GameSession {
public:
    using Dogs = std::unordered_map<Dog::Id, Dog, Dog::IdHasher>;
    using Loots = std::unordered_map<Loot::Id, std::shared_ptr<Loot>, Loot::IdHasher>;

    GameSession(const Map& map, loot_gen::LootGenerator gen):
            map_(map), loot_generator_(std::move(gen)), limit_(100) {}
    GameSession(const Map& map, loot_gen::LootGenerator gen, size_t limit):
            map_(map), loot_generator_(std::move(gen)), limit_(limit) {}
    const Map::Id& GetMapId() const noexcept;
    const Map& GetMap() const noexcept;
    const Loots& GetLoots() const noexcept;

    size_t GetActivityPlayers() const noexcept;
    size_t GetLimitPlayers() const noexcept;
    size_t AmountAvailableSeats() const noexcept;
    bool IsFull() const noexcept;
    Dog* FindDog(const Dog::Id& id);
    Dog* AddDog(const Dog& dog);
    size_t EraseDog(const Dog::Id& id);
    [[nodiscard]] const Dogs& GetDogs() const noexcept;
    Point2d GenerateNewPosition(bool enable = true) const;
    void GenerateLoot(std::chrono::milliseconds tick, bool enable = true);
    void Update(std::chrono::milliseconds tick);

private:
    void DetectCollisionWithRoadBorders(Dog& dog, Point2d current_position, Point2d new_position);
    void CollectingAndReturningLoot(ItemGatherer& item_gatherer, std::unordered_map<size_t, model::Dog&> gather_by_index);
private:
    const Map& map_;
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