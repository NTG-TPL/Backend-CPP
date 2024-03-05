#include "application.h"

namespace app {
Application::Application(const fs::path& config):
    game_(std::move(InitGame(config))) {}

/**
 * Инициализация пути до конфигурационных файлов
 * @param config пути до конфигурационных файлов
 * @return объект, отвечающий за состояние игры
 */
model::Game Application::InitGame(const fs::path& config){
    if (!fs::exists(config)) {
        throw std::invalid_argument("Invalid file path: " + config.string());
    }
    return json_loader::LoadGame(config);
}

/**
 * Находит карту
 * @param id индекс карты
 * @return сырой указатель на карут, если она есть, иначе - nullptr.
 */
const model::Map* Application::FindMap(const model::Map::Id &id) const noexcept {
    return game_.FindMap(id);
}

/**
 * Получить контейнер, содержащий карты
 * @return Game::Maps
 */
const model::Game::Maps& Application::GetMaps() const noexcept {
    return game_.GetMaps();
}

/**
 * Войти в игру
 * @param map_id индекс карты
 * @param user_name имя игрока (совпадает с именем собаки)
 * @return Возвращает пару <Токен игрока, ссылку на игрока>
 */
std::pair<Token, Player&> Application::JoinGame(const model::Map::Id& map_id, const std::string& user_name){
    using namespace model;
    auto optional_session = game_.ExtractFreeSession(map_id);
    Dog::Id id {dog_id_++};

    auto create_session_and_add_dog = [&](){
        auto [current_index, session] = game_.CreateFreeSession(map_id); // создаёт пустую сессию
        session->AddDog({id, user_name, static_cast<Point2d>(session->GenerateNewPosition(enable_random_spawn))}); // добавляет в сессию собаку
        game_.UpdateSessionFullness(current_index, *session); // обновляет порядок сессий относительно заполненности
        return session;
    };

    if(optional_session.has_value()){ // Нужная сессия есть или была когда-то
        auto [index, session] = optional_session.value();
        if(session == nullptr){ // Эта сессия когда-то была, но в ней нет людей
            session = create_session_and_add_dog();
        }else { // Нужная сессия
            session->AddDog({id, user_name, static_cast<Point2d>(session->GenerateNewPosition(enable_random_spawn))});
            game_.UpdateSessionFullness(index, *session);
        }
        return players_.AddPlayer(id, session);
    }
    // Свободных сессий нет, создаётся новая
    auto session = create_session_and_add_dog();
    return players_.AddPlayer(id, session);
}

/**
 * Поиск игрока
 * @param token токен игрока
 * @return Возвращает сырой указатель на игрока
 */
Player* Application::FindPlayer(const Token &token) {
    return players_.FindByToken(token);
}

/**
 * Вернуть Players
 * @return константная ссылка на Players
 */
const Players& Application::GetPlayers() const noexcept {
    return players_;
}

/**
 * Обновляет состояние приложения
 * @param tick время
 */
void Application::Update(std::chrono::milliseconds tick){
    game_.Update(tick);
}

void Application::SetRandomSpawm(bool enable) noexcept {
    enable_random_spawn = enable;
}

bool Application::GetRandomSpawm() const noexcept {
    return enable_random_spawn;
}

void Application::SetTickMode(bool enable) noexcept {
    enable_tick_mode = enable;
}

bool Application::GetTickMode() const noexcept {
    return enable_tick_mode;
}
} // namespace app