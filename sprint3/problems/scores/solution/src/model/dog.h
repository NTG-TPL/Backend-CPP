#pragma once
#include <memory>

#include "geom.h"
#include "../tagged.h"
#include "movement.h"
#include "loot.h"

namespace model {

class Dog : public Object {
public:
    using Id = util::Tagged<size_t, Dog>;
    using IdHasher = util::TaggedHasher<Dog::Id>;
    using Bag = std::vector<std::shared_ptr<Loot>>;

    Dog(Id id, std::string name, Point2d position = {0.0, 0.0}) noexcept:
            Object(position, ObjectWidth::DOG_WIDTH), id_(id), name_(std::move(name)) {
    }

    Dog() = delete;
    ~Dog() override = default;

    [[nodiscard]] Id GetId() const noexcept;
    [[nodiscard]] const std::string& GetName() const noexcept;
    void Move(std::string_view dir, DimensionDouble speed);
    void SetPosition(Point2d new_point);
    void Stand();
    void MoveLootToBag(std::shared_ptr<Loot> loot);
    [[nodiscard]] Velocity2d GetSpeed() const noexcept;
    [[nodiscard]] std::string GetDirection() const noexcept;
    [[nodiscard]] const Dog::Bag& GetBag() const noexcept;
    [[nodiscard]] std::int64_t GetScore() const noexcept;
    void BagClear() noexcept;

private:
    Id id_;
    std::string name_;
    std::string dir_{Movement::UP}; //FIXME:: Поменять на string_view ?
    Velocity2d speed_{Movement::Stand()};
    Bag bag_;
    std::int64_t score_;
};
} // namespace model