#include "postgres.h"

#include <pqxx/zview.hxx>
#include <pqxx/pqxx>

#include "connection_pool_factory.h"

namespace data_base::postgres {

    using namespace std::literals;
    using pqxx::operator"" _zv;

    void RetiredPlayerRepositoryImpl::Save(const domain::RetiredPlayer& retired_player) {
        work_.exec_prepared(Prepared::save_retired_player,
                            retired_player.GetPlayerId().ToString(),
                            retired_player.GetName(),
                            retired_player.GetScore(),
                            retired_player.GetPlayTime().count());
    }

    void RetiredPlayerRepositoryImpl::SaveAll(const domain::RetiredPlayers& players) {
        for (const auto& player : players) {
            Save(player);
        }
    }

    domain::RetiredPlayers RetiredPlayerRepositoryImpl::Get(size_t offset, size_t limit) const {
        domain::RetiredPlayers retired_players;
        auto query_text = PreparedQueryText::get_retired_players(offset, limit);
        auto result =  work_.query<std::string, std::string, size_t, int64_t>(query_text);
        for (const auto& [id, name, score, play_time] : result) {
            retired_players.emplace_back(app::Player::Id::FromString(id), name, score, std::chrono::milliseconds{play_time});
        }
        return retired_players;
    }

    Database::Database(size_t capacity, const std::string& db_url) :
            connection_pool_(capacity, ConnectionFactory(db_url)) {
    }
}  // namespace postgres