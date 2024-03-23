#pragma once

#include <boost/container_hash/hash.hpp>

#include <random>
#include <sstream>
#include <deque>
#include <iomanip>
#include <utility>

#include "../model/model.h"
#include "../util/tagged_uuid.h"

namespace serialization {
    class PlayersRepr;
}

namespace app {
namespace detail {
    struct TokenTag {};
    struct PlayerTag {};
}  // namespace detail

using Token = util::Tagged<std::string, detail::TokenTag>;

class Player {
public:
    using Id = util::TaggedUUID<detail::PlayerTag>;

    Player(Id id, std::shared_ptr<model::Dog> dog, std::shared_ptr<model::GameSession> session) :
            id_(std::move(id)), dog_(std::move(dog)), session_(std::move(session)) {
    }

    [[nodiscard]] std::shared_ptr<const model::GameSession> GetSession() const & noexcept;

    std::shared_ptr<model::GameSession> GetSession() & noexcept;

    [[nodiscard]] std::shared_ptr<model::Dog> GetDog() & noexcept;

    [[nodiscard]] std::shared_ptr<const model::Dog> GetDog() const & noexcept;

    void DogMove(std::string_view dir, model::DimensionDouble speed);

    [[nodiscard]] Id GetId() const noexcept;

private:
    Id id_;
    std::shared_ptr<model::Dog> dog_;
    std::shared_ptr<model::GameSession> session_;
};
}; // namespace app

namespace util {
template<>
struct TaggedHasher<app::Player::Id> {
    size_t operator () (const app::Player::Id& value) const {
        return boost::hash<typename app::Player::Id::ValueType>()(*value);
    }
};
} // namespace util

namespace app {
class PlayerTokens {
    friend class serialization::PlayersRepr;
public:
    using TokenHasher = util::TaggedHasher<Token>;
    using TokenToPlayer = std::unordered_map<Token, std::shared_ptr<Player>, TokenHasher>;

    PlayerTokens() = default;
    PlayerTokens(const PlayerTokens& other):
        token_to_player_(other.token_to_player_){
    }

    PlayerTokens& operator=(const PlayerTokens& other) {
        token_to_player_ = other.token_to_player_;
        return *this;
    }

    std::shared_ptr<Player> FindPlayer(const Token& token);
    void DeleteTokenPlayer(const Token& token);
    Token AddPlayer(std::shared_ptr<Player> player);
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
    using PlayerIdHasher = util::TaggedHasher<Player::Id>;
    using PlayerList = std::unordered_map<Player::Id, std::shared_ptr<Player>, PlayerIdHasher>;

    std::pair<Token, Player&> AddPlayer(const model::Dog::Id& id, const std::shared_ptr<model::GameSession>& session);
    std::shared_ptr<Player> FindByToken(const Token& token);
    void DeleteByToken(const Token& token);
    const PlayerList& GetList() const noexcept;
    const PlayerTokens& GetPlayerTokens() const noexcept;

private:
    PlayerList players_;
    PlayerTokens tokens_;
};
} // namespace app