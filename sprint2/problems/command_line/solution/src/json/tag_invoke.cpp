#include "tag_invoke.h"

namespace model {
using namespace boost::json;
using namespace std::string_literals;

void tag_invoke(value_from_tag, value& json_value, Road const& road) {
    object obj;
    obj[MapKey::START_X] = road.GetStart().x;
    obj[MapKey::START_Y] = road.GetStart().y;
    if (road.IsHorizontal()) {
        obj[MapKey::END_X] = road.GetEnd().x;
    } else if(road.IsVertical()) {
        obj[MapKey::END_Y] = road.GetEnd().y;
    }

    json_value = value_from(obj);
}

void tag_invoke(value_from_tag, value& json_value, Building const& building) {
    json_value = {{MapKey::POS_X,  building.GetBounds().position.x},
                  {MapKey::POS_Y,  building.GetBounds().position.y},
                  {MapKey::WIDTH,  building.GetBounds().size.width},
                  {MapKey::HEIGHT, building.GetBounds().size.height}};
}

void tag_invoke(value_from_tag, value& json_value, Office const& office) {
    json_value = {{MapKey::ID,       *office.GetId()},
                  {MapKey::POS_X,    office.GetPosition().x}, {MapKey::POS_Y, office.GetPosition().y},
                  {MapKey::OFFSET_X, office.GetOffset().dx}, {MapKey::OFFSET_Y, office.GetOffset().dy}};
}

void tag_invoke(value_from_tag, value& json_value, Map const& map) {
    auto get_json_array = [](const auto& container){
        boost::json::array json_array;
        std::transform(container.cbegin(), container.cend(), std::back_inserter(json_array), [](auto& obj) {
            return value_from(obj);
        });
        return json_array;
    };

    object obj;
    obj[MapKey::ID] = *map.GetId();
    obj[MapKey::NAME] = map.GetName();

    obj[MapKey::ROADS] = value_from(get_json_array(map.GetRoads()));
    obj[MapKey::BUILDINGS] = value_from(get_json_array(map.GetBuildings()));
    obj[MapKey::OFFICES] = value_from(get_json_array(map.GetOffices()));
    json_value = obj;
}

void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, Game::Maps const& maps){
    array arr;
    for (const auto& map : maps) {
        value value = {{MapKey::ID, *map.GetId()}, {MapKey::NAME, map.GetName()}};
        arr.push_back(value);
    }
    json_value = arr;
}

void tag_invoke(boost::json::value_from_tag, boost::json::value& json_value, Dog const& dog){
    json_value = {{UserKey::POSITION, {dog.GetPosition().x, dog.GetPosition().y}},
                  {UserKey::SPEED, {dog.GetSpeed().dx, dog.GetSpeed().dy}},
                  {UserKey::DIRECTION, dog.GetDirection()}};
}

Road tag_invoke(value_to_tag<Road>, value const& json_value) {
    auto& road = json_value.as_object();
    if (road.size() != 3) {
        throw std::logic_error(detail::error_wrong_amount_keys + "Road"s);
    }

    Point2i start{};
    CoordInt finish{};
    bool vertical = true;
    for (auto& [key, value] : road) {
        if (key == MapKey::START_X) {
            start.x = static_cast<CoordInt>(value.as_int64());
        } else if (key == MapKey::START_Y) {
            start.y = static_cast<CoordInt>(value.as_int64());
        } else if (key == MapKey::END_X) {
            vertical = false;
            finish = static_cast<CoordInt>(value.as_int64());
        } else if (key == MapKey::END_Y) {
            vertical = true;
            finish = static_cast<CoordInt>(value.as_int64());
        } else {
            throw std::logic_error(detail::error_unknown_key + "in the json Road"s);
        }
    }
    return vertical ? Road{Road::VERTICAL, start, finish} : Road{Road::HORIZONTAL, start, finish};
}

Building tag_invoke(value_to_tag<Building>, value const& json_value) {
    if (json_value.as_object().size() != 4) {
        throw std::logic_error(detail::error_wrong_amount_keys + "Building");
    }

    return Building(Rectangle{Point2i{detail::extruct<CoordInt>(json_value, MapKey::POS_X),
                                      detail::extruct<CoordInt>(json_value, MapKey::POS_Y)},
                              Size{detail::extruct<DimensionInt>(json_value, MapKey::WIDTH),
                                   detail::extruct<DimensionInt>(json_value, MapKey::HEIGHT)}});
}

Office tag_invoke(value_to_tag<Office>, value const& json_value) {
    auto& office = json_value.as_object();

    return Office{Office::Id{detail::extruct<std::string>(json_value, MapKey::ID)},
                  Point2i{detail::extruct<CoordInt>(json_value, MapKey::POS_X),
                          detail::extruct<CoordInt>(json_value, MapKey::POS_Y)},
                  Offset{detail::extruct<DimensionInt>(json_value, MapKey::OFFSET_X),
                         detail::extruct<DimensionInt>(json_value, MapKey::OFFSET_Y)}};
}

Map tag_invoke(boost::json::value_to_tag<Map>, boost::json::value const& json_value){
    auto& jmap = json_value.as_object();

    DimensionDouble default_dog_speed = 1.0;
    if(jmap.contains(MapKey::DEFAULT_DOG_SPEED)){
        default_dog_speed = jmap.at(MapKey::DEFAULT_DOG_SPEED).as_double();
    }
    DimensionDouble dog_speed = default_dog_speed;
    if(jmap.contains(MapKey::DOG_SPEED)){
        dog_speed = jmap.at(MapKey::DOG_SPEED).as_double();
    }

    Map map(Map::Id(detail::extruct<std::string>(json_value, MapKey::ID)),
            detail::extruct<std::string>(json_value, MapKey::NAME), dog_speed);

    for (auto& [key, value]: jmap) {
        if (key == MapKey::ID || key == MapKey::NAME) {
            continue;
        } else if (key == MapKey::ROADS) {
            for (auto& jroad : value.as_array()) {
                map.AddRoad(value_to<Road>(jroad));
            }
        } else if (key == MapKey::BUILDINGS) {
            for (const auto& jbuilding : value.as_array()) {
                map.AddBuilding(value_to<Building>(jbuilding));
            }
        } else if (key == MapKey::OFFICES) {
            for (const auto& joffice : value.as_array()) {
                map.AddOffice(value_to<Office>(joffice));
            }
        }
    }
    return map;
}

Game tag_invoke(boost::json::value_to_tag<Game>, boost::json::value const& json_value){
    auto& jmap = json_value.as_object();

    model::Game game;
    DimensionDouble default_dog_speed = 1.0;
    if(jmap.contains(MapKey::DEFAULT_DOG_SPEED)){
        default_dog_speed = jmap.at(MapKey::DEFAULT_DOG_SPEED).as_double();
    }

    auto maps = jmap.at(MapKey::MAPS).as_array();
    for (auto& map: maps) {
        map.as_object()[MapKey::DEFAULT_DOG_SPEED] = default_dog_speed;
        game.AddMap(boost::json::value_to<model::Map>(map));
    }
    return game;
}
}  // namespace model
