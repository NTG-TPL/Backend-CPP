#pragma once
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

#include "ser_dog.h"
#include "ser_loot.h"

namespace serialization {

    class GameSessionRepr {
    public:
        GameSessionRepr() = default;

        explicit GameSessionRepr(const GameSession& session):
                id_(session.GetId()),
                map_id(*session.GetMap()->GetId()){
            for(auto& [_, dog]: session.GetDogs()){
                if(dog != nullptr){
                    dogs_.emplace_back(*dog);
                }
            }

            for(auto& [_, loot]: session.GetLoots()){
                loots_.emplace_back(loot);
            }
        }

        [[nodiscard]] model::GameSession Restore(const model::Game& game) const {
            using Time = loot_gen::LootGenerator::TimeInterval;
            // Перевод из секунд в миллисекунды
            auto ms = std::chrono::milliseconds(static_cast<size_t>(game.GetLootPeriod()*1000));
            GameSession game_session(id_, game.FindMap(Map::Id{map_id}),
                                     loot_gen::LootGenerator{ms, game.GetLootProbability()});

            for (auto& dog: dogs_) {
                game_session.AddDog(dog.Restore());
            }

            for (auto& loot: loots_) {
                game_session.AddLoot(loot.Restore());
            }

            return game_session;
        }

        template <typename Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
            ar&* id_;
            ar& map_id;
            ar& dogs_;
            ar& loots_;
        }

    private:

        GameSession::Id id_ = GameSession::Id{0u};
        std::string map_id;
        std::vector<DogRepr> dogs_;
        std::vector<LootRepr> loots_;
    };

} // namespace serialization