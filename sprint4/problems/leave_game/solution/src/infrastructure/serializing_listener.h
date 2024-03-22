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

    void OnTick(std::chrono::milliseconds tick) override;
    void Save() const;
    void Load();

    [[nodiscard]] const fs::path& GetStateFilePath() const noexcept;

private:
    app::Application& application_;
    fs::path state_file_;
    milliseconds save_period_;
    milliseconds time_since_save_;
};

} // namespace infrastructure