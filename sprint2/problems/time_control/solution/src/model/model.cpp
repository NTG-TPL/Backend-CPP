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
 * Получить границы дороги
 * @return Borders - границы дороги от минимального до максимального значений по оси x и по y
 */
Road::RoadRectangle::Borders Road::GetBorders() const noexcept {
    return road_rectangle_.GetBorders();
}

/**
 * Получить ширину дороги
 * @return Ширина дороги
 */
[[nodiscard]] DimensionDouble Road::GetWidth() const noexcept {
    return road_rectangle_.GetWidth();
}

/**
 * Проверяет, содержится ли точка в маршруте
 * @param point точка
 * @return true - если содержится, иначе - false
 */
bool Road::Contains(Point2d point) const noexcept{
    return road_rectangle_.GetBorders().Contains(point);
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
 * Двигает собаку в направлении dir со скоростью speed
 * @param dir Направление
 * @param speed Скорость
 */
void Dog::Move(std::string_view dir, DimensionDouble speed) {
    speed_ = Movement::MOVEMENT.at(dir)(speed);
    dir_ = dir;
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
 * Получить позицию собаки
 * @return Позиция собаки
 */
Point2d Dog::GetPosition() const noexcept {
    return position_;
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
 * @param enable true - включить генератор, false - возвращать всегда стартовую точку дороги
 * @return Point2i - точка на дороге
 */
Point2d GameSession::GenerateNewPosition(bool enable) const{
    return DropOffGenerator::GeneratePosition(*this, enable);
}

void GameSession::Update(double delta){
    for (auto& [id, dog] : dogs_) {
        if(dog.GetDirection() == Movement::STOP || (dog.GetSpeed().dx == 0 && dog.GetSpeed().dy == 0)){
            continue;
        }

        auto position = dog.GetPosition();
        auto speed = dog.GetSpeed();

        Point2d new_position = {position.x + speed.dx * delta,
                                position.y + speed.dy * delta};

        std::optional<Road::RoadRectangle::Borders> union_borders;

        for(const auto & road : map_.GetRoads()) {
            if(road.Contains(position)) {
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

        // Объединение пересечение траектории с допустимой областью
        if(!union_borders->Contains(new_position)){
            new_position.y = std::max(new_position.y, union_borders->min_y);
            new_position.y = std::min(new_position.y, union_borders->max_y);
            new_position.x = std::max(new_position.x, union_borders->min_x);
            new_position.x = std::min(new_position.x, union_borders->max_x);
            dog.Stand();
        }
        dog.SetPosition(new_position);
    }
}

/**
 * Генерирует позицию объекта на дороге
 * @param session сессия
 * @param enable true - включить генератор, false - возвращать всегда стартовую точку дороги
 * @return Point2i на дороге
 */
Point2d DropOffGenerator::GeneratePosition(const GameSession& session, bool enable){
    auto& map = session.GetMap();
    auto& roads = map.GetRoads();
    if (roads.empty()) {
            throw std::logic_error("На карте " + *map.GetId() + " нет дорог");
    }
    if (!enable) {
        return static_cast<Point2d>(roads.at(0).GetStart());
    }

    static std::random_device random_device;
    static std::mt19937 generator{random_device()};

    size_t num_road = 0;
    // Случайно выбираем дорогу
    if (roads.size() > 1) {
        std::uniform_int_distribution<DimensionInt> dist(0, static_cast<DimensionInt>(roads.size()) - 1);
        num_road = dist(generator);
    }

    Road road = roads.at(num_road);

    Point2d start = static_cast<Point2d>(road.GetStart()),
            end = static_cast<Point2d>(road.GetEnd());
    Point2d result = start;

    DimensionDouble width_2 = road.GetWidth() / 2;
    {
        std::uniform_real_distribution<DimensionDouble> dist(-width_2, width_2);
        width_2 = dist(generator);
        width_2 = std::floor(width_2 * 100)/100.0; // округление до двух знаков после запятой
    }


    auto get_range = [&gen = generator](double start, double end){
        std::uniform_real_distribution<> dist(start, end);
        return dist(gen);
    };

    if (road.IsHorizontal()) {
        result.x = get_range(start.x, end.x);
        result.y += width_2;
    } else if(road.IsVertical()) {
        result.y = get_range(start.y, end.y);
        result.x += width_2;
    }

    // округление до двух знаков после запятой
    result.x = std::floor(result.x * 100)/100.0;
    result.y = std::floor(result.y * 100)/100.0;

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
 * Обновляет состояние на tick секунд
 * @param tick время
 */
void Game::Update(double tick){
    for (auto& el: sessions_) {
        if(el != nullptr){
            el->Update(tick);
        }
    }
}

}  // namespace model
