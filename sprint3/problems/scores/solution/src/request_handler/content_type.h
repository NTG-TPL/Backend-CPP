#pragma once

#include <boost/beast/http.hpp>

#include <string_view>
#include <unordered_map>
#include <variant>

namespace http_handler {
using namespace std::string_view_literals;

/**
 * Структура для хранения названий расширений MIME-типов
 */
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

/**
 * Структура для хранения названий MIME-типов
 */
struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TEXT_HTML = "text/html"sv;                // .htm, .html
    constexpr static std::string_view TEXT_CSS = "text/css"sv;                  // .css
    constexpr static std::string_view TEXT_PLAIN = "text/plain"sv;              // .txt
    constexpr static std::string_view TEXT_JS = "text/javascript"sv;            // .js
    constexpr static std::string_view APPLICATION_JSON = "application/json"sv;  // .json
    constexpr static std::string_view APPLICATION_XML = "application/xml"sv;    // .xml
    constexpr static std::string_view IMAGE_PNG = "image/png"sv;                // .png
    constexpr static std::string_view IMAGE_JPEG = "image/jpeg"sv;              // .jpg, .jpe, .jpeg
    constexpr static std::string_view IMAGE_GIF = "image/gif"sv;                // .gif
    constexpr static std::string_view IMAGE_BMP = "image/bmp"sv;                // .bmp
    constexpr static std::string_view IMAGE_ICO = "image/vnd.microsoft.icon"sv; // .ico
    constexpr static std::string_view IMAGE_TIFF = "image/tiff"sv;              // .tiff, .tif
    constexpr static std::string_view IMAGE_SVG = "image/svg+xml"sv;            // .svg, .svgz
    constexpr static std::string_view AUDIO_MP3 = "audio/mpeg"sv;               // .mp3
    constexpr static std::string_view UNKNOWN = "application/octet-stream"sv;   //

    static std::unordered_map<std::string_view, std::string_view> InitExtension() {
        return {{FileExtension::HTML,    ContentType::TEXT_HTML},
                {FileExtension::HTM,     ContentType::TEXT_HTML},
                {FileExtension::CSS,     ContentType::TEXT_CSS},
                {FileExtension::TXT,     ContentType::TEXT_PLAIN},
                {FileExtension::JS,      ContentType::TEXT_JS},
                {FileExtension::JSON,    ContentType::APPLICATION_JSON},
                {FileExtension::XML,     ContentType::APPLICATION_XML},
                {FileExtension::PNG,     ContentType::IMAGE_PNG},
                {FileExtension::JPG,     ContentType::IMAGE_JPEG},
                {FileExtension::JPE,     ContentType::IMAGE_JPEG},
                {FileExtension::JPEG,    ContentType::IMAGE_JPEG},
                {FileExtension::GIF,     ContentType::IMAGE_GIF},
                {FileExtension::BMP,     ContentType::IMAGE_BMP},
                {FileExtension::ICO,     ContentType::IMAGE_ICO},
                {FileExtension::TIFF,    ContentType::IMAGE_TIFF},
                {FileExtension::TIF,     ContentType::IMAGE_TIFF},
                {FileExtension::SVG,     ContentType::IMAGE_SVG},
                {FileExtension::SVGZ,    ContentType::IMAGE_SVG},
                {FileExtension::MP3,     ContentType::AUDIO_MP3},
                {FileExtension::UNKNOWN, ContentType::UNKNOWN}
        };
    }

    static inline const std::unordered_map<std::string_view, std::string_view> EXTENSION = InitExtension();
};
}  // namespace http_handler
