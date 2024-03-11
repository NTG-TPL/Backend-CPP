#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http.hpp>
#include <boost/date_time.hpp>
#include <boost/json.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <exception>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace server_logging {
namespace json = boost::json;
namespace logging = boost::log;
namespace net = boost::asio;

BOOST_LOG_ATTRIBUTE_KEYWORD(additional_data, "AdditionalData", boost::json::value)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)

using namespace std::literals;
namespace http = boost::beast::http;
class Logger {
public:
    static void Init();
    static void LogExit(int code);
    static void LogExit(const std::exception& ex);
    static void LogStart(const net::ip::address& address, int port);
    static void LogError(const boost::system::error_code& ec, std::string_view where);
    static void LogInfo(boost::json::value const& data, std::string_view message);


    Logger() = default;
    Logger(const Logger&) = delete;
private:
    static void LogFormatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm);
};

template <class RequestHandler>
class LoggingRequestHandler {
public:
    explicit LoggingRequestHandler(RequestHandler& handler) : decorated_(handler) {}

    template <typename Body, typename Allocator, typename Send>
    void operator()(const boost::asio::ip::tcp::endpoint& endpoint, http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
        LogRequest(endpoint, req);
        decorated_(endpoint, std::move(req), [s = std::forward<Send>(send)](auto&& response) {
            std::chrono::high_resolution_clock timer;
            auto start = std::chrono::high_resolution_clock::now();

            const int code_result = response.result_int();
            const std::string content_type = static_cast<std::string>(response.at(http::field::content_type));

            s(response);

            auto stop = std::chrono::high_resolution_clock::now();
            auto response_time = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

            LoggingRequestHandler::LogResponse(response_time, code_result, content_type);
        });
    }

private:

    /**
     * Выполняет логирование запросов
     * @param endpoint обращение к маршруту HTTP методом
     * @param req Запрос
     */
    template <typename Body, typename Allocator>
    static void LogRequest(const boost::asio::ip::tcp::endpoint &endpoint , const http::request<Body, http::basic_fields<Allocator>>& req) {
        /** message — строка request received
            data    — объект с полями:
            ip      — IP-адрес клиента (полученный через endpoint.address().to_string()),
            URI     — запрошенный адрес,
            method  — использованный метод HTTP.
        */
        json::value value{{"ip"s,     endpoint.address().to_string()},
                          {"URI"s,    req.target()},
                          {"method"s, req.method_string()}};
        BOOST_LOG_TRIVIAL(info) << logging::add_value(additional_data, value) << "request received"sv;
    }

    /**
     * Выполняет логирование ответов на запросы
     * @param response_time время формирования ответа в миллисекундах (целое число).
     * @param code статус-код ответа, например, 200 (http::response<T>::result_int()).
     * @param content строка или null, если заголовок в ответе отсутствует.
     */
    static void LogResponse(std::size_t response_time, int code, const std::string& content) {
        json::value value = {{"response_time", response_time},
                             {"code",          code},
                             {"content_type",  (content.empty() ? "null" : content)}};
        BOOST_LOG_TRIVIAL(info) << boost::log::add_value(additional_data, value) << "response sent"sv;
    }
private:
    RequestHandler& decorated_;
};
} // namespace server_logging