#ifndef GAME_SERVER_TAG_INVOKE_H
#define GAME_SERVER_TAG_INVOKE_H

#include <boost/json.hpp>
#include <string>

#include "model.h"

namespace model {

    namespace {
        const static std::string error_wrong_amount_keys = "logic_error: The wrong number of keys in json";
        const static  std::string error_unknown_key = "logic_error: Unknown key";

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

    Road tag_invoke(boost::json::value_to_tag<Road>, boost::json::value const& json_value);
    Building tag_invoke(boost::json::value_to_tag<Building>, boost::json::value const& json_value);
    Office tag_invoke(boost::json::value_to_tag<Office>, boost::json::value const& json_value);
    Map tag_invoke(boost::json::value_to_tag<Map>, boost::json::value const& json_value);

    struct MapKey {
        constexpr const static boost::json::string_view id = "id";
        constexpr const static boost::json::string_view name = "name";
        constexpr const static boost::json::string_view start_x = "x0";
        constexpr const static boost::json::string_view start_y = "y0";
        constexpr const static boost::json::string_view end_x = "x1";
        constexpr const static boost::json::string_view end_y = "y1";
        constexpr const static boost::json::string_view pos_x = "x";
        constexpr const static boost::json::string_view pos_y = "y";
        constexpr const static boost::json::string_view height = "h";
        constexpr const static boost::json::string_view width = "w";
        constexpr const static boost::json::string_view offset_x = "offsetX";
        constexpr const static boost::json::string_view offset_y = "offsetY";
        constexpr const static boost::json::string_view roads = "roads";
        constexpr const static boost::json::string_view buildings = "buildings";
        constexpr const static boost::json::string_view offices = "offices";
    };
};  // namespace model
#endif //GAME_SERVER_TAG_INVOKE_H
