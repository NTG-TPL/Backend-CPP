#pragma once
#include <pqxx/connection>
#include <pqxx/transaction>

#include <memory>

#include "player_repository.h"
#include "connection_pool.h"
#include "../app/unit_of_work.h"
//#include "unit_of_work_impl.h"

namespace data_base::postgres {

class RetiredPlayerRepositoryImpl : public domain::RetiredPlayerRepository {
public:
    explicit RetiredPlayerRepositoryImpl(pqxx::work& work) : work_(work)  {
    }

    void Save(const domain::RetiredPlayer& retired_player) override;
    void SaveAll(const domain::RetiredPlayers& players) override;
    [[nodiscard]] domain::RetiredPlayers Get(size_t offset, size_t limit) const override;

private:
    pqxx::work& work_;
};

class UnitOfWorkImpl : public app::UnitOfWork {
public:
    explicit UnitOfWorkImpl(ConnectionPool::ConnectionWrapper&& connection) :
            connection_(std::move(connection)),
            work_(*connection_),
            player_records_(work_) {}

    void Commit() override {
        work_.commit();
    }

    domain::RetiredPlayerRepository& RetiredPlayers() override {
        return player_records_;
    }

private:
    ConnectionPool::ConnectionWrapper connection_;
    pqxx::work work_;
    RetiredPlayerRepositoryImpl player_records_;
};

class UnitOfWorkFactoryImpl : public app::UnitOfWorkFactory {
public:
    explicit UnitOfWorkFactoryImpl(ConnectionPool& connection_pool) :
            connection_pool_(connection_pool) {}

    app::UnitOfWorkHolder CreateUnitOfWork() override {
        return std::make_unique<UnitOfWorkImpl>(connection_pool_.GetConnection());
    }

private:
    ConnectionPool& connection_pool_;
};

struct DataBaseConfig {
    size_t conn_pool_size = 1;
    std::string db_url;
};

struct DBConstants {
    DBConstants() = delete;
    static const inline size_t LIMIT_RECORDS = 100;
};

class Database {
public:
    explicit Database(size_t capacity, const std::string& db_url);

    app::UnitOfWorkFactory& GetUnitOfWorkFactory() { return unit_factory_; }
private:
    ConnectionPool connection_pool_;
    UnitOfWorkFactoryImpl unit_factory_{connection_pool_};
};

}  // namespace postgres