#pragma once

#include <boost/serialization/deque.hpp>
#include <boost/serialization/utility.hpp>

#include "../app/players.h"

namespace serialization {

    class PlayerRepr {
    public:
        PlayerRepr() = default;

        explicit PlayerRepr(const app::Player &player) :
                dog_id_(player.GetDog()->GetId()),
                session_id_(player.GetSession().GetId()) {
        }

        [[nodiscard]] model::Dog::Id GetDogId() const noexcept {
            return dog_id_;
        }

        [[nodiscard]] model::GameSession::Id GetSessionId() const noexcept {
            return session_id_;
        }

        template<typename Archive>
        void serialize(Archive &ar, [[maybe_unused]] const unsigned version) {
            ar & *dog_id_;
            ar & *session_id_;
        }

    private:
        model::Dog::Id dog_id_ = model::Dog::Id{0ul}; // id игрока совпадает с id собаки
        model::GameSession::Id session_id_ = model::GameSession::Id{0ul};
    };

    class PlayersRepr {
    public:
        PlayersRepr() = default;

        explicit PlayersRepr(const app::Players &players) {
            for (auto &[token, player]: players.GetPlayerTokens().GetTokenToPlayer()) {
                players_.emplace_back(*token, PlayerRepr(player));
            }
        }

        [[nodiscard]] app::Players Restore(const model::Game &game) const {
            app::Players players;
            for (auto &[token, player]: players_) {
                auto session = game.FindSession(player.GetSessionId());
                auto &new_player = players.players_.emplace_back(*session->FindDog(player.GetDogId()), session);
                players.tokens_.AddPlayerWithToken(app::Token{token}, new_player);
            }
            return players;
        }

        template<typename Archive>
        void serialize(Archive &ar, [[maybe_unused]] const unsigned version) {
            ar & players_;
        }

    private:
        std::deque<std::pair<std::string, PlayerRepr>> players_;
    };

} // namespace serialization