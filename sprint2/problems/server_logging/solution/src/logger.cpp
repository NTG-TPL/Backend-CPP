#include "logger.h"

namespace logs {

    void Logger::LogExit(int code) {
        boost::json::object obj{{"code", code}};
        if (code)
            BOOST_LOG_TRIVIAL(error) << boost::log::add_value(additional_data, obj) << "server exited"sv;
        else
            BOOST_LOG_TRIVIAL(info) << boost::log::add_value(additional_data, obj) << "server exited"sv;
    }

    void Logger::LogExit(const std::exception& ex) {
        boost::json::object obj{{"exception", ex.what()}};
        BOOST_LOG_TRIVIAL(error) << boost::log::add_value(additional_data, obj) << "server exited"sv;
    }

    void Logger::LogError(const boost::system::error_code& ec, const std::string_view where) {
        boost::json::object obj = {{"code", ec.value()},
                                   {"text", ec.message()},
                                   {"where", where}};
        BOOST_LOG_TRIVIAL(error) << boost::log::add_value(additional_data, obj) << "error"sv;
    }

    void Logger::LogFormatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm) {
        auto ts = *rec[timestamp];
        strm << R"({"timestamp":")" << to_iso_extended_string(ts) << R"(",)";
        strm << R"("data":)" << rec[additional_data] << ","sv;
        strm << R"("message":")" << rec[boost::log::expressions::smessage] << R"("})";
    }

    void Logger::Init() {
        boost::log::add_common_attributes();
        boost::log::add_console_log(
                std::clog,
                boost::log::keywords::format = &LogFormatter);
    }
}