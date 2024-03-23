#pragma once

#include <boost/json.hpp>
#include <filesystem>
#include <fstream>

#include "tag_invoke_model.h"

namespace json_loader {

model::Game LoadGame(const std::filesystem::path& json_path);

}  // namespace json_loader
