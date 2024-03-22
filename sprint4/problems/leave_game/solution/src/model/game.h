#pragma once
#include <memory>
#include <map>

#include "map.h"
#include "game_session.h"

namespace serialization {
    class GameRepr;
}

namespace model {

class Game {
    // Класс для сериализации объявлен дружественным, чтобы не портить консистентность класса
    friend class serialization::GameRepr;
public:
    using Maps = std::vector<std::shared_ptr<Map>>;
    using Sessions = std::vector<std::shared_ptr<GameSession>>;
    using IndexToSession = std::unordered_map<GameSession::Id, size_t, GameSession::IdHasher>;
    using FullnessToSessionIndex = std::map<size_t, size_t, std::greater<>>;

    void AddMap(const Map& map);
    const Maps& GetMaps() const noexcept;
    std::shared_ptr<const Map> FindMap(const Map::Id& id) const noexcept;
    std::shared_ptr<GameSession> UpdateSessionFullness(GameSession::Id index, const GameSession& session);
    std::pair<GameSession::Id, std::shared_ptr<GameSession>>  CreateFreeSession(const Map::Id& map_id);
    std::optional<std::pair<GameSession::Id, std::shared_ptr<GameSession>>> ExtractFreeSession(const Map::Id& map_id);
    void Update(std::chrono::milliseconds tick);
    void SetLootGeneratorConfig(double period, double probability) noexcept;
    double GetLootPeriod() const noexcept;
    double GetLootProbability() const noexcept;
    const Sessions& GetSessions() const noexcept;
    std::chrono::milliseconds GetDogRetirementTime() const noexcept;
    void SetDogRetirementTime(std::chrono::milliseconds retirement_time) noexcept;

    std::shared_ptr<GameSession> FindSession(GameSession::Id id) const noexcept;

private:
    void AddSession(const GameSession& session);
    void AddFreeSession(const Map::Id& id);

private:
    using MapIdToIndex = std::unordered_map<Map::Id, size_t, Map::IdHasher>;
    using MapIdToSessionIndex = std::unordered_map<Map::Id, FullnessToSessionIndex, Map::IdHasher>;
    Maps maps_;
    MapIdToIndex id_to_map_index_;
    Sessions sessions_;
    IndexToSession id_to_session;

    MapIdToSessionIndex map_to_sessions_;
    double period_ = 0.0, probability_ = 0.0;
    std::chrono::milliseconds retirement_time_;
    uint64_t game_session_id_ = 0;
};

} // namespace model