#include "tag_invoke.h"

namespace model {
    using namespace boost::json;
    using namespace std::string_literals;

    void tag_invoke(value_from_tag, value& json_value, Road const& road) {
        object obj;
        obj[MapKey::start_x] = road.GetStart().x;
        obj[MapKey::start_y] = road.GetStart().y;
        if (road.IsHorizontal()) {
            obj[MapKey::end_x] = road.GetEnd().x;
        } else if(road.IsVertical()) {
            obj[MapKey::end_y] = road.GetEnd().y;
        }

        json_value = value_from(obj);
    }

    void tag_invoke(value_from_tag, value& json_value, Building const& building) {
        json_value = {{MapKey::pos_x,  building.GetBounds().position.x},
                      {MapKey::pos_y,  building.GetBounds().position.y},
                      {MapKey::width,  building.GetBounds().size.width},
                      {MapKey::height, building.GetBounds().size.height}};
    }

    void tag_invoke(value_from_tag, value& json_value, Office const& office) {
        json_value = {{MapKey::id,       *office.GetId()},
                      {MapKey::pos_x,    office.GetPosition().x}, {MapKey::pos_y, office.GetPosition().y},
                      {MapKey::offset_x, office.GetOffset().dx}, {MapKey::offset_y, office.GetOffset().dy}};
    }

    void tag_invoke(value_from_tag, value& json_value, Map const& map) {
        object obj;
        obj[MapKey::id] = *map.GetId();
        obj[MapKey::name] = map.GetName();

        array json_array;
        for (auto& road : map.GetRoads()) { // parse road
            json_array.push_back(value_from(road));
        }
        obj[MapKey::roads] = value_from(json_array);
        json_array.clear();

        for (auto& building : map.GetBuildings()) { // parse building
            json_array.push_back(value_from(building));
        }
        obj[MapKey::buildings] = value_from(json_array);
        json_array.clear();

        for (auto& office : map.GetOffices()) { // parse office
            json_array.push_back(value_from(office));
        }
        obj[MapKey::offices] = value_from(json_array);
        json_array.clear();

        json_value = obj;
    }


    Building tag_invoke(value_to_tag<Building>, value const& json_value) {
        if (json_value.as_object().size() != 4) {
            throw std::logic_error(error_wrong_amount_keys + "Building");
        }

        return Building(Rectangle{Point{extruct<Coord>(json_value, MapKey::pos_x),
                                        extruct<Coord>(json_value, MapKey::pos_y)},
                        Size{extruct<Dimension>(json_value, MapKey::width),
                             extruct<Dimension>(json_value, MapKey::height)}});
    }

    Office tag_invoke(value_to_tag<Office>, value const& json_value) {
        auto& office = json_value.as_object();

        return Office{Office::Id{extruct<std::string>(json_value, MapKey::id)},
                      Point{extruct<Coord>(json_value, MapKey::pos_x),
                            extruct<Coord>(json_value, MapKey::pos_y)},
                      Offset{extruct<Dimension>(json_value, MapKey::offset_x),
                            extruct<Dimension>(json_value, MapKey::offset_y)}};
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
            if (key == MapKey::start_x) {
                start.x = static_cast<Coord>(value.as_int64());
            } else if (key == MapKey::start_y) {
                start.y = static_cast<Coord>(value.as_int64());
            } else if (key == MapKey::end_x) {
                vertical = false;
                finish = static_cast<Coord>(value.as_int64());
            } else if (key == MapKey::end_y) {
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
        Map map(Map::Id(extruct<std::string>(json_value, MapKey::id)),
                extruct<std::string>(json_value, MapKey::name));

        for (auto& [key, value]: jmap) {
            if (key == MapKey::id || key == MapKey::name) {
                continue;
            } else if (key == MapKey::roads) {
                for (auto& jroad : value.as_array()) {
                    map.AddRoad(value_to<Road>(jroad));
                }
            } else if (key == MapKey::buildings) {
                for (auto jbuilding : value.as_array()) {
                    map.AddBuilding(value_to<Building>(jbuilding));
                }
            } else if (key == MapKey::offices) {
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
