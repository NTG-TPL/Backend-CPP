#pragma once

#include <boost/asio.hpp>

#include "../json/json_loader.h"
#include "players.h"
#include "application_listener.h"

namespace app {
namespace net = boost::asio;
namespace fs = std::filesystem;
using namespace std::chrono_literals;
class Application {
public:

    explicit Application(const fs::path& config);
    Application(model::Game game, Players players);

    Application() = delete;
    ~Application() = default;

    const model::Game::Maps& GetMaps() const noexcept;
    std::shared_ptr<const model::Map> FindMap(const model::Map::Id& id) const noexcept;
    std::pair<Token, Player&> JoinGame(const model::Map::Id& map_id, const std::string &user_name);
    std::shared_ptr<Player> FindPlayer(const Token &token);
    const Players& GetPlayers() const noexcept;
    const model::Game& GetGameModel() const noexcept;
    void Tick(std::chrono::milliseconds tick);
    void SetRandomSpawm(bool enable) noexcept;
    bool GetRandomSpawm() const noexcept;
    void SetTickMode(bool enable) noexcept;
    bool GetTickMode() const noexcept;
    void SetApplicationListener(std::shared_ptr<ApplicationListener> listener);
    void Load(const Application& application);

private:
    static model::Game InitGame(const fs::path& config);

private:
    model::Game game_;
    Players players_;
    static inline std::atomic<uint64_t> dog_id_ = 0;
    bool enable_random_spawn = false;
    bool enable_tick_mode = false;

    std::shared_ptr<ApplicationListener> listener_;
};
} // namespace app