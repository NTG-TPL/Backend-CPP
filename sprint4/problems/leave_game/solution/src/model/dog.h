#pragma once
#include <memory>
#include <chrono>
#include <numeric>

#include "geom.h"
#include "../util/tagged.h"
#include "movement.h"
#include "loot.h"

namespace model {

struct FoundObject {
    using Id = util::Tagged<size_t , FoundObject>;
    Id id{0};
    size_t type{0};
    std::int32_t value{0};

    [[nodiscard]] auto operator<=>(const FoundObject&) const = default;
};

class Dog : public Object {
public:
    using Id = util::Tagged<uint64_t, Dog>;
    using IdHasher = util::TaggedHasher<Dog::Id>;
    using Bag = std::vector<FoundObject>;

    Dog(Id id, std::string name, Point2d position = {0.0, 0.0}) noexcept:
            Object(position, ObjectWidth::DOG_WIDTH), id_(id), name_(std::move(name)),
            direction_(Movement::UP), speed_(Movement::Stand()), score_(0),
            stay_time_(0), life_time_(0){
    }

    Dog() = delete;
    ~Dog() override = default;

    [[nodiscard]] Id GetId() const noexcept;
    [[nodiscard]] const std::string& GetName() const noexcept;
    void Move(std::string_view direction, DimensionDouble speed);
    void SetPosition(Point2d new_point);
    void Stand();
    void PutToBag(const FoundObject& loot);

    void SetSpeed(Velocity2d speed) noexcept;
    void SetDirection(std::string_view  direction);
    void AddScore(std::int32_t score) noexcept;
    [[nodiscard]] Velocity2d GetSpeed() const noexcept;
    [[nodiscard]] std::string_view GetDirection() const noexcept;
    [[nodiscard]] const Dog::Bag& GetBag() const noexcept;
    [[nodiscard]] std::int32_t GetScore() const noexcept;
    void BagClear() noexcept;
    void UpdateLifeTimer(std::chrono::milliseconds delta_time);
    std::chrono::milliseconds GetStayTime() const;
    std::chrono::milliseconds GetLifeTime() const;

private:
    using milliseconds = std::chrono::milliseconds;
    Id id_;
    std::string name_;
    std::string_view direction_;
    Velocity2d speed_;
    Bag bag_;
    std::int32_t score_;
    milliseconds stay_time_;
    milliseconds life_time_;
};
} // namespace model