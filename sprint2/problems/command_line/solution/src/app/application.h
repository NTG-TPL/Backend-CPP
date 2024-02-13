#pragma once

#include <boost/asio.hpp>
#include "../json/json_loader.h"
#include "players.h"

namespace app {
namespace net = boost::asio;
namespace fs = std::filesystem;
using namespace std::chrono_literals;
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
    void Update(std::chrono::milliseconds tick);
    void SetRandomSpawm(bool enable) noexcept;
    bool GetRandomSpawm() const noexcept;
    void SetTickMode(bool enable) noexcept;
    bool GetTickMode() const noexcept;

private:
    static model::Game InitGame(const fs::path& config);

private:
    model::Game game_;
    Players players_;
    static inline std::atomic<uint64_t > dog_id_ = 0;
    bool enable_random_spawn = false;
    bool enable_tick_mode = false;
};
} // namespace app