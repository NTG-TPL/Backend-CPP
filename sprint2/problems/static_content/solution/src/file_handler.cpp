#include "file_handler.h"

namespace http_handler {

    bool IsSubPath(fs::path path, fs::path base) {
        // Приводим оба пути к каноничному виду (без . и ..)
        path = fs::weakly_canonical(path);
        base = fs::weakly_canonical(base);

        // Проверяем, что все компоненты base содержатся внутри path
        for (auto b = base.begin(), p = path.begin(); b != base.end(); ++b, ++p) {
            if (p == path.end() || *p != *b) {
                return false;
            }
        }
        return true;
    }

    VariantResponse FileHandler::HandleFileResponse(const StringRequest& req){

        const auto text_response = [&req](http::status status, std::string_view text, std::string_view content_type) {
            return MakeStringResponse(status, text, req.version(), req.keep_alive(), content_type);
        };

        std::filesystem::path decoded_target = UrlDecode(std::string{req.target()});
        if (decoded_target == EndPoint::EMPTY){
            decoded_target = EndPoint::INDEX;
        }

        std::filesystem::path file_path = root_path_;
        file_path += decoded_target;
        if(!IsSubPath(file_path, root_path_)){
            return text_response(http::status::method_not_allowed, ErrorResponse::BAD_REQ, ContentType::TEXT_PLAIN);
        }

        if (!std::filesystem::exists(file_path) ||
            !std::filesystem::is_regular_file(file_path)) {
            return text_response(http::status::not_found, ErrorResponse::FILE_NOT_FOUND, ContentType::TEXT_PLAIN);
        }

        http::file_body::value_type file;

        if (sys::error_code ec; file.open(file_path.c_str(), beast::file_mode::read, ec), ec) {
            std::cout << "Failed to open file "sv << file_path << std::endl;
            throw std::logic_error("Wrong open file");
        }

        std::string extension = file_path.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(),
                       [](unsigned char c) {return std::tolower(c);});

        std::string_view content_type = ContentType::EXTENSION.contains(extension) ? ContentType::EXTENSION.at(extension) : ContentType::UNKNOWN;

        return MakeFileResponse(http::status::ok, file, req.version(), req.keep_alive(), content_type);
    }

    FileResponse FileHandler::MakeFileResponse(http::status status, http::file_body::value_type& body, unsigned http_version,
        bool keep_alive, std::string_view content_type) {
        FileResponse response(status, http_version);
        response.set(http::field::content_type, content_type);
        response.body() = std::move(body);
        response.prepare_payload();
        response.keep_alive(keep_alive);
        return response;
    }
}