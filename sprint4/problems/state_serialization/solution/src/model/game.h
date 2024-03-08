#pragma once
#include <memory>
#include <map>

#include "map.h"
#include "game_session.h"
namespace model {

class Game {
public:
    using Maps = std::vector<Map>;
    using Sessions = std::vector<std::shared_ptr<GameSession>>;
    using FullnessToSessionIndex = std::map<size_t, size_t, std::greater<>>;
    using MapIdHasher = util::TaggedHasher<Map::Id>;

    void AddMap(const Map& map);
    const Maps& GetMaps() const noexcept;
    const Map* FindMap(const Map::Id& id) const noexcept;
    std::shared_ptr<GameSession> UpdateSessionFullness(size_t index, const GameSession& session);
    std::pair<size_t, std::shared_ptr<GameSession>>  CreateFreeSession(const Map::Id& map_id);
    std::optional<std::pair<size_t, std::shared_ptr<GameSession>>> ExtractFreeSession(const Map::Id& map_id);
    void Update(std::chrono::milliseconds tick);
    void SetLootGeneratorConfig(double period, double probability) noexcept;
    double GetLootPeriod() const noexcept;
    double GetLootProbability() const noexcept;
private:
    using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;
    using MapIdToSessionIndex = std::unordered_map<Map::Id, FullnessToSessionIndex, MapIdHasher>;

    Maps maps_;
    MapIdToIndex id_to_map_index_;
    Sessions sessions_;
    MapIdToSessionIndex map_to_sessions_;
    double period_ = 0.0, probability_ = 0.0;
};

} // namespace model