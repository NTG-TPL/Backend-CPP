#include <catch2/catch_test_macros.hpp>
#include "../src/model/model.h"

SCENARIO("Loot") {
    using namespace model;
    using namespace std::string_literals;

    GIVEN("a Loot type empty") {
        LootType loot_type;
        CHECK(loot_type.name == std::nullopt);
        CHECK(loot_type.type == std::nullopt);
        CHECK(loot_type.file == std::nullopt);
        CHECK(loot_type.rotation == std::nullopt);
        CHECK(loot_type.color == std::nullopt);
        CHECK(loot_type.scale == std::nullopt);
        CHECK(loot_type.value == 0);

        WHEN("loot type init") {
            loot_type.name = "key";
            loot_type.type = "type";
            loot_type.file = "file";
            loot_type.rotation = 90;
            loot_type.color = "color";
            loot_type.scale = 0.5;
            loot_type.value = 10;

            WHEN("get loot_type data"){
                double eps = 1e-10;
                THEN("check data"){
                    CHECK(loot_type.name == "key"s);
                    CHECK(loot_type.type == "type"s);
                    CHECK(loot_type.file == "file"s);
                    CHECK(loot_type.rotation == 90);
                    CHECK(loot_type.color == "color"s);
                    CHECK(loot_type.color == "color"s);
                    CHECK(std::abs(loot_type.scale.value() - 0.5) < eps);
                }
            }

            Loot loot(Loot::Id{1}, loot_type.value, {0.5, 0.7}, 1);

            WHEN("get loot data"){
                double eps = 1e-10;
                THEN("check data"){
                    CHECK(std::abs(loot.GetPosition().x - 0.5) < eps);
                    CHECK(std::abs(loot.GetPosition().y - 0.7) < eps);
                    CHECK(loot.GetType() == 1);
                }
            }
        }
    }
}