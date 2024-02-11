
#include "api_handler.h"

namespace http_handler {
    bool ApiHandler::IsAPIRequest(const StringRequest& req) {
        return req.target().starts_with(EndPoint::API);
    }

    /**
     * Обработать запрос к API
     * @param req Запрос StringRequest {http::request<http::string_body>}
     * @return Возвращает ответ StringResponse{http::response<http::string_body>}
     */
    StringResponse ApiHandler::HandleApiRequest(const StringRequest& req){
        using namespace model;
        using namespace std::literals::string_view_literals;

        auto method_not_allowed = [&req](std::string_view text, std::string_view allow){
            return MakeTextResponse(req, http::status::method_not_allowed, text, CacheControl::NO_CACHE, allow);
        };

        auto is_get_or_head_request = [&req](){
            return (req.method() == http::verb::get || req.method() == http::verb::head);
        };

        auto is_post_request = [&req](){
            return (req.method() == http::verb::post);
        };

        auto decoded_target = util::UrlDecode(std::string{req.target()});

        if (decoded_target.starts_with(EndPoint::MAPS)) {
            return !is_get_or_head_request() ? method_not_allowed(ErrorResponse::INVALID_GET, Api::GET_HEAD) : RequestToMaps(req, decoded_target);
        }

        if(decoded_target.starts_with(EndPoint::GAME)){
            if (decoded_target == EndPoint::PLAYERS) {
                return !is_get_or_head_request() ? method_not_allowed(ErrorResponse::INVALID_GET, Api::GET_HEAD) : RequestForListPlayers(req);
            }

            if (decoded_target == EndPoint::JOIN) {
                return !is_post_request() ? method_not_allowed(ErrorResponse::INVALID_POST, Api::POST) : RequestToJoin(req);
            }

            if (decoded_target == EndPoint::STATE) {
                return !is_get_or_head_request() ? method_not_allowed(ErrorResponse::INVALID_GET, Api::GET_HEAD) : RequestToState(req);
            }

            if (decoded_target == EndPoint::ACTION) {
                return !is_post_request() ? method_not_allowed(ErrorResponse::INVALID_POST, Api::POST) : RequestToAction(req);
            }

            if(app_.GetTickMode()){
                if(decoded_target == EndPoint::TICK){
                    return !is_post_request() ? method_not_allowed(ErrorResponse::INVALID_POST, Api::POST) : RequestToTick(req);
                }
            }
        }

        return MakeTextResponse(req, http::status::bad_request, ErrorResponse::BAD_REQ, CacheControl::NO_CACHE);
    }

    /**
     * Ответ на запрос получения списка игроков в той же сессии
     * @param req Запрос StringRequest {http::request<http::string_body>}
     * @return Возвращает ответ StringResponse{http::response<http::string_body>}
     */
    StringResponse ApiHandler::RequestForListPlayers(const StringRequest& req){
        using namespace model;

        return ExecuteAuthorized(req, [&req](const app::Player &player) {
            json::object obj;
            auto dogs = player.GetSession().GetDogs();
            for (const auto &[id, dog]: dogs) {
                obj[std::to_string(*id)] = {MapKey::NAME, dog.GetName()};
            }

            return MakeTextResponse(req, http::status::ok, json::serialize(obj), CacheControl::NO_CACHE);
        });
    }

    /**
     * Попытаться извлечь Токен аутентификации
     * @param req Запрос StringRequest {http::request<http::string_body>}
     * @return Возвращает токен, если структура запроса и токена выли верны, иначе nullopt
     */
    std::optional<app::Token> ApiHandler::TryExtractToken(const StringRequest& req) {
        std::string authorization;

        if (req.count(http::field::authorization)) {
            authorization = req.at(http::field::authorization);
        } else {
            return std::nullopt;
        }

        const std::string bearer = "Bearer ";
        if (authorization.starts_with(bearer)) {
            authorization = authorization.substr(bearer.size());
        } else {
            return std::nullopt;
        }

        if (!std::all_of(authorization.begin(), authorization.end(), ::isxdigit)) {
            return std::nullopt;
        }

        if (authorization.size() != app::PlayerTokens::GetTokenLenght()) {
            return std::nullopt;
        }

        std::transform(authorization.begin(), authorization.end(), authorization.begin(),
                       [](unsigned char c) {
                           return std::tolower(c);
                       });
        return app::Token(authorization);
    }

    /**
     * Проверяет по токену, что пользователю разрешено выполнить некоторое действие
     * @param req Запрос StringRequest {http::request<http::string_body>}
     * @param action действие
     * @return Возвращает ответ StringResponse{http::response<http::string_body>}
     */
    StringResponse ApiHandler::ExecuteAuthorized(const StringRequest& req,
                                                  const std::function<StringResponse(app::Player&)>& action) {
        if (auto token = ApiHandler::TryExtractToken(req); token.has_value()) {
            auto player = app_.FindPlayer(*token);
            if (player == nullptr) {
                return MakeTextResponse(req, http::status::unauthorized, ErrorResponse::UNKNOWN_TOKEN, CacheControl::NO_CACHE);
            }
            return action(*player);
        } else {
            return MakeTextResponse(req, http::status::unauthorized, ErrorResponse::INVALID_TOKEN, CacheControl::NO_CACHE);
        }
    }

    /**
     * Ответ на запрос входа в игру
     * @param req Запрос StringRequest {http::request<http::string_body>}
     * @return Возвращает ответ StringResponse{http::response<http::string_body>}
     */
    StringResponse ApiHandler::RequestToJoin(const StringRequest& req){
        using namespace model;
        using namespace std::literals;

        std::string user_name;
        std::string map_id;
        try {
            auto obj = boost::json::parse(req.body()).as_object();
            user_name = obj.at(UserKey::USER_NAME).as_string();
            map_id = obj.at(UserKey::MAP_ID).as_string();
        } catch (...) {
            return MakeTextResponse(req, http::status::bad_request, ErrorResponse::BAD_PARSE_JOIN, CacheControl::NO_CACHE);
        }

        if (user_name.empty()) {
            return MakeTextResponse(req, http::status::bad_request, ErrorResponse::USERNAME_EMPTY, CacheControl::NO_CACHE);
        }

        if (auto map = app_.FindMap(Map::Id(map_id)); map == nullptr) {
            return MakeTextResponse(req, http::status::not_found, ErrorResponse::MAP_NOT_FOUND, CacheControl::NO_CACHE);
        }

        auto [token, player] = app_.JoinGame(Map::Id(map_id), user_name);
        json::value value = {{UserKey::USER_TOKEN, *token},
                             {UserKey::PLAYER_ID,  *player.GetId()}};
        return MakeTextResponse(req, http::status::ok, boost::json::serialize(value), CacheControl::NO_CACHE);
    }

    /**
     * Ответ на запрос к картам
     * @param req Запрос StringRequest {http::request<http::string_body>}
     * @param decoded_target декодированный URI
     * @return Возвращает ответ StringResponse{http::response<http::string_body>}
     */
    StringResponse ApiHandler::RequestToMaps(const StringRequest& req, std::string & decoded_target) {
        if (decoded_target == EndPoint::MAPS) {
            return MakeTextResponse(req, http::status::ok, json::serialize(json::value_from(app_.GetMaps())), CacheControl::NO_CACHE);
        }
        if (decoded_target.starts_with(EndPoint::MAP)) {
            std::string id = std::string{decoded_target.substr(EndPoint::MAP.size())};
            auto map = app_.FindMap(model::Map::Id(id));
            if (map) {
                return MakeTextResponse(req, http::status::ok, json::serialize(json::value_from(*map)), CacheControl::NO_CACHE);
            } else {
                return MakeTextResponse(req, http::status::not_found, ErrorResponse::MAP_NOT_FOUND, CacheControl::NO_CACHE);
            }
        }
        return MakeTextResponse(req, http::status::bad_request, ErrorResponse::BAD_REQ, CacheControl::NO_CACHE);
    }

    /**
     * Ответ на запрос о получении состояния игры
     * @param req Запрос StringRequest {http::request<http::string_body>}
     * @return Возвращает ответ StringResponse{http::response<http::string_body>}
     */
    StringResponse ApiHandler::RequestToState(const StringRequest& req) {
        using namespace model;

        return ExecuteAuthorized(req, [&req](const app::Player &player) {
            json::object obj;
            auto dogs = player.GetSession().GetDogs();
            json::object json_dogs;
            for (const auto &[id, dog]: dogs) {
                json_dogs[std::to_string(*id)] = json::value_from(dog);
            }
            obj[UserKey::PLAYERS] = json_dogs;

            return MakeTextResponse(req, http::status::ok, json::serialize(obj), CacheControl::NO_CACHE);
        });
    }

    StringResponse ApiHandler::RequestToAction(const StringRequest& req) {
        using namespace model;

        return ExecuteAuthorized(req, [&req](app::Player &player) {
            json::object obj;
            auto& session = player.GetSession();
            auto& map = session.GetMap();
            auto& dog = player.GetDog();

            try{
                json::object json_body = json::parse(req.body()).as_object();
                dog.Move(json_body.at(UserKey::MOVE).as_string(), map.GetDogSpeed());
            } catch (...) {
                return MakeTextResponse(req, http::status::bad_request, ErrorResponse::BAD_PARSE_ACTION, CacheControl::NO_CACHE );
            }

            return MakeTextResponse(req, http::status::ok, json::serialize(obj), CacheControl::NO_CACHE);
        });
    }

    /**
     * Ответ на запрос об обновлении игры
     * @param req Запрос StringRequest {http::request<http::string_body>}
     * @return Возвращает ответ StringResponse{http::response<http::string_body>}
     */
    StringResponse ApiHandler::RequestToTick(const StringRequest& req){
        using namespace model;
            json::object obj;
        std::chrono::milliseconds milliseconds = 0ms;
        try{
            json::object json_body = json::parse(req.body()).as_object();
            milliseconds = std::chrono::milliseconds(json_body.at(UserKey::TIME_INTERVAL).as_int64());
        } catch (...) {
            return MakeTextResponse(req, http::status::bad_request, ErrorResponse::BAD_PARSE_TICK, CacheControl::NO_CACHE );
        }

        app_.Update(milliseconds);
        /*auto& players = app_.GetPlayers();
        json::object json_dogs;
        for (const auto& player: players.GetList()) {
            json_dogs[std::to_string(*player.GetId())] = json::value_from(player.GetDog());
        }
        obj[UserKey::PLAYERS] = json_dogs;*/
        return MakeTextResponse(req, http::status::ok, json::serialize(obj), CacheControl::NO_CACHE);
    }
}