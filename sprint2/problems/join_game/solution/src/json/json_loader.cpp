#include "json_loader.h"

namespace json_loader {
namespace json = boost::json;

model::Game LoadGame(const std::filesystem::path& json_path) {
    using namespace std::literals;
    using namespace model;
    std::ifstream input(json_path);
    if(!input.is_open()){
        throw std::runtime_error("The json file \"" + json_path.string() + "\" is not open");
    }

    const std::string content((std::istreambuf_iterator<char>(input)), (std::istreambuf_iterator<char>()));
    input.close();

    boost::json::error_code ec;
    auto json = boost::json::parse(content, ec);
    if(ec){
        throw std::runtime_error("The json file is not parse " + ec.what());
    }

    model::Game game;
    auto maps = json.as_object().at("maps").as_array();
    for (auto& map: maps) {
        game.AddMap(boost::json::value_to<model::Map>(map));
    }
    return game;
}
}  // namespace json_loader