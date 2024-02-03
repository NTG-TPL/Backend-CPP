#ifndef GAME_SERVER_PLAYERS_H
#define GAME_SERVER_PLAYERS_H
#include "../model/model.h"
#include <random>
#include <sstream>
#include <deque>
#include <iomanip>

namespace app {
    namespace detail {
        struct TokenTag {};
    }  // namespace detail

    using Token = util::Tagged<std::string, detail::TokenTag>;

    class Player {
    public:
        using Id = util::Tagged<size_t, Player>;

        Player(const model::Dog& dog, const model::GameSession& session): dog_(dog), session_(session) {}
        [[nodiscard]] const model::GameSession& GetSession() const noexcept;
        [[nodiscard]] const model::Dog& GetDog() const noexcept;
        [[nodiscard]] Id GetId() const noexcept;

    private:
        const model::GameSession& session_;
        const model::Dog& dog_;
    };

    class PlayerTokens {
    public:
        const Player* FindPlayer(const Token& token) const noexcept;
        Token AddPlayer(Player* player);
        static inline constexpr uint8_t GetTokenLenght() noexcept{ return 32; }
    private:
        Token GetToken();
    private:
        using TokenHasher = util::TaggedHasher<Token>;
        using TokenToPlayer = std::unordered_map<Token, Player*, TokenHasher>;

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
    public:
        using PlayerList = std::deque<Player>;

        std::pair<Token, Player&> AddPlayer(const model::Dog& dog, const model::GameSession& session);
        const Player* FindByDogIdAndMapId(const model::Dog::Id& dog_id, const model::Map::Id& map_id) const;
        const Player* FindByToken(const Token& token);


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
        std::unordered_map<std::pair<model::Dog::Id, model::Map::Id>, Player*, Hash> player_by_ids;
        PlayerTokens tokens;
    };


}

#endif //GAME_SERVER_PLAYERS_H
