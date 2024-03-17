#include "tag_invoke_utils.h"

namespace detail {
    template <>
    int extruct<int>(const boost::json::value& json_value, boost::json::string_view key) {
        return static_cast<int>(json_value.as_object().at(key).as_int64());
    }

    template <>
    double extruct<double>(const boost::json::value& json_value, boost::json::string_view key) {
        return json_value.as_object().at(key).as_double();
    }

    template <>
    std::string extruct<std::string>(const boost::json::value& json_value, boost::json::string_view key) {
        return static_cast<std::string>(json_value.as_object().at(key).as_string());
    }

    template <>
    std::optional<int> try_extruct<int>(const boost::json::value& json_value, boost::json::string_view key) {
        if(!json_value.as_object().contains(key)){
            return std::nullopt;
        }
        return extruct<int>(json_value, key);
    }

    template <>
    std::optional<double> try_extruct<double>(const boost::json::value& json_value, boost::json::string_view key) {
        if(!json_value.as_object().contains(key)){
            return std::nullopt;
        }
        return extruct<double>(json_value, key);
    }

    template <>
    std::optional<std::string> try_extruct<std::string>(const boost::json::value& json_value, boost::json::string_view key) {
        if(!json_value.as_object().contains(key)){
            return std::nullopt;
        }
        return extruct<std::string>(json_value, key);
    }
}