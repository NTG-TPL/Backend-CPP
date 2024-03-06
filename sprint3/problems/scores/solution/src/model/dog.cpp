#include "dog.h"

namespace model {

/**
* Получить индекс собаки
* @return Индекс собаки
*/
Dog::Id Dog::GetId() const noexcept {
    return id_;
}

/**
* Получить имя собаки
* @return Имя собаки
*/
const std::string& Dog::GetName() const noexcept {
    return name_;
}

/**
* Двигает собаку в направлении dir со скоростью speed
* @param dir Направление
* @param speed Скорость
*/
void Dog::Move(std::string_view dir, DimensionDouble speed) {
    using namespace std::string_literals;
    auto it = Movement::MOVEMENT_VIEW.find(dir);
    if(it == Movement::MOVEMENT_VIEW.end()){
        throw std::domain_error("Movement in the direction of <"s + std::string{dir} +"> is not supported"s);
    }
    dir_ =  (dir != Movement::STOP) ? *it : dir_;
    speed_ = Movement::MOVEMENT.at(dir)(speed);
}


/**
* Присваивает позицию собаке
* @param new_point Позиция
*/
void Dog::SetPosition(Point2d new_point){
    position_ = new_point;
}

/**
* Останавливает собаку
*/
void Dog::Stand(){
    speed_ = Movement::Stand();
}

/**
 *
 */
void Dog::MoveLootToBag(std::shared_ptr<Loot> loot) {
    bag_.push_back(std::move(loot));
}

/**
* Получить скорость собаки
* @return Скорость собаки
*/
Velocity2d Dog::GetSpeed() const noexcept {
    return speed_;
}

/**
* Получить направление собаки
* @return Направление собаки
*/
std::string_view Dog::GetDirection() const noexcept {
    return dir_;
}

/**
 * Получить сумку с кладом
 * @return сумка с кладом
 */
const Dog::Bag& Dog::GetBag() const noexcept {
    return bag_;
}

/**
 * Получить счёт игрока
 * @return счёт
 */
[[nodiscard]] std::int64_t Dog::GetScore() const noexcept {
    return score_;
}

/**
 * Очистить сумку с кладом
 */
void Dog::BagClear() noexcept {
    for (auto& loot: bag_) {
        auto value = loot->GetValue();
        if(value.has_value()){
            score_ += value.value();
        }
    }
    bag_.clear();
}

} // namespace model