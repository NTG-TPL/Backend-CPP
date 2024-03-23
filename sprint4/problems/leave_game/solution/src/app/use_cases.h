#pragma once

#include <string>
#include <vector>
#include <optional>

#include "../db/player_repository.h"

namespace app {

    class UseCases {
    public:
        virtual void SaveRetiredPlayers(const data_base::domain::RetiredPlayers& records) = 0;
        virtual data_base::domain::RetiredPlayers GetRetiredPlayers(size_t offset, size_t limit) = 0;

    protected:
        ~UseCases() = default;
    };
}
