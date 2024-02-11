#ifndef GAME_SERVER_TAG_INVOKE_H
#define GAME_SERVER_TAG_INVOKE_H

#include <boost/json.hpp>
#include <string>

#include "../model/model.h"
#include "../app/players.h"

namespace model {

    namespace {
        const std::string error_wrong_amount_keys = "logic_error: The wrong number of keys in json";
        const  std::string error_unknown_key = "logic_error: Unknown key";

        template <typename T>
        std::enable_if_t<std::is_integral_v<T>, T>
        extruct(const boost::json::value& json_value, const boost::json::string_view& key) {
            return static_cast<T>(json_value.as_object().at(key).as_int64());
        }

        template <typename T>
        std::enable_if_t<std::is_same_v<T, std::string>, std::string>
        extruct(const boost::json::value& jv, const boost::json::string_view& key) {
            return static_cast<std::string>(jv.as_object().at(key).as_string());
        }

    }  // namespace

    void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, Road const& road);
    void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, Building const& building);
    void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, Office const& office);
    void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, Map const& map);
    void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, Game::Maps const& maps);
    void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, Dog const& dog);

    Road tag_invoke(boost::json::value_to_tag<Road>, boost::json::value const& json_value);
    Building tag_invoke(boost::json::value_to_tag<Building>, boost::json::value const& json_value);
    Office tag_invoke(boost::json::value_to_tag<Office>, boost::json::value const& json_value);
    Map tag_invoke(boost::json::value_to_tag<Map>, boost::json::value const& json_value);
    Game tag_invoke(boost::json::value_to_tag<Game>, boost::json::value const& json_value);

    struct MapKey {
        constexpr const static boost::json::string_view MAPS              = "maps";
        constexpr const static boost::json::string_view DEFAULT_DOG_SPEED = "defaultDogSpeed";
        constexpr const static boost::json::string_view DOG_SPEED         = "dogSpeed";
        constexpr const static boost::json::string_view ID                = "id";
        constexpr const static boost::json::string_view NAME              = "name";
        constexpr const static boost::json::string_view START_X           = "x0";
        constexpr const static boost::json::string_view START_Y           = "y0";
        constexpr const static boost::json::string_view END_X             = "x1";
        constexpr const static boost::json::string_view END_Y             = "y1";
        constexpr const static boost::json::string_view POS_X             = "x";
        constexpr const static boost::json::string_view POS_Y             = "y";
        constexpr const static boost::json::string_view HEIGHT            = "h";
        constexpr const static boost::json::string_view WIDTH             = "w";
        constexpr const static boost::json::string_view OFFSET_X          = "offsetX";
        constexpr const static boost::json::string_view OFFSET_Y          = "offsetY";
        constexpr const static boost::json::string_view ROADS             = "roads";
        constexpr const static boost::json::string_view BUILDINGS         = "buildings";
        constexpr const static boost::json::string_view OFFICES           = "offices";
    };

    struct UserKey {
        constexpr const static boost::json::string_view USER_NAME         = "userName";
        constexpr const static boost::json::string_view USER_TOKEN        = "authToken";
        constexpr const static boost::json::string_view MAP_ID            = "mapId";
        constexpr const static boost::json::string_view PLAYER_ID         = "playerId";
        constexpr const static boost::json::string_view PLAYERS           = "players";
        constexpr const static boost::json::string_view POSITION          = "pos";
        constexpr const static boost::json::string_view SPEED             = "speed";
        constexpr const static boost::json::string_view DIRECTION         = "dir";
        constexpr const static boost::json::string_view MOVE              = "move";
        constexpr const static boost::json::string_view TIME_INTERVAL     = "timeDelta";
    };
} // namespace model
#endif //GAME_SERVER_TAG_INVOKE_H
