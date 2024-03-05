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
    dir_ =  dir != Movement::STOP ? dir : dir_;
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
std::string Dog::GetDirection() const noexcept {
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
 * Очистить сумку с кладом
 */
void Dog::BagClear() noexcept {
    bag_.clear();
}

} // namespace model