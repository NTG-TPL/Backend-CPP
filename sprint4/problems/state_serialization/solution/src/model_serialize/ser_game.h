#pragma once

#include <filesystem>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/optional.hpp>

#include "../model/game.h"
#include "../model/geom.h"
#include "../json/json_loader.h"
#include "ser_game_session.h"

namespace serialization {
    namespace fs = std::filesystem;
    using namespace std::string_literals;
    class GameRepr {
    public:
        GameRepr() = default;

        explicit GameRepr(const model::Game& game):
                period_(game.GetLootPeriod()),
                probability_(game.GetLootProbability()) {
            for (auto& session: game.GetSessions()) {
                sessions_.push_back( (session != nullptr) ? std::optional<GameSessionRepr>{*session} : std::nullopt);
            }
        }

        [[nodiscard]] model::Game Restore(const fs::path& config) const {
            if(!fs::exists(config)){
                throw std::runtime_error("Путь "s + config.string() + " Не существует."s);
            }
            Game game = json_loader::LoadGame(config);
            game.SetLootGeneratorConfig(period_, probability_);
            auto& maps = game.GetMaps();

            for (auto& session: sessions_) {
                if(session.has_value()){
                    game.AddSession(std::make_shared<GameSession>(session->Restore(game)));
                }else {
                    auto index = static_cast<std::int32_t>(GenerateInRange(0ul, maps.size() - 1));
                    game.AddFreeSession((maps.begin() + index)->GetId());
                }
            }
            return game;
        }

        template <typename Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
            ar& sessions_;
        }

    private:
        double period_{}, probability_{};
        std::vector<std::optional<GameSessionRepr>> sessions_;
    };

}