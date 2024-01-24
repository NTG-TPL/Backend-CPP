#pragma once
#pragma once

#include <boost/json.hpp>
#include <string_view>
#include <filesystem>

#include "http_server.h"
#include "model.h"
#include "tag_invoke.h"

namespace http_handler {
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace json = boost::json;
    namespace fs = std::filesystem;
    using namespace std::literals;

    using StringResponse = http::response<http::string_body>;

    namespace {
        struct ErrorResponse {
            ErrorResponse() = delete;
            constexpr static std::string_view MAP_NOT_FOUND = R"({"code": "mapNotFound", "message": "Map not found"})"sv;
            constexpr static std::string_view BAD_REQ = R"({"code": "badRequest", "message": "Bad request"})"sv;
        };

        struct ContentType {
            ContentType() = delete;
            constexpr static std::string_view TEXT_HTML = "text/html"sv;
            constexpr static std::string_view APPLICATION_JSON = "application/json"sv;
        };

        struct EndPoint {
            EndPoint() = delete;
            constexpr const static std::string_view MAPS = "/api/v1/maps";
            constexpr const static std::string_view MAP = "/api/v1/maps/";
        };
    }  // namespace
    
    StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                      bool keep_alive, std::string_view content_type = ContentType::TEXT_HTML);

    class RequestHandler {
    public:
        explicit RequestHandler(model::Game& game) : game_{game} {}

        RequestHandler(const RequestHandler&) = delete;
        RequestHandler& operator=(const RequestHandler&) = delete;

        template <typename Body, typename Allocator, typename Send>
        void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
            const auto text_response = [&req, &send](http::status status, std::string_view text) {
                send(MakeStringResponse(status, text, req.version(), req.keep_alive(), ContentType::APPLICATION_JSON));
            };

            if (req.method_string() != "GET" && req.method_string() != "HEAD") {
                text_response(http::status::method_not_allowed, ErrorResponse::BAD_REQ);
                return;
            }

            const std::string_view  target = req.target();
            if (!target.starts_with(EndPoint::MAPS)) {
                text_response(http::status::bad_request, ErrorResponse::BAD_REQ);
                return;
            }

            if (target == EndPoint::MAPS) {
                json::array arr;
                for (const auto& map : game_.GetMaps()) {
                    json::value v = {{"id", *map.GetId()}, {"name", map.GetName()}};
                    arr.push_back(v);
                }
                json::value v = arr;
                text_response(http::status::ok, std::string(json::serialize(v)));
                return;
            }

            if (target.starts_with(EndPoint::MAP)) {
                std::string id = std::string{target.substr(EndPoint::MAP.size())};
                auto map = game_.FindMap(model::Map::Id(id));
                if (map) {
                    json::value v = json::value_from(*map);
                    text_response(http::status::ok, json::serialize(v));
                    return;
                } else {
                    text_response(http::status::not_found, ErrorResponse::MAP_NOT_FOUND);
                    return;
                }
            }

            text_response(http::status::method_not_allowed, "Invalid method"sv);
        }

    private:
        model::Game& game_;
    };
}  // namespace http_handler