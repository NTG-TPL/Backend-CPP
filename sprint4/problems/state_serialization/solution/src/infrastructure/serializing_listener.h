#pragma once

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <filesystem>
#include <utility>
#include <memory>

#include "../app/application.h"
#include "../app_serialize/ser_app.h"
#include "../logger/logger.h"

namespace infrastructure {
using milliseconds = std::chrono::milliseconds;
using namespace std::literals;

namespace fs = std::filesystem;
class SerializingListener: public app::ApplicationListener {
public:
    using InputArchive = boost::archive::text_iarchive;
    using OutputArchive = boost::archive::text_oarchive;

    SerializingListener(app::Application& application, fs::path state_file, milliseconds save_period):
            application_(application), state_file_(std::move(state_file)),
            save_period_(save_period), time_since_save_(0ms) {
    }

    void OnTick(std::chrono::milliseconds tick) override {
        time_since_save_ += tick;
        if (time_since_save_ >= save_period_) {
            /*Логирование*/{
                boost::json::object obj;
                obj["Time Since Save (ms)"] = time_since_save_.count();
                obj["Save period (ms)"] = save_period_.count();
                server_logging::Logger::LogInfo(obj, "serialization");
            }
            Save();
            time_since_save_ = 0ms;
        }
    }

    void Save() const {
        using namespace std::string_literals;
        if (state_file_.empty()){
            return;
        }

        std::filesystem::path temp_file = state_file_;
        temp_file += ".temp";
        std::ofstream state_strm(temp_file);

        if(!state_strm.is_open()) {
            throw std::logic_error("The "s + temp_file.string() + " file was not opened");
        }

        try {
            OutputArchive output_archive{state_strm};
            output_archive << serialization::ApplicationRepr{application_};
        } catch (const std::system_error& e) {
            server_logging::Logger::LogError(e.code(), "Serialization error "s + e.what());
            throw std::runtime_error("Save:: serialization error"s + e.what());
        } catch (const std::exception& e) {
            throw std::runtime_error("Save:: serialization error"s + e.what());
        }

        std::filesystem::rename(temp_file, state_file_);
    }

    void Load(const fs::path& config){
        using namespace std::string_literals;

        if (state_file_.empty() ||
            !std::filesystem::exists(state_file_)){
            return;
        }

        std::ifstream state_strm(state_file_);
        if(!state_strm.is_open()) {
            throw std::logic_error("The "s + state_file_.string() + " file was not opened");
        }

        try {
            InputArchive input_archive{state_strm};
            serialization::ApplicationRepr application_repr;
            input_archive >> application_repr;
            application_.Load(application_repr.Restore(config));
        } catch (const std::system_error& e) {
            server_logging::Logger::LogError(e.code(), "Serialization error "s + e.what());
            throw std::runtime_error("Load:: serialization error"s + e.what());
        } catch (const std::exception& e) {
            throw std::runtime_error("Load:: serialization error"s + e.what());
        }
    }

    [[nodiscard]] const fs::path& GetStateFilePath() const noexcept {
        return state_file_;
    }

private:
    app::Application& application_;
    fs::path state_file_;
    milliseconds save_period_;
    milliseconds time_since_save_;
};

}