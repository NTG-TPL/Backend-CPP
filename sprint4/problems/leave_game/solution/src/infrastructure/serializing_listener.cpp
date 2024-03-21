#include "serializing_listener.h"

namespace infrastructure {

void SerializingListener::OnTick(std::chrono::milliseconds tick) {
    time_since_save_ += tick;
    if (time_since_save_ >= save_period_) {
    Save();
    time_since_save_ = 0ms;
    }
}

void SerializingListener::Save() const {
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

        /*Логирование*/{
            boost::json::object obj;
            obj["Time Since Save (ms)"] = time_since_save_.count();
            obj["Save period (ms)"] = save_period_.count();
            server_logging::Logger::LogInfo(obj, "serialization");
        }

    } catch (const std::system_error& e) {
        server_logging::Logger::LogError(e.code(), "Serialization error "s + e.what());
        throw std::runtime_error("Save:: serialization error"s + e.what());
    } catch (const std::exception& e) {
        throw std::runtime_error("Save:: serialization error"s + e.what());
    }

    std::filesystem::rename(temp_file, state_file_);
}

void SerializingListener::Load(){
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
        auto application = application_repr.Restore(application_.GetConfigFilePath());
        for (auto& listener: application_.GetApplicationListeners()) {
            if(listener){
                application.AddApplicationListener(listener);
            }
        }
        application_ = application;
    } catch (const std::system_error& e) {
        server_logging::Logger::LogError(e.code(), "Serialization error "s + e.what());
        throw std::runtime_error("Load:: serialization error"s + e.what());
    } catch (const std::exception& e) {
        throw std::runtime_error("Load:: serialization error"s + e.what());
    }
}

[[nodiscard]] const fs::path& SerializingListener::GetStateFilePath() const noexcept {
    return state_file_;
}

} // namespace infrastructure