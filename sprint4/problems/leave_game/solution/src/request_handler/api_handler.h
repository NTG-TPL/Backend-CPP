#pragma once

#include <boost/json.hpp>
#include <boost/url.hpp>
#include <string_view>
#include <chrono>

#include "content_type.h"
#include "error_response.h"
#include "endpoint.h"
#include "make_response.h"
#include "../http_server/http_server.h"
#include "../json/tag_invoke_model.h"
#include "../json/tag_invoke_db.h"
#include "../util/util.h"
#include "../app/application.h"
#include "../db/postgres.h"
#include "../app/use_cases_impl.h"

namespace http_handler {
namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;

using namespace std::chrono_literals;
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

struct Params {
    Params() = delete;
    static const inline std::string_view START = "start";
    static const inline std::string_view MAX_ITEMS = "maxItems";
};

struct Restrictions {
    Restrictions() = delete;
    static const inline std::int32_t RECORD_MAX_ITEMS = 100;
};

class ApiHandler {
public:
    explicit ApiHandler(app::Application& app, data_base::postgres::Database& db): app_(app), db_(db) {}

    ApiHandler(const ApiHandler&) = delete;
    ApiHandler& operator=(const ApiHandler&) = delete;

    static bool IsAPIRequest(const StringRequest& req);

    StringResponse HandleApiRequest(const StringRequest& req);

private:
    StringResponse RequestForListPlayers(const StringRequest& req);
    static std::optional<app::Token> TryExtractToken(const StringRequest& req);
    StringResponse ExecuteAuthorized(const StringRequest& req,
                                     const std::function<StringResponse(std::shared_ptr<app::Player>& player)>& action);
    StringResponse RequestToJoin(const StringRequest& req);
    StringResponse RequestToMaps(const StringRequest& req, std::string & decoded_target);
    StringResponse RequestToState(const StringRequest& req);
    StringResponse RequestToAction(const StringRequest& req);
    StringResponse RequestToTick(const StringRequest& req);
    StringResponse RequestToRecords(const StringRequest& req, std::string& decoded_target);
private:
    static std::pair<std::int32_t, std::int32_t> GetUriRecordsParams(std::string& decoded_target);

private:
    app::Application& app_;
    data_base::postgres::Database& db_;
    app::UseCasesImpl use_cases_{db_.GetUnitOfWorkFactory()};
    bool is_first_tick = false;
};
}  // namespace http_handler