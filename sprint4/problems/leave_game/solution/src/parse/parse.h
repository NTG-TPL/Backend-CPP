#pragma once

#include <boost/program_options.hpp>

#include <string_view>
#include <vector>
#include <string>
#include <optional>
#include <iostream>

namespace parse {
using namespace std::literals;
/**
 * Структура для хранения аргументов командной строки
 */
struct Args {
    std::optional<uint32_t> tick_period;
    std::string config;
    std::string www_root;
    bool randomize_spawn = false;
    std::string state_file;
    uint32_t save_state_period{0};
};

/**
 * Парсит командную строку
 * @param argc количество аргументов командной строки
 * @param argv массив указателей на нуль-терминированные строки, содержащие параметры командной строки
 * @return набор параметров
 */
[[nodiscard]] std::optional<Args> ParseCommandLine(int argc, const char* const argv[]) {
    namespace po = boost::program_options;

    po::options_description desc{"All options"s};
    Args args;
    uint32_t tick_period = 0;
    desc.add_options()
            ("help,h", "produce help message")
            ("tick-period,t", po::value<uint32_t>(&tick_period)->value_name("milliseconds"), "set tick period")
            ("config-file,c", po::value(&args.config)->value_name("file"), "set config file path")
            ("www-root,w", po::value(&args.www_root)->value_name("directory path"), "set static files root")
            ("randomize-spawn-points", "spawn dogs at random positions")
            ("state-file", po::value(&args.state_file)->value_name("file"), "set game save file")
            ("save-state-period", po::value<uint32_t>(&args.save_state_period)->value_name("milliseconds"), "set period for autosave");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    // Чтобы можно было указывать --randomize-spawn-points без параметров
    if(vm.contains("randomize-spawn-points")){
        args.randomize_spawn = true;
    }

    if (vm.contains("tick-period")) {
        args.tick_period = tick_period;
    }

    if (vm.contains("help")) {
        std::cout << desc;
        return std::nullopt;
    }

    // Проверяет наличие обязательных опций
    if (!vm.contains("config-file") || !vm.contains("www-root")) {
        throw std::runtime_error("Usage: game_server --config-file <config-path> --www-root <static-files-dir>");
    }

    return args;
}
} // namespace parse