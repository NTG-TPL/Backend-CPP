#pragma once

#include "tag_invoke_utils.h"
#include "../db/player_repository.h"

namespace data_base::domain {

void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, const data_base::domain::RetiredPlayer& retired_player);
void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, const data_base::domain::RetiredPlayers& retired_players);

} // namespace data_base