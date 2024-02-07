#include "model.h"

namespace model {
using namespace std::literals;

/**
 * Проверка на горизонтальный маршрут
 * @return если маршрут горизонтальный, то возвращает true, иначе - false.
 */
bool Road::IsHorizontal() const noexcept {
    return start_.y == end_.y;
}

/**
 * Проверка на вертикальный маршрут
 * @return если маршрут вертикальный, то возвращает true, иначе - false.
 */
bool Road::IsVertical() const noexcept {
    return start_.x == end_.x;
}

/**
 * Получить начальную точку маршрута
 * @return Point2i start;
 */
Point2i Road::GetStart() const noexcept {
    return start_;
}

/**
 * Получить конечную точку маршрута
 * @return Point2i end;
 */
Point2i Road::GetEnd() const noexcept {
    return end_;
}

/**
 * Получить границы Building
 * @return Rectangle
 */
const Rectangle&  Building::GetBounds() const noexcept {
    return bounds_;
}

/**
 * Получить индекс Office
 * @return индекс
 */
const Office::Id& Office::GetId() const noexcept {
    return id_;
}

/**
 * Получить позицию объекта Office
 * @return позиция
 */
Point2i Office::GetPosition() const noexcept {
    return position_;
}

/**
 * Получить смещение объекта Office
 * @return смещение
 */
Offset Office::GetOffset() const noexcept {
    return offset_;
}

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
 * двигает собаку в направлении dir со скоростью speed
 * @param dir Направление
 * @param speed Скорость
 */
void Dog::Move(std::string_view dir, DimensionDouble speed) {
    speed_ = Direction::DIRECTION.at(dir)(speed);
    dir_ = dir;
}

/**
 * Получить позицию собаки
 * @return Позиция собаки
 */
const Point2d& Dog::GetPosition() const noexcept {
    return position_;
}

/**
 * Получить скорость собаки
 * @return Скорость собаки
 */
const Velocity2d& Dog::GetSpeed() const noexcept {
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
 * @param session сессия
 * @param enable true - включить генератор, false - возвращать всегда стартовую точку дороги
 * @return Point2i на дороге
 */
Point2i DropOffGenerator::GeneratePosition(const GameSession& session, bool enable){
    auto& map = session.GetMap();
    auto& roads = map.GetRoads();
    if (roads.empty()) {
            throw std::logic_error("На карте " + *map.GetId() + " нет дорог");
    }
    if (!enable) {
        auto start = roads.at(0).GetStart();
        return {start.x, start.y};
    }

    std::random_device random_device;
    std::mt19937 generator_{[&req = random_device] {
        std::uniform_int_distribution<std::mt19937::result_type> dist;
        return dist(req);
    }()};

    size_t num_road = 0;
    // Случайно выбираем дорогу
    if (roads.size() > 1) {
        num_road = generator_() % roads.size();
    }

    Road road = roads.at(num_road);

    Point2i start = road.GetStart(), end = road.GetEnd();
    Point2i result = start;

    auto get_range = [&gen = generator_](int32_t start, int32_t end){
        return (static_cast<int32_t>(gen()%(end - start + 1)) + start);
    };

    if (road.IsHorizontal()) {
        result.x = get_range(start.x, end.x);
    } else if(road.IsVertical()) {
        result.y = get_range(start.y, end.y);
    }

    return result;
}

/**
 * Добавляет карту
 * @param map ссылка на карту
 */
void Game::AddMap(const Map& map) {
    const size_t index = maps_.size();
    if (auto [it, inserted] = id_to_map_index_.emplace(map.GetId(), index); !inserted) {
        throw std::invalid_argument("Map with id "s + *map.GetId() + " already exists"s);
    } else {
        try {
            maps_.emplace_back(map);
        } catch (...) {
            id_to_map_index_.erase(it);
            throw;
        }
    }
}

/**
 * Получить контейнер, содержащий карты
 * @return Game::Maps
 */
const Game::Maps& Game::GetMaps() const noexcept {
    return maps_;
}

/**
 * Находит карту
 * @param id индекс карты
 * @return сырой указатель на карут, если она есть, иначе - nullptr.
 */
const Map* Game::FindMap(const Map::Id& id) const noexcept {
    if (auto it = id_to_map_index_.find(id); it != id_to_map_index_.end()) {
        return &maps_.at(it->second);
    }
    return nullptr;
}

/**
 * Обновляет словарь заполненности сессий 'map_to_sessions_'
 * @param index индекс сессии
 * @param session сессия
 * @return Указатель на сессию.
 */
std::shared_ptr<GameSession> Game::UpdateSessionFullness(size_t index, const GameSession& session) {
    map_to_sessions_[session.GetMapId()].emplace(session.AmountAvailableSeats(), index);
    return sessions_.at(index);
}

 /**
  * Создаёт пустую сессию
  * @param map_id индекс карты {string}
  * @return Возвращает индекс сессии и ссылку на неё
  */
std::pair<size_t, std::shared_ptr<GameSession>> Game::CreateFreeSession(const Map::Id& map_id) {
    auto map = FindMap(map_id);
    if (map == nullptr) {
        throw std::invalid_argument("Map id \""s + *map_id + "\" does not exist"s);
    }
    auto& session = sessions_.emplace_back(std::make_shared<GameSession>(*map));
    return {sessions_.size() - 1, session};
}

/**
 * Извлекает сессию, если есть указатель на неё
 * @param map_id индекс карты
 * @return возвращает индекс сессии и указатель на неё, если таковая имеется
 */
std::optional<std::pair<size_t, std::shared_ptr<GameSession>>> Game::ExtractFreeSession(const Map::Id& map_id) {
    if (map_to_sessions_.contains(map_id) &&
        !map_to_sessions_.at(map_id).empty() &&
        map_to_sessions_.at(map_id).begin()->first > 0) { // количество свободных мест > 0;
        auto& sessions = map_to_sessions_.at(map_id);
        auto index = sessions.begin()->second;
        if(sessions_.at(index) == nullptr){ // создадим новую сессию и поменяем указатель на неё.
            return std::pair<size_t, std::shared_ptr<GameSession>>{index, nullptr};
        }
        sessions.extract(sessions.begin()->first);
        return std::pair<size_t, std::shared_ptr<GameSession>>{index, sessions_.at(index)}; // нужная сессия уже существует
    }
    return std::nullopt; // создадим новую сессию
}

/**
 * Генерирует позицию объекта на дороге
 * @param session сессия
 * @param enable true - включить генератор, false - возвращать всегда стартовую точку дороги
 * @return Point2i на дороге
 */
Point2i Game::GenerateNewPosition(const model::GameSession& session, bool enable){
    return DropOffGenerator::GeneratePosition(session, enable);
}

}  // namespace model
