#include "tag_invoke_utils.h"

namespace model {
    void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, const Road& road);
    void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, const Building & building);
    void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, const Office& office);
    void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, const Loot& loot);
    void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, const LootType& loot_type);
    void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, const Map& map);
    void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, const Game::Maps& maps);
    void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, const Dog::Bag& bag);
    void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, const Dog& dog);

    Road tag_invoke(boost::json::value_to_tag<Road>, const boost::json::value& json_value);
    Building tag_invoke(boost::json::value_to_tag<Building>, const boost::json::value& json_value);
    Office tag_invoke(boost::json::value_to_tag<Office>, const boost::json::value& json_value);
    LootType tag_invoke(boost::json::value_to_tag<LootType>, const boost::json::value& json_value);
    Map tag_invoke(boost::json::value_to_tag<Map>, const boost::json::value& json_value);
    Game tag_invoke(boost::json::value_to_tag<Game>, const boost::json::value& json_value);
} // namespace model