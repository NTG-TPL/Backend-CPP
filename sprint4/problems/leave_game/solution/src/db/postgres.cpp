#include "postgres.h"

#include <pqxx/zview.hxx>
#include <pqxx/pqxx>
#include <iostream>

namespace data_base::postgres {

    using namespace std::literals;
    using pqxx::operator"" _zv;

    void RetiredPlayerRepositoryImpl::Save(const domain::RetiredPlayer& retired_player) {
        work_.exec_params(
R"(
INSERT INTO retired_players (id, name, score, play_time_ms) VALUES ($1, $2, $3, $4);
)"_zv,
                retired_player.GetPlayerId().ToString(),
                retired_player.GetName(),
                retired_player.GetScore(),
                retired_player.GetPlayTime().count()
        );
    }

    void RetiredPlayerRepositoryImpl::SaveAll(const domain::RetiredPlayers& players) {
        // TODO:: Исправить
        for (const auto& player : players) {
            Save(player);
        }
    }

    domain::RetiredPlayers RetiredPlayerRepositoryImpl::Get(size_t offset, size_t limit) const {
        domain::RetiredPlayers retired_players;
        auto query_text = "SELECT id, name, score, play_time_ms FROM retired_players "
                          "ORDER BY score DESC, play_time_ms ASC, name ASC "
                          "LIMIT " + std::to_string(limit) + " OFFSET " + std::to_string(offset) + ";";
        auto res =  work_.query<std::string, std::string,  size_t, int64_t>(query_text);
        for (const auto& [id, name, score, play_time] : res) {
            retired_players.emplace_back(app::Player::Id::FromString(id),
                                         name, score,
                                         std::chrono::milliseconds{play_time});
        }
        return retired_players;
    }

    Database::Database(size_t capacity, const std::string& db_url) :
            connection_pool_(capacity, [url = db_url]() { return std::make_shared<pqxx::connection>(url);}) {
        auto conn = connection_pool_.GetConnection();
        pqxx::work work_{*conn};
        work_.exec(R"(
        CREATE TABLE IF NOT EXISTS retired_players (
            id UUID PRIMARY KEY,
            name VARCHAR(100) NOT NULL,
            score INTEGER NOT NULL CONSTRAINT score_non_negative CHECK (score >= 0),
            play_time_ms INTEGER NOT NULL CONSTRAINT play_time_non_negative CHECK (play_time_ms >= 0)
        );
        CREATE INDEX IF NOT EXISTS retired_players_index ON retired_players (score DESC, play_time_ms, name);
    )");
        work_.commit();
    }

}  // namespace postgres