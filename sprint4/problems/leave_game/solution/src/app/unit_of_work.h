#pragma once

#include <memory>

#include "../db/player_repository.h"

namespace app {
    struct UnitOfWork {
        virtual void Commit() = 0;
        virtual data_base::domain::RetiredPlayerRepository& RetiredPlayers() = 0;
        virtual ~UnitOfWork() = default;
    };

    using UnitOfWorkHolder = std::unique_ptr<UnitOfWork>;

    class UnitOfWorkFactory {
    public:
        virtual UnitOfWorkHolder CreateUnitOfWork() = 0;
    protected:
        ~UnitOfWorkFactory() = default;
    };
}