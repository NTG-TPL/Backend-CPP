#pragma once
#include <boost/serialization/vector.hpp>

#include "../model/dog.h"
#include "../model/geom.h"
#include "../model_serialize/ser_geom.h"

namespace model {

template <typename Archive>
void serialize(Archive& ar, FoundObject& obj, [[maybe_unused]] const unsigned version) {
    ar&(*obj.id);
    ar&(obj.type);
    ar&(obj.value);
}

} // namespace model

namespace serialization {
using namespace model;

class DogRepr {
public:
    DogRepr() = default;

    explicit DogRepr(const model::Dog& dog):
            id_(dog.GetId()),
            name_(dog.GetName()),
            pos_(dog.GetPosition()),
            speed_(dog.GetSpeed()),
            direction_(dog.GetDirection()),
            score_(dog.GetScore()),
            bag_content_(dog.GetBag()) {
    }

    [[nodiscard]] model::Dog Restore() const {
        Dog dog{id_, name_, pos_};
        dog.SetSpeed(speed_);
        dog.SetDirection(direction_);
        dog.AddScore(score_);
        for (const auto& item : bag_content_) {
            dog.PutToBag(item);
        }
        return dog;
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar&* id_;
        ar& name_;
        ar& pos_;
        ar& speed_;
        ar& direction_;
        ar& score_;
        ar& bag_content_;
    }

private:
    model::Dog::Id id_ = model::Dog::Id{0u};
    std::string name_;
    Point2d pos_{};
    Velocity2d speed_{};
    std::string direction_{model::Movement::UP};
    std::int32_t score_ = 0;
    Dog::Bag bag_content_;
};

} // namespace serialization