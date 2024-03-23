#pragma once

#include "content_type.h"

namespace http_handler {
namespace beast = boost::beast;
namespace http = beast::http;

using StringResponse = http::response<http::string_body>;
using StringRequest = http::request<http::string_body>;
using FileResponse = http::response<http::file_body>;
using VariantResponse = std::variant<StringResponse, FileResponse>;

StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                  bool keep_alive, std::string_view content_type = ContentType::TEXT_HTML,
                                  std::string_view cache_control = "", std::string_view allow = "" );

StringResponse MakeTextResponse(const StringRequest& req, http::status status, std::string_view text,
                                std::string_view cache_control = std::string_view(), std::string_view allow = std::string_view());

FileResponse MakeFileResponse(http::status status, http::file_body::value_type& body, unsigned http_version,
                                     bool keep_alive, std::string_view content_type);
} // namespace http_handler