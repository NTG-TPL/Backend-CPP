#include "logger.h"

namespace server_logging {
/**
 * Логирование выхода из программы
 * @param code статус-код {0 или 1}
 */
void Logger::LogExit(int code) {
    boost::json::object obj{{"code", code}};
    if (code == 1){
        BOOST_LOG_TRIVIAL(error) << boost::log::add_value(additional_data, obj) << "server exited"sv;
    }else if(code == 0){
        BOOST_LOG_TRIVIAL(info) << boost::log::add_value(additional_data, obj) << "server exited"sv;
    }
}

/**
 * Логирование выхода из программы при исключении
 * @param ex исключение
 */
void Logger::LogExit(const std::exception& ex) {
    boost::json::object obj{{"exception", ex.what()}};
    BOOST_LOG_TRIVIAL(error) << boost::log::add_value(additional_data, obj) << "server exited"sv;
}

void Logger::LogStart(const net::ip::address& address, int port) {
    boost::json::value js_port_address{{"port"s, port}, {"address"s, address.to_string()}};
    LogInfo(js_port_address, "server started"sv);
}

/**
 * Логирование исключений
 * @param ec исключение
 * @param where где допущено исключение
 */
void Logger::LogError(const boost::system::error_code& ec, const std::string_view where) {
    boost::json::object obj = {{"code", ec.value()},
                               {"text", ec.message()},
                               {"where", where}};
    BOOST_LOG_TRIVIAL(error) << boost::log::add_value(additional_data, obj) << "error"sv;
}

/**
 * Логирование сообщения
 * @param input_data данные
 * @param message сообщение
 */
void Logger::LogInfo(boost::json::value const& input_data, std::string_view message) {
    BOOST_LOG_TRIVIAL(info) << logging::add_value(additional_data, input_data) << message;
}

/**
 * Метод форматирование
 * @param rec сообщение журнала
 * @param strm поток
 */
void Logger::LogFormatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm) {
    auto ts = *rec[timestamp];
    strm << R"({"timestamp":")" << to_iso_extended_string(ts) << R"(",)";
    strm << R"("data":)" << rec[additional_data] << ","sv;
    strm << R"("message":")" << rec[boost::log::expressions::smessage] << R"("})";
}

/**
 * Инициализация параметров логирования
 */
void Logger::Init() {
    boost::log::add_common_attributes();
    logging::add_console_log(std::cout,
                             logging::keywords::format = &LogFormatter,
                             logging::keywords::auto_flush = true);
}
} // namespace server_logging