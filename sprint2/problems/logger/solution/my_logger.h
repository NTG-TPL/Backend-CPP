#pragma once

#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <optional>
#include <mutex>
#include <thread>

using namespace std::literals;

#define LOG(...) Logger::GetInstance().Log(__VA_ARGS__)

class Logger {
    [[nodiscard]] auto GetTime() const {
        if (manual_ts_) {
            return *manual_ts_;
        }

        return std::chrono::system_clock::now();
    }

    [[nodiscard]] auto GetTimeStamp() const {
        const auto now = GetTime();
        const auto t_c = std::chrono::system_clock::to_time_t(now);
        return std::put_time(std::localtime(&t_c), "%F %T");
    }

    [[nodiscard]] std::string GetFileTimeStamp() const{
        const auto current_time = std::chrono::system_clock::to_time_t(GetTime());
        std::ostringstream ss;
        ss << "/var/log/sample_log_" << std::put_time(std::localtime(&current_time), "%Y_%m_%d") << ".log";
        return ss.str();
    }

    Logger() = default;
    Logger(const Logger&) = delete;

public:
    static Logger& GetInstance() {
        static Logger obj;
        return obj;
    }

    template<class... Ts>
    void Log(const Ts&... args) {
        std::scoped_lock lock{mutex_};
        std::ofstream log_file_{GetFileTimeStamp(), std::ios::app};
        log_file_ << GetTimeStamp() << ": ";
        ((log_file_ << args), ...);
        log_file_ << '\n';
    }

    void SetTimestamp(std::chrono::system_clock::time_point ts){
        std::scoped_lock ts_lock{mutex_};
        manual_ts_ = ts;
    }

private:
    std::mutex mutex_;
    std::optional<std::chrono::system_clock::time_point> manual_ts_;
};
