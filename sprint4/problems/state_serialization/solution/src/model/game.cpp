#include "game.h"

namespace model {
using namespace std::string_literals;
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
std::shared_ptr<GameSession> Game::UpdateSessionFullness(GameSession::Id index, const GameSession& session) {
    auto id_session = id_to_session.at(index);
    map_to_sessions_[session.GetMapId()].emplace(session.AmountAvailableSeats(), id_session);
    return sessions_.at(id_session);
}

/**
 * Создаёт пустую сессию
 * @param map_id индекс карты {string}
 * @return Возвращает индекс сессии и ссылку на неё
 */
std::pair<GameSession::Id, std::shared_ptr<GameSession>> Game::CreateFreeSession(const Map::Id& map_id) {
    auto map = FindMap(map_id);
    if (map == nullptr) {
        throw std::invalid_argument("Map id \""s + *map_id + "\" does not exist"s);
    }
    auto ms = std::chrono::milliseconds(static_cast<size_t>(period_*1000));
    auto& session = sessions_.emplace_back(std::make_shared<GameSession>(GameSession::Id{game_session_id_++},
                                                                         *map,
                                                                         loot_gen::LootGenerator(ms, probability_)));
    id_to_session.emplace(session->GetId(), sessions_.size() - 1);
    return {session->GetId(), session};
}

/**
* Извлекает сессию, если есть указатель на неё
* @param map_id индекс карты
* @return возвращает индекс сессии и указатель на неё, если таковая имеется
*/
std::optional<std::pair<GameSession::Id, std::shared_ptr<GameSession>>> Game::ExtractFreeSession(const Map::Id& map_id) {
    if (map_to_sessions_.contains(map_id) &&
        !map_to_sessions_.at(map_id).empty() &&
        map_to_sessions_.at(map_id).begin()->first > 0) { // количество свободных мест > 0;
        auto& sessions = map_to_sessions_.at(map_id);
        auto index = GameSession::Id{sessions.begin()->second};
        if(sessions_.at(*index) == nullptr){ // создадим новую сессию и поменяем указатель на неё.
            return std::pair<GameSession::Id, std::shared_ptr<GameSession>>{index, nullptr};
        }
        sessions.extract(sessions.begin()->first);
        return std::pair<GameSession::Id, std::shared_ptr<GameSession>>{index, sessions_.at(*index)}; // нужная сессия уже существует
    }
    return std::nullopt; // создадим новую сессию
}

/**
* Обновляет состояние на tick миллисекунд
* @param tick время
*/
void Game::Update(std::chrono::milliseconds tick){
    for (auto& el: sessions_) {
        if(el != nullptr){
            el->Update(tick);
        }
    }
}
/**
* Задаёт параметры конфигурации для генерации потерянных вещей.
* Каждый period с вероятностью probability на картах будет появляться объект
* @param period интервал появления объекта (в секундах)
* @param probability вероятность появления объекта (probability от 0 до 1)
*/
void Game::SetLootGeneratorConfig(double period, double probability) noexcept {
    using namespace std::chrono_literals;
    period_ = period;
    probability_ = probability;
}

/**
* Возвращает интервал появления клада (в секундах)
* @return интервал появления клада
*/
double Game::GetLootPeriod() const noexcept{
    return period_;
}

/**
* Возвращает вероятность появления клада (в пределах [0, 1])
* @return вероятность появления клада
*/
double Game::GetLootProbability() const noexcept{
    return probability_;
}

/**
 * Получить контейнер с сессиями
 * @return контейнер с сессиями
 */
const Game::Sessions& Game::GetSessions() const noexcept {
    return sessions_;
}

/**
 * Искать карту с id
 * @param id Индекс сессии
 * @return Указатель на сессию или nullptr, если таковая не найдена
 */
std::shared_ptr<GameSession> Game::FindSession(GameSession::Id id) const noexcept {
    return id_to_session.contains(id) ? sessions_.at(id_to_session.at(id)) : nullptr;
}

/**
 * Добавить сессию
 * @param session сессия
 */
void Game::AddSession(const std::shared_ptr<GameSession>& session) {
    if(session == nullptr){
        throw std::runtime_error("AddSession:: session == nullptr");
    }
    sessions_.push_back(session);
    // Меняет счётчик id, чтобы не допускать пересечения индексов в дальнейшем при создании
    game_session_id_ = (*session->GetId() >= game_session_id_ ||
                        sessions_.size() > game_session_id_) ?
                                std::max(*session->GetId() + 1, sessions_.size()):
                                game_session_id_;

    auto id_session = sessions_.size() - 1;
    id_to_session[session->GetId()] = id_session;
    map_to_sessions_[session->GetMapId()].emplace(session->AmountAvailableSeats(), id_session);
}

/**
 * Добавить пустую сессию к карте с индексм id
 * @param id индекс карты
 */
void Game::AddFreeSession(const Map::Id& id) {
    auto map = FindMap(id);
    sessions_.push_back(nullptr);
    // Меняет счётчик id, чтобы не допускать пересечения индексов в дальнейшем при создании
    game_session_id_ = (sessions_.size() > game_session_id_) ? sessions_.size(): game_session_id_;
    auto id_session = sessions_.size() - 1;
    id_to_session[GameSession::Id{game_session_id_}] = id_session;
    map_to_sessions_[id].emplace(map->GetLimitPlayers(), id_session);
}

} // namespace model