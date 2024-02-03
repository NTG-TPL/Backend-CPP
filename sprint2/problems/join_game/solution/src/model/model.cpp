#include "model.h"

namespace model {
using namespace std::literals;

bool Road::IsHorizontal() const noexcept {
    return start_.y == end_.y;
}

bool Road::IsVertical() const noexcept {
    return start_.x == end_.x;
}

Point Road::GetStart() const noexcept {
    return start_;
}

Point Road::GetEnd() const noexcept {
    return end_;
}

const Rectangle&  Building::GetBounds() const noexcept {
    return bounds_;
}

const Office::Id& Office::GetId() const noexcept {
    return id_;
}

Point Office::GetPosition() const noexcept {
    return position_;
}

Offset Office::GetOffset() const noexcept {
    return offset_;
}

const Map::Id& Map::GetId() const noexcept {
    return id_;
}

const std::string& Map::GetName() const noexcept {
    return name_;
}

const Map::Buildings& Map::GetBuildings() const noexcept {
    return buildings_;
}

const Map::Roads& Map::GetRoads() const noexcept {
    return roads_;
}

const Map::Offices& Map::GetOffices() const noexcept {
    return offices_;
}

void Map::AddRoad(const Road& road) {
    roads_.emplace_back(road);
}

void Map::AddBuilding(const Building& building) {
    buildings_.emplace_back(building);
}

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

Dog::Id Dog::GetId() const noexcept {
    return id_;
}

const std::string& Dog::GetName() const noexcept{
    return name_;
}

const Map::Id& GameSession::GetMapId() const noexcept {
    return map_->GetId();
}

size_t GameSession::GetActivityPlayers() const noexcept{
    return dogs_.size();
}

size_t GameSession::GetLimitPlayers() const noexcept{
    return limit_;
}

bool GameSession::IsFull() const noexcept{
    return (limit_ - dogs_.size()) == 0;
}

// Добавляет собаку в сессию (если id собаки не уникален, или сессия переполнена, то возвращает false)
Dog * GameSession::AddDog(const Dog& dog) {
    if(dogs_.size() < limit_ && !dogs_.contains(dog.GetId())){
        return &(dogs_.emplace(dog.GetId(), dog).first)->second;
    }
    return nullptr;
}

size_t GameSession::EraseDog(const Dog::Id& id){
    return dogs_.erase(id);
}

const GameSession::Dogs& GameSession::GetDogs() const noexcept{
    return dogs_;
}

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

const Game::Maps& Game::GetMaps() const noexcept {
    return maps_;
}

const Map* Game::FindMap(const Map::Id& id) const noexcept {
    if (auto it = id_to_map_index_.find(id); it != id_to_map_index_.end()) {
        return &maps_.at(it->second);
    }
    return nullptr;
}

const GameSession& Game::AddSession(GameSession session) {
    auto map = FindMap(session.GetMapId());
    CheckMap(map, session.GetMapId());
    auto& sessions = map_to_sessions_.at(session.GetMapId());
    sessions.push(std::move(session));
    return sessions.top();
}

std::pair<Dog*, const GameSession&> Game::CreateSession(const Map::Id& map_id, const Dog& dog){
    auto map = FindMap(map_id);
    CheckMap(map, map_id);
    GameSession session(*map);
    auto dog_ = session.AddDog(dog);
    map_to_sessions_[map_id].push(std::move(session));
    return {dog_, map_to_sessions_.at(map_id).top()};
}

std::optional<GameSession> Game::ExtractFreeSession(const Map::Id& map_id) {
    if (map_to_sessions_.contains(map_id) && !map_to_sessions_.at(map_id).top().IsFull()) {
        auto& session = map_to_sessions_.at(map_id);
        GameSession out_session = std::move(const_cast<GameSession&>(session.top()));
        session.pop();
        return out_session;
    }
    return std::nullopt;
}

void Game::CheckMap(const Map* map, const Map::Id& map_id){
    if (map == nullptr) {
        throw std::invalid_argument("Map id \""s + *map_id + "\" does not exist"s);
    }
}

bool operator < (const GameSession& lhs, const GameSession& rhs){
    return (lhs.GetLimitPlayers() - lhs.GetActivityPlayers()) > (rhs.GetLimitPlayers()- rhs.GetActivityPlayers());
}

}  // namespace model
