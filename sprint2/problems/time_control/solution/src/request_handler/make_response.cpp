#include "make_response.h"

namespace http_handler {
    StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                      bool keep_alive, std::string_view content_type,
                                      std::string_view cache_control, std::string_view allow) {
        StringResponse response(status, http_version);
        response.set(http::field::content_type, content_type);
        response.body() = body;
        response.content_length(body.size());
        response.keep_alive(keep_alive);
        if (!cache_control.empty()) {
            response.set(http::field::cache_control, cache_control);
        }
        if (!allow.empty()) {
            response.set(http::field::allow, allow);
        }
        return response;
    }

    StringResponse MakeTextResponse(const StringRequest& req, http::status status, std::string_view text,
                                                std::string_view cache_control, std::string_view allow) {
        return MakeStringResponse(status, text, req.version(), req.keep_alive(), ContentType::APPLICATION_JSON, cache_control, allow);
    }

    FileResponse MakeFileResponse(http::status status, http::file_body::value_type& body, unsigned http_version,
                                               bool keep_alive, std::string_view content_type) {
        FileResponse response(status, http_version);
        response.set(http::field::content_type, content_type);
        response.body() = std::move(body);
        response.prepare_payload();
        response.keep_alive(keep_alive);
        return response;
    }
}