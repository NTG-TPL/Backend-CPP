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

        WHEN("loot type init") {
            loot_type.name = "key";
            loot_type.type = "type";
            loot_type.file = "file";
            loot_type.rotation = 90;
            loot_type.color = "color";
            loot_type.scale = 0.5;

            Loot loot(loot_type, {0.5, 0.7}, 1);

            WHEN("get loot data"){
                double eps = 1e-10;
                THEN("check data"){
                    auto type = loot.GetLootType();
                    CHECK(std::abs(loot.GetPosition().x - 0.5) < eps);
                    CHECK(std::abs(loot.GetPosition().y - 0.7) < eps);
                    CHECK(loot.GetTypeIndex() == 1);
                    CHECK(type.name == "key"s);
                    CHECK(type.type == "type"s);
                    CHECK(type.file == "file"s);
                    CHECK(type.rotation == 90);
                    CHECK(type.color == "color"s);
                    CHECK(std::abs(loot.GetLootType().scale.value() - 0.5) < eps);
                }
            }
        }
    }
}