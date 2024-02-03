#include "application.h"

namespace app {
    Application::Application(const fs::path& config):
        game_(std::move(InitGame(config))) {}

    model::Game Application::InitGame(const fs::path& config){
        if (!fs::exists(config)) {
            throw std::invalid_argument("Invalid file path: " + config.string());
        }
        return json_loader::LoadGame(config);
    }

    const model::Map* Application::FindMap(const model::Map::Id &id) const noexcept {
        return game_.FindMap(id);
    }

    const model::Game::Maps& Application::GetMaps() const noexcept {
        return game_.GetMaps();
    }

    std::pair<Token, Player&> Application::JoinGame(const model::Map::Id& id, const std::string& user_name){
        using namespace model;

        auto session = game_.ExtractFreeSession(id);
        Dog dog(Dog::Id {++dog_id}, user_name);
        if(!session.has_value()){ // Если нет свободной сесии, то создаёт новую
            auto [dog_ref, new_session] = game_.CreateSession(id, dog);
            return players_.AddPlayer(*dog_ref, new_session);
        }else { // Иначе обновляет старую сессию
            auto dog_ref = session->AddDog(dog);
            auto updated_session = game_.AddSession(session.value());
            return players_.AddPlayer(*dog_ref, updated_session);
        }
    }

    const Player* Application::FindPlayer(const Token &t) {
        return players_.FindByToken(t);
    }

    const Players& Application::GetPlayers() const noexcept {
        return players_;
    }
}