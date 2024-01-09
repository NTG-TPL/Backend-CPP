#ifdef WIN32
#include <sdkddkver.h>
#endif

#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <mutex>
#include <sstream>
#include <syncstream>
#include <unordered_map>
#include <thread>
#include <atomic>

class ThreadChecker {
public:
    explicit ThreadChecker(std::atomic_int& counter)
        : counter_{counter} {
    }

    ThreadChecker(const ThreadChecker&) = delete;
    ThreadChecker& operator=(const ThreadChecker&) = delete;

    ~ThreadChecker() {
        // assert выстрелит, если между вызовом конструктора и деструктора
        // значение expected_counter_ изменится
        assert(expected_counter_ == counter_);
    }

private:
    std::atomic_int& counter_;
    int expected_counter_ = ++counter_;
};

// Запускает функцию fn на n потоках, включая текущий
template <typename Fn>
void RunWorkers(unsigned n, const Fn& fn) {
    n = std::max(1u, n);
    std::vector<std::jthread> workers;
    workers.reserve(n - 1);
    // Запускаем n-1 рабочих потоков, выполняющих функцию fn
    while (--n) {
        workers.emplace_back(fn);
    }
    fn();
}

namespace net = boost::asio;
namespace sys = boost::system;
namespace ph = std::placeholders;
using namespace std::chrono;
using namespace std::literals;
using Timer = net::steady_timer;

class Hamburger {
public:
    [[nodiscard]] bool IsCutletRoasted() const {
        return cutlet_roasted_;
    }
    void SetCutletRoasted() {
        if (IsCutletRoasted()) {  // Котлету можно жарить только один раз
            throw std::logic_error("Cutlet has been roasted already"s);
        }
        cutlet_roasted_ = true;
    }

    [[nodiscard]] bool HasOnion() const {
        return has_onion_;
    }
    // Добавляем лук
    void AddOnion() {
        if (IsPacked()) {  // Если гамбургер упакован, класть лук в него нельзя
            throw std::logic_error("Hamburger has been packed already"s);
        }
        AssureCutletRoasted();  // Лук разрешается класть лишь после прожаривания котлеты
        has_onion_ = true;
    }

    [[nodiscard]] bool IsPacked() const {
        return is_packed_;
    }
    void Pack() {
        AssureCutletRoasted();  // Нельзя упаковывать гамбургер, если котлета не прожарена
        is_packed_ = true;
    }

private:
    // Убеждаемся, что котлета прожарена
    void AssureCutletRoasted() const {
        if (!cutlet_roasted_) {
            throw std::logic_error("Bread has not been roasted yet"s);
        }
    }

    bool cutlet_roasted_ = false;  // Обжарена ли котлета?
    bool has_onion_ = false;       // Есть ли лук?
    bool is_packed_ = false;       // Упакован ли гамбургер?
};

std::ostream& operator<<(std::ostream& os, const Hamburger& h) {
    return os << "Hamburger: "sv << (h.IsCutletRoasted() ? "roasted cutlet"sv : " raw cutlet"sv)
              << (h.HasOnion() ? ", onion"sv : ""sv)
              << (h.IsPacked() ? ", packed"sv : ", not packed"sv);
}

class Logger {
public:
    explicit Logger(std::string id)
        : id_(std::move(id)) {
    }

    void LogMessage(std::string_view message) const {
        std::osyncstream os{std::cout};
        os << id_ << "> ["sv << duration<double>(steady_clock::now() - start_time_).count()
           << "s] "sv << message << std::endl;
    }

private:
    std::string id_;
    steady_clock::time_point start_time_{steady_clock::now()};
};

// Функция, которая будет вызвана по окончании обработки заказа
using OrderHandler = std::function<void(sys::error_code ec, int id, Hamburger* hamburger)>;

class Order : public std::enable_shared_from_this<Order> {
public:
    Order(net::io_context& io, int id, bool with_onion, OrderHandler handler)
        : io_{io}
        , id_{id}
        , with_onion_{with_onion}
        , handler_{std::move(handler)} {
    }

    // Запускает асинхронное выполнение заказа
    void Execute() {
        logger_.LogMessage("Order has been started."sv);
        RoastCutlet();
        if (with_onion_) {
            MarinadeOnion();
        }
    }
private:
    void RoastCutlet() {
        logger_.LogMessage("Start roasting cutlet"sv);
        roast_timer_.async_wait(
            // OnRoasted будет вызван последовательным исполнителем strand_
            net::bind_executor(strand_, [self = shared_from_this()](sys::error_code ec) {
              self->OnRoasted(ec);
            }));
    }

    void OnRoasted(sys::error_code ec) {
        ThreadChecker checker{counter_};
        if (ec) {
            logger_.LogMessage("Roast error : "s + ec.what());
        } else {
            logger_.LogMessage("Cutlet has been roasted."sv);
            hamburger_.SetCutletRoasted();
        }
        CheckReadiness(ec);
    }

    void MarinadeOnion() {
        logger_.LogMessage("Start marinading onion"sv);
        marinade_timer_.async_wait(
            // OnOnionMarinaded будет вызван последовательным исполнителем strand_
            net::bind_executor(strand_, [self = shared_from_this()](sys::error_code ec) {
              self->OnOnionMarinaded(ec);
            }));
    }

    void OnOnionMarinaded(sys::error_code ec) {
        ThreadChecker checker{counter_};
        if (ec) {
            logger_.LogMessage("Marinade onion error: "s + ec.what());
        } else {
            logger_.LogMessage("Onion has been marinaded."sv);
            onion_marinaded_ = true;
        }
        CheckReadiness(ec);
    }
    void CheckReadiness(sys::error_code ec) {
        if (delivered_) {
            // Выходим, если заказ уже доставлен либо клиента уведомили об ошибке
            return;
        }
        if (ec) {
            // В случае ошибки уведомляем клиента о невозможности выполнить заказ
            return Deliver(ec);
        }

        // Самое время добавить лук
        if (CanAddOnion()) {
            logger_.LogMessage("Add onion"sv);
            hamburger_.AddOnion();
        }

        // Если все компоненты гамбургера готовы, упаковываем его
        if (IsReadyToPack()) {
            Pack();
        }
    }

    void Deliver(sys::error_code ec) {
        // Защита заказа от повторной доставки
        delivered_ = true;
        // Доставляем гамбургер в случае успеха либо nullptr, если возникла ошибка
        handler_(ec, id_, ec ? nullptr : &hamburger_);
    }

    [[nodiscard]] bool CanAddOnion() const {
        // Лук можно добавить, если котлета обжарена, лук замаринован, но пока не добавлен
        return hamburger_.IsCutletRoasted() && onion_marinaded_ && !hamburger_.HasOnion();
    }

    [[nodiscard]] bool IsReadyToPack() const {
        // Если котлета обжарена и лук добавлен, как просили, гамбургер можно упаковывать
        return hamburger_.IsCutletRoasted() && (!with_onion_ || hamburger_.HasOnion());
    }

    void Pack() {
        logger_.LogMessage("Packing"sv);
        // Просто потребляем ресурсы процессора в течение 0,5 с.
        auto start = steady_clock::now();
        while (steady_clock::now() - start < 500ms) {}
        hamburger_.Pack();
        logger_.LogMessage("Packed"sv);
        Deliver({});
    }
private:
    net::io_context& io_;
    net::strand<net::io_context::executor_type> strand_{net::make_strand(io_)};
    Timer roast_timer_{io_, 1s}; // 1s
    Timer marinade_timer_{io_, 2s}; // 2s

    Hamburger hamburger_;
    bool onion_marinaded_ = false;

    int id_;
    bool with_onion_;
    OrderHandler handler_;
    Logger logger_{std::to_string(id_)};
    bool delivered_ = false; // Заказ доставлен?

    std::atomic_int counter_{0};

};

class Restaurant {
public:
    explicit Restaurant(net::io_context& io)
        : io_(io) {
    }

    int MakeHamburger(bool with_onion, OrderHandler handler) {
        const int order_id = ++next_order_id_;
        std::make_shared<Order>(io_, order_id, with_onion, std::move(handler))->Execute();
        return order_id;
    }

private:
    net::io_context& io_;
    int next_order_id_ = 0;
};

//int main() {
//
//    const unsigned num_workers = 4;
//    // Сообщаем io_context о количестве потоков, которые будут одновременно вызывать метод run
//    net::io_context io(num_workers);
////    net::io_context io;
//
//    Restaurant restaurant{io};
//
//    Logger logger{"main"s};
//
//    struct OrderResult {
//        sys::error_code ec;
//        Hamburger hamburger;
//    };
//
//    std::unordered_map<int, OrderResult> orders;
//    auto handle_result = [&orders](sys::error_code ec, int id, Hamburger* h) {
//        orders.emplace(id, OrderResult{ec, ec ? Hamburger{} : *h});
//    };
//
//    const int id1 = restaurant.MakeHamburger(false, handle_result);
//    const int id2 = restaurant.MakeHamburger(true, handle_result);
//
//    // До вызова io.run() никакие заказы не выполняются
//    assert(orders.empty());
//    RunWorkers(num_workers, [&io] {
//      io.run();
//    });
////    io.run();
//
//    // После вызова io.run() все заказы быть выполнены
//    assert(orders.size() == 2u);
//    {
//        // Проверяем заказ без лука
//        const auto& o = orders.at(id1);
//        assert(!o.ec);
//        assert(o.hamburger.IsCutletRoasted());
//        assert(o.hamburger.IsPacked());
//        assert(!o.hamburger.HasOnion());
//    }
//    {
//        // Проверяем заказ с луком
//        const auto& o = orders.at(id2);
//        assert(!o.ec);
//        assert(o.hamburger.IsCutletRoasted());
//        assert(o.hamburger.IsPacked());
//        assert(o.hamburger.HasOnion());
//    }
//}

int main() {
    using osync = std::osyncstream;

    net::io_context io;
    auto strand1 = net::make_strand(io);

    net::post(strand1, [strand1] {  // (1)
      net::post(strand1, [] {     // (2)
        osync(std::cout) << 'A';
      });
      net::dispatch(strand1, [] {  // (3)
        osync(std::cout) << 'B';
      });
      osync(std::cout) << 'C';
    });

    auto strand2 = net::make_strand(io);
    // Эти функции выполняются в strand2
    net::post(strand2, [strand2] {  // (4)
      net::post(strand2, [] {     // (5)
        osync(std::cout) << 'D';
      });
      net::dispatch(strand2, [] {  // (6)
        osync(std::cout) << 'E';
      });
      osync(std::cout) << 'F';
    });

    RunWorkers(2, [&io] {
      io.run();
    });
}
