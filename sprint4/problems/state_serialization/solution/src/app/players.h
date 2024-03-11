#pragma once
#include <random>
#include <sstream>
#include <deque>
#include <iomanip>
#include <utility>

#include "../model/model.h"

namespace serialization {
    class PlayersRepr;
}

namespace app {
namespace detail {
    struct TokenTag {};
}  // namespace detail

using Token = util::Tagged<std::string, detail::TokenTag>;

class Player {
public:
    using Id = util::Tagged<uint64_t , Player>;

    Player(std::shared_ptr<model::Dog> dog, std::shared_ptr<model::GameSession> session):
        dog_(std::move(dog)), session_(std::move(session)) {}
    [[nodiscard]] const model::GameSession& GetSession() const noexcept;
    [[nodiscard]] const std::shared_ptr<model::Dog>& GetDog();
    void DogMove(std::string_view dir, model::DimensionDouble speed);
    [[nodiscard]] const std::shared_ptr<model::Dog>& GetDog() const noexcept;
    [[nodiscard]] Id GetId() const noexcept;

private:
    std::shared_ptr<model::Dog> dog_;
    std::shared_ptr<model::GameSession> session_;
};

class PlayerTokens {
    friend class serialization::PlayersRepr;
public:
    using TokenHasher = util::TaggedHasher<Token>;
    using TokenToPlayer = std::unordered_map<Token, std::shared_ptr<Player>, TokenHasher>;

    PlayerTokens() = default;
    PlayerTokens(const PlayerTokens& other) {
        token_to_player_ = other.token_to_player_;
    }
    PlayerTokens& operator=(const PlayerTokens& other) {
        token_to_player_ = other.token_to_player_;
        return *this;
    }

    std::shared_ptr<Player> FindPlayer(const Token& token);
    Token AddPlayer(const Player& player);
    static inline constexpr uint8_t GetTokenLenght() noexcept{ return 32; }
    const TokenToPlayer& GetTokenToPlayer() const noexcept;

private:
    void AddPlayerWithToken(const app::Token& token, const app::Player& player);
    Token GetToken();

private:
    std::random_device random_device_;
    std::mt19937_64 generator1_{[this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }()};
    std::mt19937_64 generator2_{[this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }()};

    TokenToPlayer token_to_player_;
};

class Players {
    friend class serialization::PlayersRepr;
public:
    using PlayerList = std::deque<Player>;

    std::pair<Token, Player&> AddPlayer(const model::Dog::Id& id, const std::shared_ptr<model::GameSession>& session);
    std::shared_ptr<Player> FindByToken(const Token& token);
    const PlayerList& GetList() const noexcept;
    const PlayerTokens& GetPlayerTokens() const noexcept;

private:
    class Hash {
    public:
        inline size_t operator()(const std::pair<const model::Dog::Id&, const model::Map::Id&> ids) const{
            return hasher_(*ids.first) + 37*str_hasher_(*ids.second);
        }
    private:
        std::hash<size_t> hasher_;
        std::hash<std::string> str_hasher_;
    };
    PlayerList players_;
    PlayerTokens tokens_;
};
} // namespace app