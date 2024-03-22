#pragma once

#include <boost/asio.hpp>

#include <vector>

#include "../json/json_loader.h"
#include "players.h"
#include "application_listener.h"

namespace app {
namespace net = boost::asio;
namespace fs = std::filesystem;
using namespace std::chrono_literals;
class Application {
public:
    Application() = delete;

    explicit Application(fs::path config):
            config_(std::move(config)), game_(std::move(InitGame(GetConfigFilePath()))) {
    }

    Application(fs::path config, model::Game game, Players players):
            config_(std::move(config)), game_(std::move(game)), players_(std::move(players)) {
    }

    ~Application() = default;

    const model::Game::Maps& GetMaps() const noexcept;
    std::shared_ptr<const model::Map> FindMap(const model::Map::Id& id) const noexcept;
    std::pair<Token, Player&> JoinGame(const model::Map::Id& map_id, const std::string &user_name);
    std::shared_ptr<Player> FindPlayer(const Token &token);
    const Players& GetPlayers() const & noexcept;
    Players& GetPlayers() & noexcept;
    const model::Game& GetGameModel() const noexcept;
    void Tick(std::chrono::milliseconds tick);
    void SetRandomSpawn(bool enable) noexcept;
    bool GetRandomSpawn() const noexcept;
    void SetTickMode(bool enable = false) noexcept;
    bool GetTickMode() const noexcept;
    void AddApplicationListener(std::shared_ptr<ApplicationListener> listener);
    const fs::path& GetConfigFilePath() const noexcept;
    std::vector<std::shared_ptr<ApplicationListener>>& GetApplicationListeners() noexcept;

private:
    static model::Game InitGame(const fs::path& config);

private:
    fs::path config_;
    model::Game game_;
    Players players_;
    static inline std::atomic<uint64_t> dog_id_ = 0;
    bool enable_random_spawn = false;
    bool enable_tick_mode = false;
    std::vector<std::shared_ptr<ApplicationListener>> listeners_;
};
} // namespace app