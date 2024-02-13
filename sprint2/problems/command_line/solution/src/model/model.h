#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <memory>
#include <optional>
#include <map>
#include <random>
#include <iostream>
#include <utility>
#include <chrono>

#include "../util/tagged.h"

namespace model {
using namespace std::string_view_literals;

using DimensionInt = int;
using DimensionDouble = double;
using CoordInt = DimensionInt;
using CoordDouble = DimensionDouble;

template <typename TypeNumber>
struct Point {
    TypeNumber x, y;

    explicit operator Point<CoordDouble>() const {
        return {static_cast<CoordDouble>(x), static_cast<CoordDouble>(y)};
    }

    explicit operator Point<DimensionInt>() const {
        return {static_cast<DimensionInt>(x), static_cast<DimensionInt>(y)};
    }


};

template <typename TypeNumber>
struct Velocity {
    TypeNumber dx, dy;
};

using Point2i = Point<DimensionInt>;
using Point2d = Point<CoordDouble>;
using Velocity2d = Velocity<CoordDouble>;

/**
* Структура для хранения движения собак
*/
struct Movement {
    using Function = Velocity2d (*const)(DimensionDouble);

    Movement() = delete;
    constexpr const static std::string_view UP    = "U"sv;
    constexpr const static std::string_view DOWN  = "D"sv;
    constexpr const static std::string_view LEFT  = "L"sv;
    constexpr const static std::string_view RIGHT = "R"sv;
    constexpr const static std::string_view STOP  = ""sv;

    static Velocity2d MoveUp(DimensionDouble speed){ return {0.0, -speed}; }
    static Velocity2d MoveDown(DimensionDouble speed){ return {0.0, speed}; }
    static Velocity2d MoveLeft(DimensionDouble speed){ return {-speed, 0.0}; }
    static Velocity2d MoveRight(DimensionDouble speed){ return {speed, 0.0}; }
    static Velocity2d Stand(DimensionDouble _ = 0.0){ return {0.0, 0.0};}

    static std::unordered_map<std::string_view, Function> InitMovement() {
        return {{Movement::UP,    MoveUp},
                {Movement::DOWN,  MoveDown},
                {Movement::LEFT,  MoveLeft},
                {Movement::RIGHT, MoveRight},
                {Movement::STOP,  Stand}};
    }

    static inline const std::unordered_map<std::string_view, Function> MOVEMENT = InitMovement();
};

struct Object {
    Point2d position_;
};

struct Size {
    DimensionInt width, height;
};

struct Rectangle {
    Point2i position;
    Size size;
};

struct Offset {
    DimensionInt dx, dy;
};

class Road {
    struct HorizontalTag {
        HorizontalTag() = default;
    };

    struct VerticalTag {
        VerticalTag() = default;
    };

public:
    class RoadRectangle {
    public:
        struct Borders {
            CoordDouble min_x, max_x,
                        min_y, max_y;

            inline bool Contains(Point2d point){
                return  point.x >= min_x && point.x <= max_x &&
                        point.y >= min_y && point.y <= max_y;
            }
        };

        RoadRectangle (Point2d start, Point2d end, DimensionDouble width) :
                borders_{std::min(start.x, end.x) - width / 2,
                         std::max(start.x, end.x) + width / 2,
                         std::min(start.y, end.y) - width / 2,
                         std::max(start.y, end.y) + width / 2},
                width_(width) {}

        [[nodiscard]] Borders GetBorders() const {
            return borders_;
        }

        [[nodiscard]] DimensionDouble GetWidth() const{
            return width_;
        }

    private:
        Borders borders_;
        DimensionDouble width_;
    };

    constexpr static HorizontalTag HORIZONTAL{};
    constexpr static VerticalTag VERTICAL{};

    Road(HorizontalTag, Point2i start, CoordInt end_x, DimensionDouble width = 0.8) noexcept:
            start_{start}, end_{end_x, start.y},
            road_rectangle_(static_cast<Point2d>(start_), static_cast<Point2d>(end_), width) {}
    Road(VerticalTag, Point2i start, CoordInt end_y, DimensionDouble width = 0.8) noexcept:
            start_{start}, end_{start.x, end_y},
            road_rectangle_(static_cast<Point2d>(start_), static_cast<Point2d>(end_), width) {}

    [[nodiscard]] bool IsHorizontal() const noexcept;
    [[nodiscard]] bool IsVertical() const noexcept;
    [[nodiscard]] Point2i GetStart() const noexcept;
    [[nodiscard]] Point2i GetEnd() const noexcept;
    [[nodiscard]] Road::RoadRectangle::Borders GetBorders() const noexcept;
    [[nodiscard]] DimensionDouble GetWidth() const noexcept;
    [[nodiscard]] bool Contains(Point2d point) const noexcept;

private:
    Point2i start_;
    Point2i end_;
    RoadRectangle road_rectangle_;
};

class Building {
public:
    explicit Building(Rectangle bounds) noexcept: bounds_{bounds} {}

    [[nodiscard]] const Rectangle& GetBounds() const noexcept;

private:
    Rectangle bounds_;
};

class Office {
public:
    using Id = util::Tagged<std::string, Office>;

    Office(Id id, Point2i position, const Offset& offset) noexcept: id_{std::move(id)}, position_{position}, offset_{offset} {}

    [[nodiscard]] const Id& GetId() const noexcept;
    [[nodiscard]] Point2i GetPosition() const noexcept;
    [[nodiscard]] Offset GetOffset() const noexcept;

private:
    Id id_;
    Point2i position_;
    Offset offset_;
};

class Map {
public:
    using Id = util::Tagged<std::string, Map>;
    using Roads = std::vector<Road>;
    using Buildings = std::vector<Building>;
    using Offices = std::vector<Office>;

    Map(Id id, std::string name, DimensionDouble dog_speed) noexcept:
        id_(std::move(id)), name_(std::move(name)), dog_speed_(dog_speed) {}
    const Id& GetId() const noexcept;
    const std::string& GetName() const noexcept;
    const Buildings& GetBuildings() const noexcept;
    const Roads& GetRoads() const noexcept;
    const Offices& GetOffices() const noexcept;
    DimensionDouble GetDogSpeed() const noexcept;
    void AddRoad(const Road& road);
    void AddBuilding(const Building& building);
    void AddOffice(const Office& office);

private:
    using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

    Id id_;
    std::string name_;
    DimensionDouble dog_speed_;

    Roads roads_;
    Buildings buildings_;

    OfficeIdToIndex warehouse_id_to_index_;
    Offices offices_;
};

class Dog : private Object {
public:
    using Id = util::Tagged<size_t, Dog>;

    Dog(Id id, std::string name, Point2d position = {0.0, 0.0}):
        Object(position), id_(id), name_(std::move(name)) {}
    Dog() = delete;
    ~Dog() = default;

    [[nodiscard]] Id GetId() const noexcept;
    [[nodiscard]] const std::string& GetName() const noexcept;
    void Move(std::string_view dir, DimensionDouble speed);
    void SetPosition(Point2d new_point);
    void Stand();
    [[nodiscard]] Point2d GetPosition() const noexcept;
    [[nodiscard]] Velocity2d GetSpeed() const noexcept;
    [[nodiscard]] std::string GetDirection() const noexcept;

private:
    Id id_;
    std::string name_;
    std::string dir_{Movement::UP}; // Поменять на string_view ?
    Velocity2d speed_{Movement::Stand()};
};

class GameSession {
public:
    using DogIdHasher = util::TaggedHasher<Dog::Id>;
    using Dogs = std::unordered_map<Dog::Id, Dog, DogIdHasher>;

    explicit GameSession(const Map& map): map_(map), limit_(100) {}
    GameSession(const Map& map, size_t limit): map_(map), limit_(limit) {}
    const Map::Id& GetMapId() const noexcept;
    const Map& GetMap() const noexcept;

    size_t GetActivityPlayers() const noexcept;
    size_t GetLimitPlayers() const noexcept;
    size_t AmountAvailableSeats() const noexcept;
    bool IsFull() const noexcept;
    Dog* FindDog(const Dog::Id& id);
    Dog * AddDog(const Dog& dog);
    size_t EraseDog(const Dog::Id& id);
    [[nodiscard]] const Dogs& GetDogs() const noexcept;
    Point2d GenerateNewPosition(bool enable = true) const;
    void Update(std::chrono::milliseconds tick);

private:
    const Map& map_;
    size_t limit_;
    Dogs dogs_;
};

class DropOffGenerator {
public:
    static Point2d GeneratePosition(const GameSession& session, bool enable = true);
};

class Game {
public:
    using Maps = std::vector<Map>;
    using Sessions = std::vector<std::shared_ptr<GameSession>>;
    using FullnessToSessionIndex = std::map<size_t, size_t, std::greater<>>;

    void AddMap(const Map& map);
    const Maps& GetMaps() const noexcept;
    const Map* FindMap(const Map::Id& id) const noexcept;
    std::shared_ptr<GameSession> UpdateSessionFullness(size_t index, const GameSession& session);
    std::pair<size_t, std::shared_ptr<GameSession>>  CreateFreeSession(const Map::Id& map_id);
    std::optional<std::pair<size_t, std::shared_ptr<GameSession>>> ExtractFreeSession(const Map::Id& map_id);
    void Update(std::chrono::milliseconds tick);

private:
    using MapIdHasher = util::TaggedHasher<Map::Id>;
    using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;
    using MapIdToSessionIndex = std::unordered_map<Map::Id, FullnessToSessionIndex, MapIdHasher>;

    Maps maps_;
    MapIdToIndex id_to_map_index_;
    Sessions sessions_;
    MapIdToSessionIndex map_to_sessions_;
};
}  // namespace model
