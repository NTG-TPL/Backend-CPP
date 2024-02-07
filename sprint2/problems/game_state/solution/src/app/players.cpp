#include "players.h"

namespace app {
    /**
     * Получить сессию игрока
     * @return Ссылка на сессию
     */
    const model::GameSession& Player::GetSession() const noexcept {
        return *session_;
    }

    /**
     * Получить собаку игрока
     * @return Ссылка на собаку
     */
    const model::Dog& Player::GetDog() const noexcept {
        return dog_;
    }

    /**
     * Получить индекс игрока (совпадает с индексом собаки)
     * @return Индекс игрока
     */
    Player::Id Player::GetId() const noexcept{
        return Player::Id{*dog_.GetId()};
    }

    /**
     * Найти игрока по токену
     * @param token Токен игрока
     * @return Сырой указатель на константу Player
     */
    const Player* PlayerTokens::FindPlayer(const Token& token) const noexcept {
        return token_to_player_.contains(token) ? &token_to_player_.at(token) : nullptr;
    }

    /**
     * Добавить игрока
     * @param player ссылка на игрока
     * @return Токен игрока
     */
    Token PlayerTokens::AddPlayer(Player& player){
        Token token{GetToken()};
        token_to_player_.emplace(token, player);
        return token;
    }

    /**
     * Получить Токен
     * @return Токен
     */
    Token PlayerTokens::GetToken() {
        std::stringstream ss;
        ss << std::setw(16) << std::setfill('0') << std::hex << generator1_();
        ss << std::setw(16) << std::setfill('0') << std::hex << generator2_();
        return Token(ss.str());
    }

    /**
     * Добавить игрока
     * @param id индекс собаки
     * @param session указатель на сессию игрока
     * @return Возвращает пару <Токен игрока, ссылка на игрока>
     */
    std::pair<Token, Player&> Players::AddPlayer(const model::Dog::Id& id, const std::shared_ptr<model::GameSession>& session) {
        Player& player = players_.emplace_back(*session->FindDog(id), session);
        auto Token = tokens.AddPlayer(player);
        player_by_ids[{player.GetDog().GetId(), player.GetSession().GetMapId()}] = &player;
        return {Token, player};
    }

    /**
     * Ищет игрока по индексу его собаки и id карты
     * @param dog_id индекс собаки
     * @param map_id индекс карты
     * @return Указатель на игрока
     */
    const Player* Players::FindByDogIdAndMapId(const model::Dog::Id& dog_id, const model::Map::Id& map_id) const{
        if(player_by_ids.contains({dog_id, map_id})){
            return player_by_ids.at({dog_id, map_id});
        }
        return nullptr;
    }

    /**
     * Ищет игрока по индексу его собаки и id карты
     * @param dog_id индекс собаки
     * @param map_id индекс карты
     * @return Указатель на игрока
     */
    const Player* Players::FindByToken(const Token& token){
        return tokens.FindPlayer(token);
    }
}