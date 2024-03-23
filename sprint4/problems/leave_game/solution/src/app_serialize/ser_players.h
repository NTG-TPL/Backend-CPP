#pragma once

#include <boost/serialization/deque.hpp>
#include <boost/serialization/utility.hpp>

#include "../app/players.h"

namespace serialization {
    class PlayerRepr {
    public:
        PlayerRepr() = default;

        explicit PlayerRepr(const app::Player &player) :
                id_(std::move(player.GetId().ToString())),
                dog_id_(player.GetDog()->GetId()),
                session_id_(player.GetSession()->GetId()) {
        }

        [[nodiscard]] app::Player::Id GetId() const noexcept {
            return app::Player::Id::FromString(id_);
        }

        [[nodiscard]] model::Dog::Id GetDogId() const noexcept {
            return dog_id_;
        }

        [[nodiscard]] model::GameSession::Id GetSessionId() const noexcept {
            return session_id_;
        }

        template<typename Archive>
        void serialize(Archive &ar, [[maybe_unused]] const unsigned version) {
            ar & id_;
            ar & *dog_id_;
            ar & *session_id_;
        }

    private:
        std::string id_;
        model::Dog::Id dog_id_ = model::Dog::Id{0ul};
        model::GameSession::Id session_id_ = model::GameSession::Id{0ul};
    };

    class PlayersRepr {
    public:
        PlayersRepr() = default;

        explicit PlayersRepr(const app::Players &players) {
            for (auto &[token, player] : players.GetPlayerTokens().GetTokenToPlayer()) {
                players_.emplace_back(*token, PlayerRepr(*player));
            }
        }

        [[nodiscard]] app::Players Restore(const model::Game &game) const {
            app::Players players;
            for (auto &[token, player_repr]: players_) {
                auto session = game.FindSession(player_repr.GetSessionId());
                auto it = players.players_.emplace(player_repr.GetId(),
                                     std::make_shared<app::Player>(player_repr.GetId(),
                                                                   *session->FindDog(player_repr.GetDogId()), session));
                players.tokens_.AddPlayerWithToken(app::Token{token}, *it.first->second);
            }
            return players;
        }

        template<typename Archive>
        void serialize(Archive &ar, [[maybe_unused]] const unsigned version) {
            ar & players_;
        }

    private:
        std::vector<std::pair<std::string, PlayerRepr>> players_;
    };

} // namespace serialization