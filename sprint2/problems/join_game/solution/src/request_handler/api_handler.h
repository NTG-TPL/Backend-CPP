#ifndef GAME_SERVER_API_HANDLER_H
#define GAME_SERVER_API_HANDLER_H
#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/json.hpp>
#include <string_view>

#include "content_type.h"
#include "error_response.h"
#include "endpoint.h"
#include "make_response.h"

#include "../http_server/http_server.h"
#include "../json/tag_invoke.h"
#include "../util/util.h"
#include "../app/application.h"

namespace http_handler {
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace json = boost::json;

    using namespace std::string_view_literals;

    struct CacheControl {
        CacheControl() = delete;
        constexpr static std::string_view NO_CACHE = "no-cache"sv;
    };

    struct Api {
        Api() = delete;
        constexpr static std::string_view GET      = "GET"sv;
        constexpr static std::string_view HEAD     = "HEAD"sv;
        constexpr static std::string_view GET_HEAD = "GET, HEAD"sv;
        constexpr static std::string_view POST     = "POST"sv;
    };

    class ApiHandler {
    public:
        explicit ApiHandler(app::Application& app): app_(app) {}

        ApiHandler(const ApiHandler&) = delete;
        ApiHandler& operator=(const ApiHandler&) = delete;

        static bool IsAPIRequest(const StringRequest& req);

        StringResponse HandleApiRequest(const StringRequest& req);

    private:
        StringResponse RequestForListPlayers(const StringRequest& req);
        static std::optional<app::Token> TryExtractToken(const StringRequest& req);
        StringResponse ExecuteAuthorized(const StringRequest& req, const std::function<StringResponse(const app::Player& player)>& action);
        StringResponse RequestToJoin(const StringRequest& req);
        StringResponse RequestToMaps(const StringRequest& req, std::string & decoded_target);
    private:
        app::Application& app_;
    };
}
#endif //GAME_SERVER_API_HANDLER_H
