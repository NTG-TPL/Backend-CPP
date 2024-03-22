#pragma once

#include <boost/json.hpp>
#include <string>
#include <optional>

#include "../model/model.h"

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

struct DefaultValues {
    DefaultValues() = delete;
    static const inline model::DimensionDouble DOG_SPEED          = 1.0;
    static const inline model::DimensionDouble MS_IN_SECOND       = 1000.0;
    static const inline size_t BAG_CAPACITY                       = 3;
    static const inline std::chrono::milliseconds RETIREMENT_TIME = std::chrono::milliseconds{60000};
};

struct MapKey {
    MapKey() = delete;
    static constexpr boost::json::string_view MAPS                 = "maps";
    static constexpr boost::json::string_view DEFAULT_DOG_SPEED    = "defaultDogSpeed";
    static constexpr boost::json::string_view DOG_SPEED            = "dogSpeed";
    static constexpr boost::json::string_view ID                   = "id";
    static constexpr boost::json::string_view NAME                 = "name";
    static constexpr boost::json::string_view START_X              = "x0";
    static constexpr boost::json::string_view START_Y              = "y0";
    static constexpr boost::json::string_view END_X                = "x1";
    static constexpr boost::json::string_view END_Y                = "y1";
    static constexpr boost::json::string_view POS_X                = "x";
    static constexpr boost::json::string_view POS_Y                = "y";
    static constexpr boost::json::string_view HEIGHT               = "h";
    static constexpr boost::json::string_view WIDTH                = "w";
    static constexpr boost::json::string_view OFFSET_X             = "offsetX";
    static constexpr boost::json::string_view OFFSET_Y             = "offsetY";
    static constexpr boost::json::string_view ROADS                = "roads";
    static constexpr boost::json::string_view BUILDINGS            = "buildings";
    static constexpr boost::json::string_view OFFICES              = "offices";
};

struct LootKey{
    LootKey() = delete;
    static constexpr boost::json::string_view ID                   = "id";
    static constexpr boost::json::string_view LOOT_TYPES           = "lootTypes";
    static constexpr boost::json::string_view LOOT_CONFIG          = "lootGeneratorConfig";
    static constexpr boost::json::string_view PERIOD               = "period";
    static constexpr boost::json::string_view PROBABILITY          = "probability";
    static constexpr boost::json::string_view NAME                 = "name";
    static constexpr boost::json::string_view FILE                 = "file";
    static constexpr boost::json::string_view TYPE                 = "type";
    static constexpr boost::json::string_view POSITION             = "pos";
    static constexpr boost::json::string_view ROTATION             = "rotation";
    static constexpr boost::json::string_view COLOR                = "color";
    static constexpr boost::json::string_view SCALE                = "scale";
    static constexpr boost::json::string_view LOST                 = "lostObjects";
    static constexpr boost::json::string_view DEFAULT_BAG_CAPACITY = "defaultBagCapacity";
    static constexpr boost::json::string_view BAG_CAPACITY         = "bagCapacity";
    static constexpr boost::json::string_view BAG                  = "bag";
    static constexpr boost::json::string_view VALUE                = "value";
    static constexpr boost::json::string_view SCORE                = "score";
};

struct UserKey {
    UserKey() = delete;
    static constexpr boost::json::string_view ID                   = "id";
    static constexpr boost::json::string_view NAME                 = "name";
    static constexpr boost::json::string_view USER_NAME            = "userName";
    static constexpr boost::json::string_view USER_TOKEN           = "authToken";
    static constexpr boost::json::string_view MAP_ID               = "mapId";
    static constexpr boost::json::string_view PLAYER_ID            = "playerId";
    static constexpr boost::json::string_view PLAYERS              = "players";
    static constexpr boost::json::string_view POSITION             = "pos";
    static constexpr boost::json::string_view SPEED                = "speed";
    static constexpr boost::json::string_view DIRECTION            = "dir";
    static constexpr boost::json::string_view MOVE                 = "move";
    static constexpr boost::json::string_view TIME_INTERVAL        = "timeDelta";
    static constexpr boost::json::string_view RETIREMENT_TIME      = "dogRetirementTime";
};

struct DataBaseKey {
    DataBaseKey() = delete;
    static constexpr boost::json::string_view NAME                 = "name";
    static constexpr boost::json::string_view SCORE                = "score";
    static constexpr boost::json::string_view PLAY_TIME            = "playTime";
};

}  // namespace detail