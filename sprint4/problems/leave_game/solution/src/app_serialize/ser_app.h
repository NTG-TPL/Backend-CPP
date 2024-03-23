#pragma once
#include <filesystem>
#include <chrono>

#include "../app/application.h"
#include "../model_serialize/ser_game.h"
#include "ser_players.h"

namespace serialization {

    namespace fs = std::filesystem;
    class ApplicationRepr {
    public:
        ApplicationRepr() = default;

        explicit ApplicationRepr(const app::Application& application):
            game_(application.GetGameModel()),
            players_(application.GetPlayers()){
        }

        [[nodiscard]] app::Application Restore(const fs::path& config) const {
            if(!fs::exists(config)){
                throw std::runtime_error("Путь "s + config.string() + " Не существует."s);
            }
            Game game = game_.Restore(config);
            app::Players players = players_.Restore(game);
            return {config, std::move(game), std::move(players)};
        }

        template<typename Archive>
        void serialize(Archive &ar, [[maybe_unused]] const unsigned version) {
            ar & game_;
            ar & players_;
        }

    private:
        GameRepr game_;
        PlayersRepr players_;
    };

} // namespace serialization