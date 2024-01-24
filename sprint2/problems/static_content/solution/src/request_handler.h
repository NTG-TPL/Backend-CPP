#pragma once

#include <string_view>

#include "file_handler.h"


namespace http_handler {

    class RequestHandler {
    public:
        RequestHandler(model::Game& game, fs::path path) : game_{game}, root_path_(std::filesystem::absolute(std::move(path))) {
            if (!std::filesystem::exists(root_path_)) {
                throw std::logic_error("path to static files not exist");
            }
        }

        RequestHandler(const RequestHandler&) = delete;
        RequestHandler& operator=(const RequestHandler&) = delete;

        template <typename Body, typename Allocator, typename Send>
        void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
            if(ApiHandler::IsAPIRequest(req)){
                ApiHandler handler(game_);
                send(handler.HandleApiRequest(req));
            }else {
                FileHandler handler(root_path_);
                return std::visit(
                        [&send](auto&& result) {
                            send(std::forward<decltype(result)>(result));
                        },
                        handler.HandleFileResponse(req)
                );
            }
        }

    private:
        model::Game& game_;
        const fs::path root_path_;
    };
}  // namespace http_handler