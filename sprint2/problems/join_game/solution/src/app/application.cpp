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
     * @param id индекс карты
     * @param user_name имя игрока (совпадает с именем собаки)
     * @return Возвращает пару <Токен игрока, ссылку на игрока>
     */
    std::pair<Token, Player&> Application::JoinGame(const model::Map::Id& id, const std::string& user_name){
        using namespace model;

        auto optional_session = game_.ExtractFreeSession(id);
        Dog dog(Dog::Id {dog_id++}, user_name);
        if(optional_session.has_value()){ // Нужная сессия есть или была когда-то
            auto [index, session] = optional_session.value();
            if(session == nullptr){ // Эта сессия когда-то была, но в ней нет людей
                session = game_.CreateSession(id, dog);
            }else { // Нужная сессия
                session->AddDog(dog);
                game_.UpdateSessionFullness(index, *session);
            }
            return players_.AddPlayer(dog, session);
        }
        // Свободных сессий нет, создаётся новая
        auto session = game_.CreateSession(id, dog);
        return players_.AddPlayer(dog, session);
    }

    /**
     * Поиск игрока
     * @param token токен игрока
     * @return Возвращает сырой указатель на игрока
     */
    const Player* Application::FindPlayer(const Token &token) {
        return players_.FindByToken(token);
    }

    /**
     * Вернуть Players
     * @return константная ссылка на Players
     */
    const Players& Application::GetPlayers() const noexcept {
        return players_;
    }
}