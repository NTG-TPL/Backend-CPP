#include "map.h"

namespace model {

/**
* Получить индекс карты
* @return индекс карты
*/
const Map::Id& Map::GetId() const noexcept {
    return id_;
}

/**
* Получить название карты
* @return Название карты
*/
const std::string& Map::GetName() const noexcept {
    return name_;
}
/**
* Получить контейнер, содеражайщий здания на карте
* @return Контейнер, содеражайщий здания
*/
const Map::Buildings& Map::GetBuildings() const noexcept {
    return buildings_;
}

/**
* Получить контейнер, содеражайщий маршруты на карте
* @return Контейнер, содеражайщий маршруты
*/
const Map::Roads& Map::GetRoads() const noexcept {
    return roads_;
}

/**
* Получить контейнер, содеражайщий офисы на карте
* @return Контейнер, содеражайщий офисы
*/
const Map::Offices& Map::GetOffices() const noexcept {
    return offices_;
}

/**
* Получить скорость собак на карте
* @return DimensionDouble {double} - Скорость собак
*/
DimensionDouble Map::GetDogSpeed() const noexcept {
    return dog_speed_;
}

/**
* Возвращает ссылку на контейнер с типами клада
* @return типы клада
*/
const Map::LootTypes& Map::GetLootTypes() const noexcept{
    return loot_types_;
}

/**
* Возвращает допустимую вместимость сумки на текущей карте
* @return Вместимость сумки
*/
size_t Map::GetBagCapacity() const noexcept {
    return bag_capacity_;
}

/**
* Добавить маршрут
* @param road Ссылка на маршрут
*/
void Map::AddRoad(const Road& road) {
    roads_.emplace_back(road);
}

/**
* Добавить здание
* @param building Ссылка на здание
*/
void Map::AddBuilding(const Building& building) {
    buildings_.emplace_back(building);
}

/**
* Добавить оффис
* @param office Ссылка на офис
*/
void Map::AddOffice(const Office& office) {
    if (warehouse_id_to_index_.contains(office.GetId())) {
        throw std::invalid_argument("Duplicate warehouse");
    }

    const size_t index = offices_.size();
    Office& o = offices_.emplace_back(office);
    try {
        warehouse_id_to_index_.emplace(o.GetId(), index);
    } catch (...) {
        // Удаляем офис из вектора, если не удалось вставить в unordered_map
        offices_.pop_back();
        throw;
    }
}

/**
* Добавляет тип клада
* @param loot_type тип клада
*/
void Map::AddLootType(const LootType& loot_type){
    loot_types_.push_back(loot_type);
}

/**
 * Возвращает максимальное количество игроков на карту
 * @return максимальное количество игроков
 */
size_t Map::GetLimitPlayers() const noexcept {
    return limit_players_;
}

} // namespace model