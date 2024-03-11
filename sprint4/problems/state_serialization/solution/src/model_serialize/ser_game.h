#pragma once

#include <filesystem>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/optional.hpp>

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

        explicit GameRepr(const model::Game& game) {
            for (auto& session: game.GetSessions()) {
                sessions_.push_back( (session != nullptr) ? boost::optional<GameSessionRepr>{*session} : boost::none);
            }
        }

        [[nodiscard]] model::Game Restore(const fs::path& config) const {
            if(!fs::exists(config)){
                throw std::runtime_error("Путь "s + config.string() + " Не существует."s);
            }
            Game game = json_loader::LoadGame(config);
            auto& maps = game.GetMaps();

            for (auto& session: sessions_) {
                if(session.has_value()){
                    game.AddSession(session->Restore(game));
                }else {
                    auto index = static_cast<std::int32_t>(GenerateInRange(0ul, maps.size() - 1));
                    game.AddFreeSession((maps.begin() + index)->get()->GetId());
                }
            }
            return game;
        }

        template <typename Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
            ar& sessions_;
        }

    private:
        std::vector<boost::optional<GameSessionRepr>> sessions_;
    };

}