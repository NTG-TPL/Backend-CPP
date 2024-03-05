#pragma once

#include <boost/json.hpp>
#include <string>

#include "../model/model.h"
#include "../app/players.h"

namespace model {

namespace detail {
    const std::string error_wrong_amount_keys = "logic_error: The wrong number of keys in json";
    const  std::string error_unknown_key = "logic_error: Unknown key";

    template <typename T>
    T extruct(const boost::json::value& json_value, boost::json::string_view key);

    template <typename T>
    std::optional<T> try_extruct(const boost::json::value& json_value, boost::json::string_view key);

    template <typename T>
    void try_assign(boost::json::object& json_object, const std::optional<T>& value, boost::json::string_view key){
        if(value.has_value()){
            json_object[key] = value.value();
        }
    }


}  // namespace detail

void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, Road const& road);
void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, Building const& building);
void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, Office const& office);
void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, Loot const& loot);
void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, LootType const& loot_type);
void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, Map const& map);
void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, Game::Maps const& maps);
void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, Dog::Bag const& bag);
void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, Dog const& dog);

Road tag_invoke(boost::json::value_to_tag<Road>, boost::json::value const& json_value);
Building tag_invoke(boost::json::value_to_tag<Building>, boost::json::value const& json_value);
Office tag_invoke(boost::json::value_to_tag<Office>, boost::json::value const& json_value);
LootType tag_invoke(boost::json::value_to_tag<LootType>, boost::json::value const& json_value);
Map tag_invoke(boost::json::value_to_tag<Map>, boost::json::value const& json_value);
Game tag_invoke(boost::json::value_to_tag<Game>, boost::json::value const& json_value);

struct MapKey {
    MapKey() = delete;
    constexpr const static boost::json::string_view MAPS                 = "maps";
    constexpr const static boost::json::string_view DEFAULT_DOG_SPEED    = "defaultDogSpeed";
    constexpr const static boost::json::string_view DOG_SPEED            = "dogSpeed";
    constexpr const static boost::json::string_view ID                   = "id";
    constexpr const static boost::json::string_view NAME                 = "name";
    constexpr const static boost::json::string_view START_X              = "x0";
    constexpr const static boost::json::string_view START_Y              = "y0";
    constexpr const static boost::json::string_view END_X                = "x1";
    constexpr const static boost::json::string_view END_Y                = "y1";
    constexpr const static boost::json::string_view POS_X                = "x";
    constexpr const static boost::json::string_view POS_Y                = "y";
    constexpr const static boost::json::string_view HEIGHT               = "h";
    constexpr const static boost::json::string_view WIDTH                = "w";
    constexpr const static boost::json::string_view OFFSET_X             = "offsetX";
    constexpr const static boost::json::string_view OFFSET_Y             = "offsetY";
    constexpr const static boost::json::string_view ROADS                = "roads";
    constexpr const static boost::json::string_view BUILDINGS            = "buildings";
    constexpr const static boost::json::string_view OFFICES              = "offices";
};

struct LootKey{
    LootKey() = delete;
    constexpr const static boost::json::string_view ID                   = "id";
    constexpr const static boost::json::string_view LOOT_TYPES           = "lootTypes";
    constexpr const static boost::json::string_view LOOT_CONFIG          = "lootGeneratorConfig";
    constexpr const static boost::json::string_view PERIOD               = "period";
    constexpr const static boost::json::string_view PROBABILITY          = "probability";
    constexpr const static boost::json::string_view NAME                 = "name";
    constexpr const static boost::json::string_view FILE                 = "file";
    constexpr const static boost::json::string_view TYPE                 = "type";
    constexpr const static boost::json::string_view POSITION             = "pos";
    constexpr const static boost::json::string_view ROTATION             = "rotation";
    constexpr const static boost::json::string_view COLOR                = "color";
    constexpr const static boost::json::string_view SCALE                = "scale";
    constexpr const static boost::json::string_view LOST                 = "lostObjects";
    constexpr const static boost::json::string_view DEFAULT_BAG_CAPACITY = "defaultBagCapacity";
    constexpr const static boost::json::string_view BAG_CAPACITY         = "bagCapacity";
    constexpr const static boost::json::string_view BAG                  = "bag";
    constexpr const static boost::json::string_view VALUE                = "value";
};

struct UserKey {
    UserKey() = delete;
    constexpr const static boost::json::string_view ID                   = "id";
    constexpr const static boost::json::string_view NAME                 = "name";
    constexpr const static boost::json::string_view USER_NAME            = "userName";
    constexpr const static boost::json::string_view USER_TOKEN           = "authToken";
    constexpr const static boost::json::string_view MAP_ID               = "mapId";
    constexpr const static boost::json::string_view PLAYER_ID            = "playerId";
    constexpr const static boost::json::string_view PLAYERS              = "players";
    constexpr const static boost::json::string_view POSITION             = "pos";
    constexpr const static boost::json::string_view SPEED                = "speed";
    constexpr const static boost::json::string_view DIRECTION            = "dir";
    constexpr const static boost::json::string_view MOVE                 = "move";
    constexpr const static boost::json::string_view TIME_INTERVAL        = "timeDelta";
};
} // namespace model