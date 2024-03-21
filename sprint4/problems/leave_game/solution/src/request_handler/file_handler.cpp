#include "file_handler.h"

namespace http_handler {
VariantResponse FileHandler::HandleFileResponse(const StringRequest& req){

    const auto text_response = [&req](http::status status, std::string_view text, std::string_view content_type) {
        return MakeStringResponse(status, text, req.version(), req.keep_alive(), content_type);
    };

    std::filesystem::path decoded_target = util::UrlDecode(std::string{req.target()});
    if (decoded_target == EndPoint::EMPTY){
        decoded_target = EndPoint::INDEX;
    }

    std::filesystem::path file_path = root_path_;
    file_path += decoded_target;
    if(!util::IsSubPath(file_path, root_path_)){
        return text_response(http::status::method_not_allowed, ErrorResponse::BAD_REQ(), ContentType::TEXT_PLAIN);
    }

    if (!std::filesystem::exists(file_path) ||
        !std::filesystem::is_regular_file(file_path)) {
        return text_response(http::status::not_found, ErrorResponse::FILE_NOT_FOUND, ContentType::TEXT_PLAIN);
    }

    http::file_body::value_type file;

    if (sys::error_code ec; file.open(file_path.c_str(), beast::file_mode::read, ec), ec) {
        return text_response(http::status::internal_server_error,
                             ErrorResponse::SERVER_ERROR("Failed to open file: " + file_path.string()),
                             ContentType::TEXT_PLAIN);
    }

    std::string extension = file_path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](unsigned char c) {return std::tolower(c);});

    std::string_view content_type = ContentType::EXTENSION.contains(extension) ? ContentType::EXTENSION.at(extension) : ContentType::UNKNOWN;

    return MakeFileResponse(http::status::ok, file, req.version(), req.keep_alive(), content_type);
}
}  // namespace http_handler