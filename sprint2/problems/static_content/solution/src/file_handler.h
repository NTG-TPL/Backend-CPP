#ifndef GAME_SERVER_FILE_HANDLER_H
#define GAME_SERVER_FILE_HANDLER_H

#include <variant>
#include "api_handler.h"

namespace http_handler {

    using FileResponse = http::response<http::file_body>;
    using VariantResponse = std::variant<StringResponse, FileResponse>;

    // Возвращает true, если каталог path содержится внутри base.
    bool IsSubPath(fs::path path, fs::path base);

    class FileHandler {
    public:
        explicit FileHandler(fs::path path): root_path_(std::move(path)) {}

        FileHandler(const FileHandler&) = delete;
        FileHandler& operator=(const FileHandler&) = delete;

        VariantResponse HandleFileResponse(const StringRequest& req);

    private:
        static FileResponse MakeFileResponse(http::status status, http::file_body::value_type& body, unsigned http_version,
                                      bool keep_alive, std::string_view content_type);
    private:
        const fs::path root_path_;
    };
}

#endif //GAME_SERVER_FILE_HANDLER_H
