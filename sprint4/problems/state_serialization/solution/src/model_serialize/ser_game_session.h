#pragma once
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>

#include "ser_dog.h"
#include "ser_loot.h"


namespace serialization {

    class GameSessionRepr {
    public:
        GameSessionRepr() = default;

        explicit GameSessionRepr(const GameSession& session):
                map_id(*session.GetMap().GetId()),
                base_interval_(session.GetLootTimeInterval().count()),
                probability_(session.GetLootProbability()),
                limit_(session.GetLimitPlayers()) {
            for(auto& [_, dog]: session.GetDogs()){
                dogs_.emplace_back(dog);
            }

            for(auto& [_, loot]: session.GetLoots()){
                loots_.emplace_back(loot);
            }
        }

        [[nodiscard]] model::GameSession Restore(const model::Game& game) const {
            using Time = loot_gen::LootGenerator::TimeInterval;
            auto map = game.FindMap(Map::Id{map_id});
            GameSession game_session(*map, loot_gen::LootGenerator{Time{base_interval_}, probability_}, limit_);

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
            ar& map_id;
            ar& base_interval_;
            ar& probability_;
            ar& limit_;
            ar& dogs_;
            ar& loots_;
        }

    private:
        std::string map_id;
        int64_t base_interval_{};
        double probability_{};
        size_t limit_{};
        std::vector<DogRepr> dogs_;
        std::vector<LootRepr> loots_;
    };

} // namespace serialization