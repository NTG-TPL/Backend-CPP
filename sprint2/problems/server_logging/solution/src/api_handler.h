#ifndef GAME_SERVER_API_HANDLER_H
#define GAME_SERVER_API_HANDLER_H

#define BOOST_BEAST_USE_STD_STRING_VIEW
#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <string_view>
#include <filesystem>

#include "http_server.h"
#include "tag_invoke.h"

namespace http_handler {
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace json = boost::json;
    namespace fs = std::filesystem;
    namespace sys = boost::system;
    using namespace std::literals;

    using StringResponse = http::response<http::string_body>;
    using StringRequest = http::request<http::string_body>;

    struct EndPoint {
        EndPoint() = delete;
        constexpr const static std::string_view API = "/api/";
        constexpr const static std::string_view MAPS = "/api/v1/maps";
        constexpr const static std::string_view MAP = "/api/v1/maps/";
        constexpr const static std::string_view EMPTY = "/";
        constexpr const static std::string_view INDEX = "/index.html";
    };

    struct ErrorResponse {
        ErrorResponse() = delete;
        constexpr static std::string_view MAP_NOT_FOUND  = R"({"code": "mapNotFound", "message": "Map not found"})"sv;
        constexpr static std::string_view BAD_REQ        = R"({"code": "badRequest", "message": "Bad request"})"sv;
        constexpr static std::string_view FILE_NOT_FOUND = R"({"code": "NotFound", "message": "File not found"})"sv;
    };

    struct FileExtension {
        FileExtension() = delete;
        constexpr static std::string_view HTML    = ".html"sv;
        constexpr static std::string_view HTM     = ".htm"sv;
        constexpr static std::string_view CSS     = ".css"sv;
        constexpr static std::string_view TXT     = ".txt"sv;
        constexpr static std::string_view JS      = ".js"sv;
        constexpr static std::string_view JSON    = ".json"sv;
        constexpr static std::string_view XML     = ".xml"sv;
        constexpr static std::string_view PNG     = ".png"sv;
        constexpr static std::string_view JPG     = ".jpg"sv;
        constexpr static std::string_view JPE     = ".jpe"sv;
        constexpr static std::string_view JPEG    = ".jpeg"sv;
        constexpr static std::string_view GIF     = ".gif"sv;
        constexpr static std::string_view BMP     = ".bmp"sv;
        constexpr static std::string_view ICO     = ".ico"sv;
        constexpr static std::string_view TIFF    = ".tiff"sv;
        constexpr static std::string_view TIF     = ".tif"sv;
        constexpr static std::string_view SVG     = ".svg"sv;
        constexpr static std::string_view SVGZ    = ".svgz"sv;
        constexpr static std::string_view MP3     = ".mp3"sv;
        constexpr static std::string_view UNKNOWN = ""sv;
    };

    struct ContentType {
        ContentType() = delete;
        constexpr static std::string_view TEXT_HTML        = "text/html"sv;                // .htm, .html
        constexpr static std::string_view TEXT_CSS         = "text/css"sv;                 // .css
        constexpr static std::string_view TEXT_PLAIN       = "text/plain"sv;               // .txt
        constexpr static std::string_view TEXT_JS          = "text/javascript"sv;          // .js
        constexpr static std::string_view APPLICATION_JSON = "application/json"sv;         // .json
        constexpr static std::string_view APPLICATION_XML  = "application/xml"sv;          // .xml
        constexpr static std::string_view IMAGE_PNG        = "image/png"sv;                // .png
        constexpr static std::string_view IMAGE_JPEG       = "image/jpeg"sv;               // .jpg, .jpe, .jpeg
        constexpr static std::string_view IMAGE_GIF        = "image/gif"sv;                // .gif
        constexpr static std::string_view IMAGE_BMP        = "image/bmp"sv;                // .bmp
        constexpr static std::string_view IMAGE_ICO        = "image/vnd.microsoft.icon"sv; // .ico
        constexpr static std::string_view IMAGE_TIFF       = "image/tiff"sv;               // .tiff, .tif
        constexpr static std::string_view IMAGE_SVG        = "image/svg+xml"sv;            // .svg, .svgz
        constexpr static std::string_view AUDIO_MP3        = "audio/mpeg"sv;               // .mp3
        constexpr static std::string_view UNKNOWN          = "application/octet-stream"sv; //
        static const std::unordered_map<std::string_view, std::string_view> EXTENSION;
    };


    StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                      bool keep_alive, std::string_view content_type = ContentType::TEXT_HTML);

    // Декодирование url
    std::string UrlDecode(const std::string& encoded);

    class ApiHandler {
    public:
        explicit ApiHandler(model::Game& game): game_(game) {}

        ApiHandler(const ApiHandler&) = delete;
        ApiHandler& operator=(const ApiHandler&) = delete;

        static bool IsAPIRequest(const StringRequest& req);

        StringResponse HandleApiRequest(const StringRequest& req){

            const auto text_response = [&req](http::status status, std::string_view text) {
                return MakeStringResponse(status, text, req.version(), req.keep_alive(), ContentType::APPLICATION_JSON);
            };

            auto decoded_target = UrlDecode(std::string{req.target()});

            if (req.method() != http::verb::get && req.method() != http::verb::head) {
                return text_response(http::status::method_not_allowed, ErrorResponse::BAD_REQ);
            }

            if (!decoded_target.starts_with(EndPoint::MAPS)) {
                return text_response(http::status::bad_request, ErrorResponse::BAD_REQ);
            }

            if (decoded_target == EndPoint::MAPS) {
                json::array arr;
                for (const auto& map : game_.GetMaps()) {
                    json::value v = {{"id", *map.GetId()}, {"name", map.GetName()}};
                    arr.push_back(v);
                }
                json::value v = arr;
                return text_response(http::status::ok, std::string(json::serialize(v)));
            }

            if (decoded_target.starts_with(EndPoint::MAP)) {
                std::string id = std::string{decoded_target.substr(EndPoint::MAP.size())};
                auto map = game_.FindMap(model::Map::Id(id));
                if (map) {
                    json::value value = json::value_from(*map);
                    return text_response(http::status::ok, json::serialize(value));
                } else {
                    return text_response(http::status::not_found, ErrorResponse::MAP_NOT_FOUND);
                }
            }

            return text_response(http::status::method_not_allowed, "Invalid method"sv);
        }

    private:
        model::Game& game_;
    };
}
#endif //GAME_SERVER_API_HANDLER_H
