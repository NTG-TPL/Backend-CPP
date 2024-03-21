#pragma once
#include <string>
#include <vector>
#include "../util/tagged_uuid.h"
#include "../app/players.h"

namespace data_base::domain {
    using namespace std::string_literals;
    struct RetiredPlayersKeys {
        static inline std::string ID                  = "id"s;
        static inline std::string NAME                = "name"s;
        static inline std::string SCORE               = "score"s;
        static inline std::string PLAY_TIME           = "play_time_ms"s;
        static inline std::string RETIRED_PLAYERS     = "retired_players"s;
        static inline std::string ALL                 = "id, name, score, play_time_ms"s;
    };

    using milliseconds = std::chrono::milliseconds;
    class RetiredPlayer {
    public:
        RetiredPlayer(app::Player::Id id, std::string name, std::int32_t score, milliseconds play_time):
                id_(std::move(id)), name_(std::move(name)),
                score_(score), play_time_(play_time) {
        }
        [[nodiscard]] const app::Player::Id& GetPlayerId() const { return id_; }
        [[nodiscard]] const std::string& GetName() const { return name_;}
        [[nodiscard]] std::int32_t GetScore() const { return score_; }
        [[nodiscard]] const milliseconds& GetPlayTime() const { return play_time_; }

    private:
        app::Player::Id id_;
        std::string name_;
        std::int32_t score_;
        milliseconds play_time_;
    };

    using RetiredPlayers = std::vector<RetiredPlayer>;

    class RetiredPlayerRepository {
    public:
        virtual void Save(const RetiredPlayer& dog) = 0;
        virtual void SaveAll(const RetiredPlayers& records) = 0;
        [[nodiscard]] virtual RetiredPlayers Get(size_t offset, size_t limit) const = 0;

    protected:
        virtual ~RetiredPlayerRepository() = default;
    };

}  // namespace domain