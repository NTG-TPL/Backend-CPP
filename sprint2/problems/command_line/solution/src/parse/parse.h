#ifndef GAME_SERVER_PARSE_H
#define GAME_SERVER_PARSE_H
#include <boost/program_options.hpp>

#include <string_view>
#include <vector>
#include <string>
#include <optional>
#include <iostream>

namespace parse {
    using namespace std::literals;

    /**
     * Структура для хранения названий параметров командной строки
     */
    struct CommandLineOptions {
        CommandLineOptions() = delete;
        constexpr static inline const char * HELP_SHORT_HELP  = "help,h";
        constexpr static inline const char * TICK_PERIOD  = "tick-period,t";
        constexpr static inline const char * CONFIG = "config-file,c";
        constexpr static inline const char * WWW_ROOT  = "www-root,w";
        constexpr static inline const char * RANDOM_SPAWN  = "randomize-spawn-points";
    };

    /**
     * Структура для хранения аргументов командной строки
     */
    struct Args {
        std::optional<uint32_t> tick_period;
        std::string config;
        std::string www_root;
        bool randomize_spawn = false;
    };

    /**
     * Парсит командную строку
     * @param argc количество аргументов командной строки
     * @param argv массив указателей на нуль-терминированные строки, содержащие параметры командной строки
     * @return набор параметров
     */
    [[nodiscard]] std::optional<Args> ParseCommandLine(int argc, const char* const argv[]) {
        namespace po = boost::program_options;
        using CLO = CommandLineOptions;

        po::options_description desc{"All options"s};
        Args args;
        int32_t tick_period = 0;
        desc.add_options()
                ("help,h", "produce help message")
                ("tick-period,t", po::value<int32_t>(&tick_period)->value_name("milliseconds"), "set tick period")
                ("config-file,c", po::value(&args.config)->value_name("file"), "set config file path")
                ("www-root,w", po::value(&args.www_root)->value_name("directory path"), "set static files root")
                ("randomize-spawn-points", po::value<bool>(&args.randomize_spawn), "spawn dogs at random positions");
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

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
}

#endif //GAME_SERVER_PARSE_H
