#ifndef GAME_SERVER_APPLICATION_H
#define GAME_SERVER_APPLICATION_H

#include <boost/asio.hpp>
#include "../json/json_loader.h"
#include "players.h"

namespace app {
    namespace net = boost::asio;
    namespace fs = std::filesystem;
    class Application {
    public:
        explicit Application(const fs::path& config);

        Application() = delete;
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;
        ~Application() = default;

        const model::Map* FindMap(const model::Map::Id& id) const noexcept;
        const model::Game::Maps& GetMaps() const noexcept;
        std::pair<Token, Player&> JoinGame(const model::Map::Id& map_id, const std::string &user_name);
        Player* FindPlayer(const Token &token);
        const Players& GetPlayers() const noexcept;

    private:
        static model::Game InitGame(const fs::path& config);

    private:
        model::Game game_;
        Players players_;
        static inline std::atomic<size_t> dog_id = 0;
    };
}

#endif //GAME_SERVER_APPLICATION_H
