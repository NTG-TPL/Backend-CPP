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
const std::shared_ptr<model::Dog>& Player::GetDog() {
    return dog_;
}

/**
 * Перемещает собаку в направлении dir со скоростью speed
 * @param dir Направление перемещения
 * @param speed Скорость перемещения
 */
void Player::DogMove(std::string_view dir, model::DimensionDouble speed){
    dog_->Move(dir, speed);
}

/**
 * Получить собаку игрока
 * @return Константная ссылка на собаку
 */
[[nodiscard]] const std::shared_ptr<model::Dog>& Player::GetDog() const noexcept{
    return dog_;
}

/**
 * Получить индекс игрока (совпадает с индексом собаки)
 * @return Индекс игрока
 */
Player::Id Player::GetId() const noexcept{
    return Player::Id{*dog_->GetId()};
}

/**
 * Найти игрока по токену
 * @param token Токен игрока
 * @return Сырой указатель на константу Player
 */
Player* PlayerTokens::FindPlayer(const Token& token) {
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
 * Возвращает словарь токенов и игроков
 * @return PlayerTokens::TokenToPlayer
 */
const PlayerTokens::TokenToPlayer& PlayerTokens::GetTokenToPlayer() const noexcept {
    return token_to_player_;
}

/**
 * добавляет игрока вместе с токеном
 * @param token Токен
 * @param player игрок
 */
void PlayerTokens::AddPlayerWithToken(const app::Token& token, app::Player& player) {
    token_to_player_.emplace(token, player);
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
    auto Token = tokens_.AddPlayer(player);
    return {Token, player};
}

/**
 * Ищет игрока по индексу его собаки и id карты
 * @param dog_id индекс собаки
 * @param map_id индекс карты
 * @return Указатель на игрока
 */
Player* Players::FindByToken(const Token& token){
    return tokens_.FindPlayer(token);
}

/**
 * Получить список игроков
 * @return список игроков {deque<Player>}
 */
const Players::PlayerList& Players::GetList() const noexcept{
    return players_;
}

/**
 * Вернуть объект, отвечающий за хранение токенов
 * @return PlayerTokens
 */
const PlayerTokens& Players::GetPlayerTokens() const noexcept {
    return tokens_;
}

} // namespace app