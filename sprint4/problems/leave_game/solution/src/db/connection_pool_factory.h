#pragma once

#include <pqxx/pqxx>
#include <pqxx/connection.hxx>

#include "connection_pool.h"
#include "player_repository.h"
#include "prepared.h"

namespace data_base {

using namespace std::literals;

/**
 * Фабрика соединений к базе данных
 */
class ConnectionFactory {
public:
    explicit ConnectionFactory(const std::string& db_url) : db_url_(db_url) {
        using key = domain::RetiredPlayersKeys;
        pqxx::connection conn(db_url);
        pqxx::work work(conn);

        work.exec("CREATE TABLE IF NOT EXISTS "+key::RETIRED_PLAYERS+
                  " ("+key::ID+" UUID PRIMARY KEY,"+
                  key::NAME+" VARCHAR(100) NOT NULL,"+
                  key::SCORE+" INTEGER NOT NULL CONSTRAINT score_non_negative CHECK (score >= 0),"+
                  key::PLAY_TIME+ " INTEGER NOT NULL CONSTRAINT play_time_non_negative CHECK (play_time_ms >= 0)); "

                  "CREATE INDEX IF NOT EXISTS retired_players_index ON "+key::RETIRED_PLAYERS+
                  " ("+key ::SCORE+" DESC, "+key::PLAY_TIME+", "+key::NAME+");"
                  );

        work.commit();
    }
    ~ConnectionFactory() = default;

    ConnectionPool::ConnectionPtr operator()() {
        using key = domain::RetiredPlayersKeys;
        auto connection_ptr = std::make_shared<ConnectionPool::ConnectionPtr::element_type>(db_url_);
        connection_ptr->prepare(Prepared::save_retired_player,
                                "INSERT INTO " + key::RETIRED_PLAYERS + " (" + key::ALL + ") VALUES ($1, $2, $3, $4);");
        return connection_ptr;
    }

private:
    std::string db_url_;
};

}  // namespace dbconn