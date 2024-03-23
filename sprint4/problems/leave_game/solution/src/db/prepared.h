#pragma once
#include <string>

#include "player_repository.h"

namespace data_base {
using namespace std::string_literals;

struct Prepared {
    static inline std::string save_retired_player = "save_retired_player"s;
};

struct PreparedQueryText {
    static inline auto get_retired_players = [](size_t offset, size_t limit){
        using key = domain::RetiredPlayersKeys;
        return "SELECT "+ key::ALL +" FROM "+ key::RETIRED_PLAYERS +
        " ORDER BY "+key ::SCORE+" DESC, "+key::PLAY_TIME+" ASC, "+key::NAME+
        " ASC LIMIT "+std::to_string(limit) +" OFFSET "+std::to_string(offset)+";";
    };
};

} // namespace data_base