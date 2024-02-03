#include "players.h"

namespace app {
    const model::GameSession& Player::GetSession() const noexcept {
        return session_;
    }

    const model::Dog& Player::GetDog() const noexcept {
        return dog_;
    }

    Player::Id Player::GetId() const noexcept{
        return Player::Id{*dog_.GetId()};
    }

    const Player* PlayerTokens::FindPlayer(const Token& token) const noexcept {
        return token_to_player_.contains(token) ? token_to_player_.at(token) : nullptr;
    }

    Token PlayerTokens::AddPlayer(Player* player){
        Token token{GetToken()};
        token_to_player_[token] = player;
        return token;
    }

    Token PlayerTokens::GetToken() {
        std::stringstream ss;
        ss << std::setw(16) << std::setfill('0') << std::hex << generator1_();
        ss << std::setw(16) << std::setfill('0') << std::hex << generator2_();
        return Token(ss.str());
    }

    std::pair<Token, Player&> Players::AddPlayer(const model::Dog& dog, const model::GameSession& session) {
        auto& player = players_.emplace_back(dog, session);
        auto Token = tokens.AddPlayer(&player);
        player_by_ids[{player.GetDog().GetId(), player.GetSession().GetMapId()}] = &player;
        return {Token, player};
    }

    const Player* Players::FindByDogIdAndMapId(const model::Dog::Id& dog_id, const model::Map::Id& map_id) const{
        if(player_by_ids.contains({dog_id, map_id})){
            return player_by_ids.at({dog_id, map_id});
        }
        return nullptr;
    }

    const Player* Players::FindByToken(const Token& token){
        return tokens.FindPlayer(token);
    }
}