#include "tag_invoke_db.h"

namespace data_base::domain {
using namespace detail;
void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, const data_base::domain::RetiredPlayer& retired_player) {
    json_value = {{DataBaseKey::NAME, retired_player.GetName()},
                  {DataBaseKey::SCORE, retired_player.GetScore()},
                  {DataBaseKey::PLAY_TIME, static_cast<double>(retired_player.GetPlayTime().count())/DefaultValues::MS_IN_SECOND}};
}

void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, const data_base::domain::RetiredPlayers& retired_players) {
    boost::json::array json_array;
    std::transform(retired_players.cbegin(), retired_players.cend(), std::back_inserter(json_array), [](auto& el) {
        return boost::json::value_from(el);
    });
    json_value = std::move(json_array);
}

} // namespace data_base