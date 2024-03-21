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
* @param direction Направление
* @param speed Скорость
*/
void Dog::Move(std::string_view direction, DimensionDouble speed) {
    using namespace std::string_literals;
    auto it = Movement::MOVEMENT_VIEW.find(direction);
    direction_ = (direction != Movement::STOP) ? *it : direction_;
    speed_ = Movement::MOVEMENT.at(direction)(speed);
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
 * Положить клад в рюкзак
 * @param loot клад
 */
void Dog::PutToBag(const FoundObject& loot) {
    bag_.push_back(loot);
}

void Dog::SetSpeed(Velocity2d speed) noexcept {
    speed_ = speed;
}

void Dog::SetDirection(std::string_view direction) {
    auto it = Movement::MOVEMENT_VIEW.find(direction);
    direction_ = (direction != Movement::STOP) ? *it : direction_;
}

/**
 * Присвоить счёт
 */
void Dog::AddScore(std::int32_t score) noexcept {
    score_ += score;
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
    return direction_;
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
[[nodiscard]] std::int32_t Dog::GetScore() const noexcept {
    return score_;
}

/**
 * Очистить сумку с кладом
 */
void Dog::BagClear() noexcept {
    for (auto& loot: bag_) {
        score_ += loot.value;
    }
    bag_.clear();
}

/**
 * Обновить время
 * @param delta_time интервал времени
 */
void Dog::UpdateLifeTimer(std::chrono::milliseconds delta_time) {
    using namespace std::literals::chrono_literals;
    life_time_ += delta_time;
    stay_time_ = (speed_ == Movement::Stand()) ? (stay_time_ += delta_time) : 0ms;
}

/**
 * Вернуть время без движения
 * @return время собаки без движения
 */
std::chrono::milliseconds Dog::GetStayTime() const {
    return stay_time_;
}

/**
 * Вернуть время жизни собаки
 * @return время жизни собаки
 */
std::chrono::milliseconds Dog::GetLifeTime() const {
    return life_time_;
}


} // namespace model