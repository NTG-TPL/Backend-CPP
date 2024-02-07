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

    operator Point<CoordDouble>() const {
        return {static_cast<CoordDouble>(x), static_cast<CoordDouble>(y)};
    }

    operator Point<DimensionInt>() const {
        return {static_cast<DimensionInt>(x), static_cast<DimensionInt>(y)};
    }
};

template <typename TypeNumber>
struct Speed {
    TypeNumber dx, dy;
};

using Point2i = Point<DimensionInt>;
using Point2d = Point<CoordDouble>;
using Speed2d = Speed<CoordDouble>;

/**
* Структура-хранилка для направления собак
*/
struct Direction {
    Direction() = delete;
    constexpr const static std::string_view UP    = "U"sv;
    constexpr const static std::string_view DOWN  = "D"sv;
    constexpr const static std::string_view LEFT  = "L"sv;
    constexpr const static std::string_view RIGHT = "R"sv;
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
    constexpr static HorizontalTag HORIZONTAL{};
    constexpr static VerticalTag VERTICAL{};

    Road(HorizontalTag, Point2i start, CoordInt end_x) noexcept: start_{start}, end_{end_x, start.y} {}
    Road(VerticalTag, Point2i start, CoordInt end_y) noexcept: start_{start}, end_{start.x, end_y} {}

    [[nodiscard]] bool IsHorizontal() const noexcept;
    [[nodiscard]] bool IsVertical() const noexcept;
    [[nodiscard]] Point2i GetStart() const noexcept;
    [[nodiscard]] Point2i GetEnd() const noexcept;

private:
    Point2i start_;
    Point2i end_;
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

    Map(Id id, std::string name) noexcept: id_(std::move(id)), name_(std::move(name)) {}
    const Id& GetId() const noexcept;
    const std::string& GetName() const noexcept;
    const Buildings& GetBuildings() const noexcept;
    const Roads& GetRoads() const noexcept;
    const Offices& GetOffices() const noexcept;
    void AddRoad(const Road& road);
    void AddBuilding(const Building& building);
    void AddOffice(const Office& office);

private:
    using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

    Id id_;
    std::string name_;
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
    void SetPosition(const Point2d& position);
    [[nodiscard]] const Point2d& GetPosition() const noexcept;
    [[nodiscard]] const Speed2d & GetSpeed() const noexcept;
    [[nodiscard]] std::string_view GetDirection() const noexcept;

private:
    Id id_;
    std::string name_;
    std::string_view dir_{Direction::UP};
    Speed2d speed_{0.0, 0.0};
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
    const Dog* FindDog(const Dog::Id& id) const noexcept;
    Dog * AddDog(const Dog& dog);
    size_t EraseDog(const Dog::Id& id);
    [[nodiscard]] const Dogs& GetDogs() const noexcept;
private:
    const Map& map_;
    size_t limit_;
    Dogs dogs_;
};

class DropOffGenerator {
public:
    Point2i GeneratePosition(const GameSession& session, bool enable = true);
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
    Point2i GenerateNewPosition(const model::GameSession& session, bool enable = true);

private:
    using MapIdHasher = util::TaggedHasher<Map::Id>;
    using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;
    using MapIdToSessionIndex = std::unordered_map<Map::Id, FullnessToSessionIndex, MapIdHasher>;

    Maps maps_;
    MapIdToIndex id_to_map_index_;
    Sessions sessions_;
    MapIdToSessionIndex map_to_sessions_;
    DropOffGenerator position_generator;
};

}  // namespace model
