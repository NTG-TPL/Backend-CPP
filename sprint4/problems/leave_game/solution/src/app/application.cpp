#include "application.h"

#include <utility>

namespace app {

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
std::shared_ptr<const model::Map> Application::FindMap(const model::Map::Id &id) const noexcept {
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
 * @return Возвращает указатель на игрока
 */
std::shared_ptr<Player> Application::FindPlayer(const Token &token) {
    return players_.FindByToken(token);
}

/**
 * Вернуть Players
 * @return константная ссылка на Players
 */
const Players& Application::GetPlayers() const & noexcept {
    return players_;
}

Players& Application::GetPlayers() & noexcept {
    return players_;
}

/**
 * Получить игровую модель
 * @return Игровая модель
 */
const model::Game& Application::GetGameModel() const noexcept {
    return game_;
}

/**
 * Обновляет состояние приложения
 * @param tick время
 */
void Application::Tick(std::chrono::milliseconds tick){
    game_.Update(tick);
    for (auto& listener: listeners_) {
        if(listener){
            listener->OnTick(tick);
        }
    }
}

/**
 * Установить свойство случайного размещения игроков
 * @param enable если true, то размещение игроков случайное,
 *  иначе - игроки высаживаются в начале первой дороги.
 */
void Application::SetRandomSpawn(bool enable) noexcept {
    enable_random_spawn = enable;
}

/**
 * Получить свойство случайного размещения игроков
 * @return enable_random_spawn
 */
bool Application::GetRandomSpawn() const noexcept {
    return enable_random_spawn;
}

/**
 * Включить TickMode
 * @param enable eсли true, то управление временем происходит вручную (через запросы к API),
 * иначе - автоматически.
 */
void Application::SetTickMode(bool enable) noexcept {
    enable_tick_mode = enable;
}

/**
 * Получить свойство enable_tick_mode
 * @return enable_tick_mode
 */
bool Application::GetTickMode() const noexcept {
    return enable_tick_mode;
}

/**
 * Устанавливает слушателя
 * @param listener слушатель
 */
void Application::AddApplicationListener(std::shared_ptr<ApplicationListener> listener) {
        listeners_.push_back(std::move(listener));
}

/**
 * Получить путь к файлу конфигурацц
 * @return путь к файлу конфигурацц
 */
const fs::path& Application::GetConfigFilePath() const noexcept {
    return config_;
}

/**
 * Получить слушателей приложения
 * @return Контейнер слушателей
 */
std::vector<std::shared_ptr<ApplicationListener>>& Application::GetApplicationListeners() noexcept {
    return listeners_;
}

} // namespace app