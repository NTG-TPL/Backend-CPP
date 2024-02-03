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
        object obj;
        obj[MapKey::ID] = *map.GetId();
        obj[MapKey::NAME] = map.GetName();

        array json_array;
        for (auto& road : map.GetRoads()) { // parse road
            json_array.push_back(value_from(road));
        }
        obj[MapKey::ROADS] = value_from(json_array);
        json_array.clear();

        for (auto& building : map.GetBuildings()) { // parse building
            json_array.push_back(value_from(building));
        }
        obj[MapKey::BUILDINGS] = value_from(json_array);
        json_array.clear();

        for (auto& office : map.GetOffices()) { // parse office
            json_array.push_back(value_from(office));
        }
        obj[MapKey::OFFICES] = value_from(json_array);
        json_array.clear();

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

    Building tag_invoke(value_to_tag<Building>, value const& json_value) {
        if (json_value.as_object().size() != 4) {
            throw std::logic_error(error_wrong_amount_keys + "Building");
        }

        return Building(Rectangle{Point{extruct<Coord>(json_value, MapKey::POS_X),
                                        extruct<Coord>(json_value, MapKey::POS_Y)},
                        Size{extruct<Dimension>(json_value, MapKey::WIDTH),
                             extruct<Dimension>(json_value, MapKey::HEIGHT)}});
    }

    Office tag_invoke(value_to_tag<Office>, value const& json_value) {
        auto& office = json_value.as_object();

        return Office{Office::Id{extruct<std::string>(json_value, MapKey::ID)},
                      Point{extruct<Coord>(json_value, MapKey::POS_X),
                            extruct<Coord>(json_value, MapKey::POS_Y)},
                      Offset{extruct<Dimension>(json_value, MapKey::OFFSET_X),
                            extruct<Dimension>(json_value, MapKey::OFFSET_Y)}};
    }

    Road tag_invoke(value_to_tag<Road>, value const& json_value) {
        auto& road = json_value.as_object();
        if (road.size() != 3) {
            throw std::logic_error(error_wrong_amount_keys + "Road"s);
        }

        Point start{};
        Coord finish{};
        bool vertical = true;
        for (auto& [key, value] : road) {
            if (key == MapKey::START_X) {
                start.x = static_cast<Coord>(value.as_int64());
            } else if (key == MapKey::START_Y) {
                start.y = static_cast<Coord>(value.as_int64());
            } else if (key == MapKey::END_X) {
                vertical = false;
                finish = static_cast<Coord>(value.as_int64());
            } else if (key == MapKey::END_Y) {
                vertical = true;
                finish = static_cast<Coord>(value.as_int64());
            } else {
                throw std::logic_error(error_unknown_key + "in the json Road"s);
            }
        }
        return vertical ? Road{Road::VERTICAL, start, finish} : Road{Road::HORIZONTAL, start, finish};
    }

    Map tag_invoke(value_to_tag<Map>, value const& json_value) {
        auto& jmap = json_value.as_object();
        Map map(Map::Id(extruct<std::string>(json_value, MapKey::ID)),
                extruct<std::string>(json_value, MapKey::NAME));

        for (auto& [key, value]: jmap) {
            if (key == MapKey::ID || key == MapKey::NAME) {
                continue;
            } else if (key == MapKey::ROADS) {
                for (auto& jroad : value.as_array()) {
                    map.AddRoad(value_to<Road>(jroad));
                }
            } else if (key == MapKey::BUILDINGS) {
                for (auto jbuilding : value.as_array()) {
                    map.AddBuilding(value_to<Building>(jbuilding));
                }
            } else if (key == MapKey::OFFICES) {
                for (auto joffice : value.as_array()) {
                    map.AddOffice(value_to<Office>(joffice));
                }
            } else {
                throw std::logic_error(error_unknown_key + "in the Map: "s + key.to_string());
            }
        }
        return map;
    }
}  // namespace model
