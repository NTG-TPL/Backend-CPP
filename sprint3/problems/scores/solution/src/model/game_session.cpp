#include "game_session.h"

namespace model {

/**
* Получить индекс карты
* @return индекс карты
*/
const Map::Id& GameSession::GetMapId() const noexcept {
    return map_.GetId();
}

/**
* Получить ссылку на карту
* @return Ссылка на карту
*/
const Map& GameSession::GetMap() const noexcept{
    return map_;
}

/**
* Возвращает контейнер кладов
* @return контейнер кладов
*/
const GameSession::Loots& GameSession::GetLoots() const noexcept{
    return loots_;
}

/**
* Получить количество занятых мест
* @return Количество занятых мест
*/
size_t GameSession::GetActivityPlayers() const noexcept {
    return dogs_.size();
}

/**
* Получить ограничение количества мест в сесии
* @return Количество мест сесии
*/
size_t GameSession::GetLimitPlayers() const noexcept {
    return limit_;
}

/**
* Получить количество свободных мест в сесии
* @return Количество свободных мест
*/
size_t GameSession::AmountAvailableSeats() const noexcept {
    return (limit_ - dogs_.size());
}

/**
* Проверяет заполненность сессии.
* @return Возвращает true, если сессия переполнена, иначе - false.
*/
bool GameSession::IsFull() const noexcept {
    return (AmountAvailableSeats() == 0);
}

/**
* Ищет собаку по индексу
* @param id Индекс собаки
* @return Сырой указатель на собаку
*/
Dog* GameSession::FindDog(const Dog::Id& id){
    return dogs_.contains(id) ? &dogs_.at(id) : nullptr;
}

/**
* Добавляет собаку в сессию (если id собаки не уникален, или сессия переполнена, то возвращает false)
* @param dog ссылка на собаку
* @return сырой указатель на собаку
*/
Dog * GameSession::AddDog(const Dog& dog) {
    if(dogs_.size() < limit_ && !dogs_.contains(dog.GetId())){
        return &(dogs_.emplace(dog.GetId(), dog).first)->second;
    }
    return nullptr;
}

/**
* Удаляет собаку из сессии
* @param id индекс собаки
* @return количество удалённых объектов
*/
size_t GameSession::EraseDog(const Dog::Id& id){
    return dogs_.erase(id);
}

/**
* Получить контейнер, содержащий собак
* @return GameSession::Dogs
*/
const GameSession::Dogs& GameSession::GetDogs() const noexcept {
    return dogs_;
}

/**
* Генерирует позицию объекта на дороге
* @param enable true - включить генератор, false - возвращать всегда стартовую точку дороги
* @return Point2i - точка на дороге
*/
Point2d GameSession::GenerateNewPosition(bool enable) const{
    return DogDropOffGenerator::GenerateDogPosition(*this, enable);
}

/**
* Генерирует клад каждый tick.
* @param tick время (в миллисекундах)
* @param enable true - включить генератор, false - возвращать всегда стартовую точку дороги
*/
void GameSession::GenerateLoot(std::chrono::milliseconds tick, bool enable) {
    auto& loot_types = map_.GetLootTypes();
    if(loot_types.empty() || loots_.size() >= dogs_.size()){
        return;
    }

    const unsigned count = loot_generator_.Generate(tick, loots_.size(), dogs_.size());
    size_t loot_index = 0;
    if (loot_types.size() > 1){
        loot_index = GenerateInRange(0ul, loot_types.size() - 1);
    }

    for(size_t i = 0; i < count; ++i) {
        auto id = Loot::Id{loot_id_++};
        loots_.emplace(id, std::make_shared<Loot>(id,
                                                  map_.GetLootTypes().at(loot_index),
                                                  GenerateNewPosition(enable),
                                                  loot_index
                                                  ));
    }
}

/**
 * Определяет коллизию собаки с границами дороги
 * @param dog собака
 * @param current_position текущая позиция собаки
 * @param new_position потенциально новая позиция собаки
 */
void GameSession::DetectCollisionWithRoadBorders(Dog& dog, Point2d current_position, Point2d new_position) {
    std::optional<Road::RoadRectangle::Borders> union_borders;
    for(const auto & road : map_.GetRoads()) {
        if(road.Contains(current_position)) {
            auto borders = road.GetBorders();
            if(!union_borders.has_value()){
                union_borders = borders;
                continue;
            }
            // Объединение областей дорог
            union_borders->min_x = std::min(union_borders->min_x , borders.min_x);
            union_borders->max_x = std::max(union_borders->max_x , borders.max_x);
            union_borders->min_y = std::min(union_borders->min_y , borders.min_y);
            union_borders->max_y = std::max(union_borders->max_y , borders.max_y);
        }
    }
    // Пересечение траектории с допустимой областью
    if(!union_borders->Contains(new_position)){
        new_position.y = std::max(new_position.y, union_borders->min_y);
        new_position.y = std::min(new_position.y, union_borders->max_y);
        new_position.x = std::max(new_position.x, union_borders->min_x);
        new_position.x = std::min(new_position.x, union_borders->max_x);
        dog.Stand();
    }
    dog.SetPosition(new_position);
}

/**
 * Отвечает за получение добычи и её возврат на базу. Разрешает временные конфликты
 * (Например, кто первым из собак взял добычу или отнёс её на базу)
 * @param item_gatherer Сборщик предметов
 * @param gather_by_index Словарь собак через счётчик
 */
void GameSession::CollectingAndReturningLoot(ItemGatherer& item_gatherer, std::unordered_map<size_t, model::Dog&> gather_by_index) {
    std::unordered_map<size_t, std::shared_ptr<Loot>> loot_numb_to_item;
    size_t loot_idx = 0;
    for (auto& [id, loot]: loots_) {
        loot_numb_to_item[loot_idx++] = loot;
        item_gatherer.Add(Object(loot->GetPosition(), loot->GetWidth()));
    }
    for (const auto& office : map_.GetOffices()) {
        item_gatherer.Add(Object(office.GetPosition(), office.GetWidth()));
    }
    auto gathering_events = FindGatherEvents(item_gatherer);
    for (const auto& gatherring_event : gathering_events) {
        // Если это офис
        if (gatherring_event.item_id >= loots_.size()) {
            gather_by_index.at(gatherring_event.gatherer_id).BagClear(); // Возвращает все предметы на базу
            continue;
        }

        // Если это клад
        if (loot_numb_to_item.contains(gatherring_event.item_id)) {
            // Если сумка полна, то добыча не подбирается
            if (gather_by_index.at(gatherring_event.gatherer_id).GetBag().size() >= map_.GetBagCapacity()){
                continue;
            }
            if(loot_numb_to_item.contains(gatherring_event.item_id)){
                auto& loot = loot_numb_to_item.at(gatherring_event.item_id);
                gather_by_index.at(gatherring_event.gatherer_id).MoveLootToBag(loots_.at(loot->GetId()));
                loots_.erase(loot->GetId());
                loot_numb_to_item.erase(gatherring_event.item_id);
            }
        }
    }
}

/**
* Обновляет состояние сессии на tick секунд
* @param tick время (в миллисекундах)
*/
void GameSession::Update(std::chrono::milliseconds tick){
    ItemGatherer item_gatherer;
    std::unordered_map<size_t, model::Dog&> gather_by_index;
    size_t dog_index = 0;

    for (auto& [id, dog] : dogs_) {
        if(dog.GetDirection() == Movement::STOP || (dog.GetSpeed().dx == 0 && dog.GetSpeed().dy == 0)){
            continue;
        }

        auto position = dog.GetPosition();
        auto speed = dog.GetSpeed();

        static constexpr const std::int32_t divider = 1000;
        double delta_seconds = static_cast<double>(tick.count())/divider;
        Point2d new_position = {position.x + speed.dx * delta_seconds,
                                position.y + speed.dy * delta_seconds};

        DetectCollisionWithRoadBorders(dog, position, new_position);

        // Добавление собак в сборщик предметов для дальнейшего разрешения временных конфликтов
        gather_by_index.emplace(dog_index++, dog);
        item_gatherer.Add(Gatherer{position, new_position, dog.GetWidth()});
    }
    CollectingAndReturningLoot(item_gatherer, gather_by_index);
    GenerateLoot(tick);
}


/**
 * Генерирует позицию объекта на дороге
 * @param session сессия
 * @param enable true - включить генератор, false - возвращать всегда стартовую точку дороги
 * @return Point2i на дороге
 */
    Point2d DogDropOffGenerator::GenerateDogPosition(const GameSession& session, bool enable){
        auto& map = session.GetMap();
        auto& roads = map.GetRoads();
        if (roads.empty()) {
            throw std::logic_error("На карте " + *map.GetId() + " нет дорог");
        }
        if (!enable) {
            return static_cast<Point2d>(roads.at(0).GetStart());
        }

        size_t num_road = 0;
        // Случайно выбираем дорогу
        if (roads.size() > 1) {
            num_road = GenerateInRange(0, static_cast<DimensionInt>(roads.size()) - 1);
        }

        Road road = roads.at(num_road);

        Point2d start = static_cast<Point2d>(road.GetStart()),
                end = static_cast<Point2d>(road.GetEnd());
        Point2d result = start;

        static auto floor_2 = [](DimensionDouble number){
            return std::floor(number * 100)/100.0; // округляет до двух знаков после запятой
        };

        DimensionDouble width_2 = road.GetWidth() / 2;
        {
            std::uniform_real_distribution<DimensionDouble> dist(-width_2, width_2);
            width_2 = GenerateInRange(-width_2, width_2);
            width_2 = floor_2(width_2);
        }

        if (road.IsHorizontal()) {
            result.x = GenerateInRange(start.x, end.x);
            result.y += width_2;
        } else if(road.IsVertical()) {
            result.y = GenerateInRange(start.y, end.y);
            result.x += width_2;
        }
        result.x = floor_2(result.x);
        result.y = floor_2(result.y);

        return result;
    }

} // namespace model