#include "sdk.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>

#include <iostream>
#include <thread>

#include "json/json_loader.h"
#include "request_handler/request_handler.h"
#include "request_handler/ticker.h"
#include "logger/logger.h"
#include "parse/parse.h"
#include "infrastructure/serializing_listener.h"

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

} // namespace

const std::chrono::milliseconds DEFAULT_TICK = 50ms;

int main(int argc, const char* argv[]) {

    parse::Args args;
    try {
        if (auto optional_args = parse::ParseCommandLine(argc, argv)) {
            args = optional_args.value();
        }else {
            return EXIT_FAILURE;
        }
    } catch (const std::exception& e) {
        std::cout << "Parse arguments failure. " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Инициализация Logger
    server_logging::Logger::Init();

    try {
        // 1. Загружаем карту из файла и построить модель игры
        fs::path config_file = args.config;
        fs::path static_files_root = args.www_root;
        app::Application app(config_file);
        infrastructure::SerializingListener listener(app, args.state_file,
                                                     std::chrono::milliseconds{args.save_state_period});
        app.SetApplicationListener(std::make_shared<infrastructure::SerializingListener>(listener));
        listener.Load();

        // 2. Инициализируем io_context
        const unsigned num_threads = std::thread::hardware_concurrency();
        net::io_context ioc(num_threads);
        // strand для выполнения запросов к API
        auto api_strand = net::make_strand(ioc);

        std::shared_ptr<http_handler::Ticker> ticker;
        if(!args.tick_period.has_value()) {
            ticker = std::make_shared<http_handler::Ticker>(api_strand, DEFAULT_TICK, [&app](std::chrono::milliseconds delta) {
                app.Tick(delta);
            });
            ticker->Start();
        }else {
            app.SetTickMode(true);
        }
        app.SetRandomSpawm(args.randomize_spawn);

        // 3. Добавляем асинхронный обработчик сигналов SIGINT и SIGTERM
        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const sys::error_code& ec, [[maybe_unused]] int signal_number) {
          if (!ec) {
              ioc.stop();
          }
        });

        // 4. Создаём обработчик HTTP-запросов и связываем его с приложением
        auto handler = std::make_shared<http_handler::RequestHandler>(static_files_root, api_strand, app);
        // 4.1 Использование паттерна 'Декоратор', чтобы залогировать получение запросов и формирование ответов
        server_logging::LoggingRequestHandler logging_handler{(*handler)};


        // 5. Запустить обработчик HTTP-запросов, делегируя их обработчику запросов
        const auto address = net::ip::make_address("0.0.0.0");
        constexpr net::ip::port_type port = 8080;
        http_server::ServeHttp(ioc, {address, port}, [&logging_handler](auto&& endpoint, auto&& req, auto&& send) {
            logging_handler(std::forward<decltype(endpoint)>(endpoint), std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
        });

        // Эта надпись сообщает тестам о том, что сервер запущен и готов обрабатывать запросы
        server_logging::Logger::LogStart(address, port);

        // 6. Запускаем обработку асинхронных операций
        RunWorkers(std::max(1u, num_threads), [&ioc] {
            ioc.run();
        });
        listener.Save();
    } catch (const std::exception& ex) {
        server_logging::Logger::LogExit(ex);
        return EXIT_FAILURE;
    }

    server_logging::Logger::LogExit(0);
    return 0;
}
