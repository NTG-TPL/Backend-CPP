#include "players.h"

namespace app {
/**
 * Получить сессию игрока
 * @return Ссылка на сессию
 */
std::shared_ptr<const model::GameSession>Player::GetSession() const & noexcept {
    return session_;
}

std::shared_ptr<model::GameSession> Player::GetSession() & noexcept {
    return session_;
}

/**
 * Получить собаку игрока
 * @return Ссылка на собаку
 */
std::shared_ptr<model::Dog> Player::GetDog() & noexcept {
    return dog_;
}

/**
 * Получить собаку игрока
 * @return Константная ссылка на собаку
 */
[[nodiscard]] std::shared_ptr<const model::Dog> Player::GetDog() const & noexcept{
    return dog_;
}


/**
 * Перемещает собаку в направлении dir со скоростью speed
 * @param dir Направление перемещения
 * @param speed Скорость перемещения
 */
void Player::DogMove(std::string_view dir, model::DimensionDouble speed){
    if(dog_){
        dog_->Move(dir, speed);
    }
}

/**
 * Получить индекс игрока
 * @return Индекс игрока
 */
Player::Id Player::GetId() const noexcept{
    return id_;
}

/**
 * Найти игрока по токену
 * @param token Токен игрока
 * @return Указатель на константу Player
 */
std::shared_ptr<Player> PlayerTokens::FindPlayer(const Token& token) {
    return token_to_player_.contains(token) ? token_to_player_.at(token) : nullptr;
}

/**
 * Удалить связь токена с игроком
 * @param token токен
 */
void PlayerTokens::DeleteTokenPlayer(const Token& token) {
    if(token_to_player_.contains(token)){
        token_to_player_.erase(token);
    }
}

/**
 * Добавить игрока
 * @param player ссылка на игрока
 * @return Токен игрока
 */
Token PlayerTokens::AddPlayer(std::shared_ptr<Player> player){
    Token token{GetToken()};
    token_to_player_.emplace(token, std::move(player));
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
void PlayerTokens::AddPlayerWithToken(const app::Token& token, const app::Player& player) {
    token_to_player_.emplace(token, std::make_shared<app::Player>(player));
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
    auto player_shrd = std::make_shared<Player>(app::Player::Id::New(), *session->FindDog(id), session);
    auto it = players_.emplace(player_shrd->GetId(), player_shrd);
    auto Token = tokens_.AddPlayer(player_shrd);
    return {Token, *it.first->second};
}

/**
 * Ищет игрока по индексу его собаки и id карты
 * @param dog_id индекс собаки
 * @param map_id индекс карты
 * @return Указатель на игрока
 */
std::shared_ptr<Player> Players::FindByToken(const Token& token){
    return tokens_.FindPlayer(token);
}

/**
 * Удалить игрока по токену
 * @param token токен игрока
 */
void Players::DeleteByToken(const Token& token) {
    auto player = tokens_.FindPlayer(token);
    if(player){
        auto session = player->GetSession();
        auto dog = player->GetDog();
        if(session && dog){
            session->DeleteDog(dog->GetId());
        }
        players_.erase(player->GetId());
        tokens_.DeleteTokenPlayer(token);
    }
}

/**
 * Получить список игроков
 * @return список игроков
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