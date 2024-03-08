#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_contains.hpp>
#include <sstream>
#include <iostream>

#include "../src/model/model.h"
#include "../src/model_serialize/model_serialize.h"
#include "../src/json/json_loader.h"

using namespace model;
using namespace serialization;
using namespace std::literals;
namespace fs = std::filesystem;

namespace {

using InputArchive = boost::archive::text_iarchive;
using OutputArchive = boost::archive::text_oarchive;

struct Fixture {
    std::stringstream strm;
    OutputArchive output_archive{strm};
};

}  // namespace

namespace model {
    bool operator == (const Dog& lhs, const Dog& rhs){
        return  *lhs.GetId() == *rhs.GetId() &&
                lhs.GetSpeed() == rhs.GetSpeed() &&
                lhs.GetName() == rhs.GetName() &&
                lhs.GetPosition() == rhs.GetPosition() &&
                lhs.GetScore() == rhs.GetScore() &&
                std::string{lhs.GetDirection()} == std::string{rhs.GetDirection()} &&
                lhs.GetWidth() == rhs.GetWidth() &&
                lhs.GetBag() == rhs.GetBag();
    }

    bool operator == (const Loot& lhs, const Loot& rhs){
        return  lhs.GetId() == rhs.GetId() &&
                lhs.GetType() == rhs.GetType() &&
                lhs.GetPosition() == rhs.GetPosition() &&
                lhs.GetValue() == rhs.GetValue() &&
                lhs.GetWidth() == rhs.GetWidth();
    }

    template<typename T>
    std::ostream& operator << (std::ostream& out, const model::Point<T>& value) {
        return out << "{" << value.x << ',' << value.y << "}";
    }

    template<typename T>
    std::ostream& operator << (std::ostream& out, const model::Velocity<T>& value) {
        return out << "{" << value.dx << ',' << value.dy << "}";
    }

    std::ostream& operator << (std::ostream& out, const model::Dog::Bag& bag) {
        out << "(";
        for (auto loot: bag) {
            return out << "{ id " << *loot.id << ", value " << loot.value << ", type " << loot.type << "} ";
        }
        out << ")";
        return out;
    }

    std::ostream& operator << (std::ostream& out, Dog const& value) {
        return out << "(" << "id " << *value.GetId() << ','
                   << " name " << value.GetName() << ','
                   << " pos " << value.GetPosition() << ','
                   << " speed " << value.GetSpeed() << ','
                   << " score " << value.GetScore() << ','
                   << " direction " << std::string{value.GetDirection()} << ','
                   << " width " << value.GetWidth() << ','
                   << " bag: " << value.GetBag() << ")";
    }

    std::ostream& operator << (std::ostream& out, Loot const& value) {
        return out << "(" << "id " << *value.GetId() << ','
                   << " type " << value.GetType() << ','
                   << " pos " << value.GetPosition() << ','
                   << " value " << value.GetValue() << ','
                   << " width " << value.GetWidth();
    }
}

namespace Catch {

template<typename T>
struct StringMaker<Velocity<T>> {
    static std::string convert(Velocity<T> const& value) {
        std::ostringstream tmp;
        tmp << value;
        return tmp.str();
    }
};

template<typename T>
struct StringMaker<Point<T>> {
    static std::string convert(Point<T> const& value) {
        std::ostringstream tmp;
        tmp << value;
        return tmp.str();
    }
};

}  // namespace Catch

template<class Object>
struct CommonMatcher : Catch::Matchers::MatcherGenericBase {
    explicit CommonMatcher(const Object& obj): obj_{obj} {}
    CommonMatcher(CommonMatcher&&) = default;

    template <typename OtherObject>
    bool match(OtherObject other) const {
        return obj_ == other;
    }

    std::string describe() const override {
        std::ostringstream tmp;
        tmp << obj_ << '\n';
        return tmp.str();
    }

private:
    const Object& obj_;
};

struct ExampleLoot {
    ExampleLoot() = delete;
    static inline const Loot loot1 = {Loot::Id{1}, 10, {50.5, 15.2}, 1};
    static inline const Loot loot2 = {Loot::Id{2}, 20, {0.0, 0.0}, 2};
    static inline const Loot loot3 = {Loot::Id{3}, 30, {-2, -2}, 3};
    static inline const Loot loot4 = {Loot::Id{4}, 20, {10, -2}, 1};
    static inline const Loot loot5 = {Loot::Id{5}, 40, {0.5, -0.5}, 2};
    static inline const Loot loot6 = {Loot::Id{6}, 50, {4, 4}, 3};
};

struct ExampleDogs {
    ExampleDogs() = delete;

    static Dog InitDog(Dog::Id id, const std::string& name,
                Point2d position, std::int32_t score,
                const std::vector<Loot>& bag, std::string_view direction,
                Velocity2d speed){
        Dog dog(id, name, position);
        dog.AddScore(score);
        dog.SetDirection(direction);
        dog.SetSpeed(speed);
        for (auto& loot: bag) {
            dog.PutToBag(FoundObject{FoundObject::Id {*loot.GetId()}, loot.GetType(), loot.GetValue()});
        }
        return dog;
    }

    static inline const Dog dog1 = InitDog(Dog::Id{1}, "Pluto"s, {42.2, 12.5}, 20,
                                           {ExampleLoot::loot1}, Movement::RIGHT, {2.3, 1});
    static inline const Dog dog2 = InitDog(Dog::Id{2}, "Вася"s, {0.0, 0.0}, 30,
                                           {ExampleLoot::loot1, ExampleLoot::loot2}, Movement::UP, {0, 1});
    static inline const Dog dog3 = InitDog(Dog::Id{3}, "Шарик"s, {-1, -1}, 40,
                                           {ExampleLoot::loot1, ExampleLoot::loot2, ExampleLoot::loot3}, Movement::DOWN, {-1, 1});
};

SCENARIO_METHOD(Fixture, "Point serialization") {
    GIVEN("A point") {
        const Point2d p{10, 20};
        WHEN("point is serialized") {
            output_archive << p;

            THEN("it is equal to point after serialization") {
                InputArchive input_archive{strm};
                Point2d restored_point;
                input_archive >> restored_point;
                CHECK(p == restored_point);
            }
        }
    }
}

SCENARIO_METHOD(Fixture, "Dog Serialization") {
    GIVEN("a dog") {
        const auto dog = [] {
            const LootType type;
            Dog dog{Dog::Id{42}, "Pluto"s, {42.2, 12.5}};
            dog.AddScore(42);
            dog.PutToBag({FoundObject::Id{10}, 2u, 10});
            dog.SetDirection(Movement::RIGHT);
            dog.SetSpeed(Velocity2d{2.3, -1.2});
            return dog;
        }();

        WHEN("dog is serialized") {
            {
                serialization::DogRepr repr{dog};
                output_archive << repr;
            }

            THEN("it can be deserialized") {
                InputArchive input_archive{strm};
                serialization::DogRepr repr;
                input_archive >> repr;
                const auto restored = repr.Restore();

                CHECK_THAT(dog, CommonMatcher(restored));
            }
        }
    }
}

SCENARIO_METHOD(Fixture, "Loot Serialization") {
    GIVEN("a loot") {
        const auto loot = [] {
            return Loot{Loot::Id{104}, 10, {42.2, 12.5}, 1};
        }();

        WHEN("loot is serialized") {
            {
                serialization::LootRepr repr{loot};
                output_archive << repr;
            }

            THEN("it can be deserialized") {
                InputArchive input_archive{strm};
                serialization::LootRepr repr;
                input_archive >> repr;
                const auto restored = repr.Restore();

                CHECK_THAT(loot, CommonMatcher(restored));
            }
        }
    }
}


SCENARIO_METHOD(Fixture, "GameSession Serialization") {
    fs::path test_config = "../../tests/test_config.json"s;
    REQUIRE(fs::exists(test_config));
    Game game = json_loader::LoadGame(test_config);

    GIVEN("a session") {
        const auto session = [&game] {
            auto map = game.FindMap(Map::Id{"map1"});
            CHECK(map != nullptr);
            GameSession session(*map, loot_gen::LootGenerator{30ms, 0.5}, 1000);
            session.AddDog(ExampleDogs::dog1);
            session.AddDog(ExampleDogs::dog2);
            session.AddDog(ExampleDogs::dog3);
            session.AddLoot(ExampleLoot::loot4);
            session.AddLoot(ExampleLoot::loot5);
            session.AddLoot(ExampleLoot::loot6);
            return session;
        }();

        WHEN("loot is serialized") {
            const GameSession& game_session = session;
            {
                serialization::GameSessionRepr repr{game_session};
                output_archive << repr;
            }

            THEN("it can be deserialized") {
                InputArchive input_archive{strm};
                serialization::GameSessionRepr repr;
                input_archive >> repr;
                const auto restored = repr.Restore(game);


                CHECK(restored.GetMapId() == game_session.GetMapId());
                CHECK(restored.GetLimitPlayers() == game_session.GetLimitPlayers());
                CHECK(restored.GetLootTimeInterval() == game_session.GetLootTimeInterval());
                CHECK(restored.GetLootProbability() == game_session.GetLootProbability());

                auto dogs_restored = restored.GetDogs();
                auto dogs = game_session.GetDogs();
                CHECK(dogs_restored.size() == dogs.size());
                for (auto& [id, dog]: dogs_restored) {
                    CHECK_THAT(dog, CommonMatcher(dogs.at(id)));
                }

                auto loots_restored = restored.GetLoots();
                const auto& loots = game_session.GetLoots();
                CHECK(loots_restored.size() == loots.size());
                for (auto& [id, loot]: loots_restored) {
                    CHECK_THAT(loot, CommonMatcher(loots_restored.at(id)));
                }
            }
        }
    }
}