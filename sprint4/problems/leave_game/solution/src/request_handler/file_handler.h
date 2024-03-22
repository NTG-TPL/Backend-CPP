#pragma once

#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <string_view>
#include <variant>

#include "content_type.h"
#include "endpoint.h"
#include "error_response.h"
#include "make_response.h"
#include "../util/util.h"

namespace http_handler {
namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;
namespace fs = std::filesystem;
namespace sys = boost::system;
using namespace std::literals;

class FileHandler {
public:
    explicit FileHandler(fs::path path): root_path_(std::move(path)) {}

    FileHandler(const FileHandler&) = delete;
    FileHandler& operator=(const FileHandler&) = delete;

    VariantResponse HandleFileResponse(const StringRequest& req);

private:
    const fs::path root_path_;
};
}  // namespace http_handler