#include "sdk.h"
//
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <iostream>
#include <thread>
#include <filesystem>

#include "json_loader.h"
#include "request_handler.h"
#include "logger.h"

using namespace std::literals;
namespace net = boost::asio;
namespace sys = boost::system;
namespace fs  = std::filesystem;

namespace {

// Запускает функцию fn на num_threads потоках, включая текущий
template <typename Fn>
void RunWorkers(unsigned num_threads, const Fn& fn) {
    num_threads = std::max(1u, num_threads);
    std::vector<std::jthread> workers;
    workers.reserve(num_threads - 1);
    // Запускаем num_threads-1 рабочих потоков, выполняющих функцию fn
    while (--num_threads) {
        workers.emplace_back(fn);
    }
    fn();
}

}  // namespace

void ServerIsRunning(const boost::asio::ip::tcp::endpoint& endpoint, const std::string& text){
    boost::json::value js_port_address{{"port"s, endpoint.port()}, {"address"s, endpoint.address().to_string()}};
    BOOST_LOG_TRIVIAL(info) << boost::log::add_value(logs::additional_data, js_port_address) << text;
}

int main(int argc, const char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: game_server <game-config-json> <path-directory-content>"sv << std::endl;
        return EXIT_FAILURE;
    }

    // Инициализация Logger
    logs::Logger::Init();

    try {
        // 1. Загружаем карту из файла и построить модель игры
        model::Game game = json_loader::LoadGame(argv[1]);

        // 2. Инициализируем io_context
        const unsigned num_threads = std::thread::hardware_concurrency();
        net::io_context ioc(num_threads);

        // 3. Добавляем асинхронный обработчик сигналов SIGINT и SIGTERM
        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const sys::error_code& ec, [[maybe_unused]] int signal_number) {
          if (!ec) {
              ioc.stop();
          }
        });

        // 4. Создаём обработчик HTTP-запросов и связываем его с моделью игры
        http_handler::RequestHandler handler{game, argv[2]};
        // 4.1 Используйте паттерн 'Декоратор', чтобы залогировать получение запросов и формирование ответов
        logs::LoggingRequestHandler<http_handler::RequestHandler> logging_handler{handler};

        // 5. Запустить обработчик HTTP-запросов, делегируя их обработчику запросов
        const auto address = net::ip::make_address("0.0.0.0");
        constexpr net::ip::port_type port = 8080;
        http_server::ServeHttp(ioc, {address, port}, [&logging_handler](auto&& endpoint, auto&& req, auto&& send) {
            logging_handler(std::forward<decltype(endpoint)>(endpoint), std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
        });

        // Эта надпись сообщает тестам о том, что сервер запущен и готов обрабатывать запросы
        ServerIsRunning({address, port}, "server started"s);

        // 6. Запускаем обработку асинхронных операций
        RunWorkers(std::max(1u, num_threads), [&ioc] {
            ioc.run();
        });
    } catch (const std::exception& ex) {
        logs::Logger::LogExit(ex);
        return EXIT_FAILURE;
    }

    logs::Logger::LogExit(0);
    return 0;
}
