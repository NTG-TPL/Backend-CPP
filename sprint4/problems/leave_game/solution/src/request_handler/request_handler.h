#pragma once

#include <boost/asio/strand.hpp>

#include <string_view>
#include <utility>

#include "api_handler.h"
#include "file_handler.h"

namespace http_handler {
namespace net = boost::asio;
class RequestHandler : public std::enable_shared_from_this<RequestHandler> {
public:
    using Strand = net::strand<net::io_context::executor_type>;

    RequestHandler(fs::path root, Strand api_strand, app::Application& app, data_base::postgres::Database& db)
            : root_{std::move(root)}
            , api_strand_{std::move(api_strand)}
            , app_(app),
              db_(db) {
        if (!std::filesystem::exists(root_)) {
            throw std::logic_error("path to static files not exist: " + root_.string());
        }
    }

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    template <typename Body, typename Allocator, typename Send>
    void operator()(const net::ip::tcp::endpoint& /* endpoint */, http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
        if(ApiHandler::IsAPIRequest(req)){
            auto handle = [self = shared_from_this(), send, handler = std::make_shared<ApiHandler>(app_, db_), req] {
                    assert(self->api_strand_.running_in_this_thread());
                    send(std::move(handler->HandleApiRequest(req)));
            };
            return net::dispatch(api_strand_, handle);
        }else {
            FileHandler handler(root_);
            return std::visit(
                    [&send](auto&& result) {
                        send(std::move(std::forward<decltype(result)>(result)));
                    },
                    handler.HandleFileResponse(req)
            );
        }
    }

private:
    const fs::path root_;
    Strand api_strand_;
    app::Application& app_;
    data_base::postgres::Database& db_;

};
}  // namespace http_handler