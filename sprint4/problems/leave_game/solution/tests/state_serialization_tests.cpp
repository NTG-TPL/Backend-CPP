#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_contains.hpp>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "../src/model/model.h"
#include "../src/serialize.h"
#include "../src/infrastructure/serializing_listener.h"

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

struct InitGame {
    std::stringstream strm;
    OutputArchive output_archive{strm};
    fs::path test_config = "../../tests/test_config.json"s;
    Game game = json_loader::LoadGame(test_config);
};

struct ForSerializingListener {
    std::chrono::milliseconds tick_period = 10ms;
    std::stringstream strm;
    OutputArchive output_archive{strm};
    fs::path test_config = "../../tests/test_config.json"s;
    fs::path state_save_test = "../../tests/state.save"s;
    std::chrono::milliseconds save_period = tick_period;
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

    bool operator == (const GameSession& lhs, const GameSession& rhs){
        const auto& lhs_dogs = lhs.GetDogs();
        const auto& rhs_dogs = rhs.GetDogs();
        const auto& lhs_loots = lhs.GetLoots();
        const auto& rhs_loots = rhs.GetLoots();

        bool check = *lhs.GetId() == *rhs.GetId() &&
                     *lhs.GetMapId() == *rhs.GetMapId() &&
                     lhs.GetLimitPlayers() == rhs.GetLimitPlayers() &&
                     lhs.GetLootTimeInterval() == rhs.GetLootTimeInterval() &&
                     lhs.GetLootProbability() == rhs.GetLootProbability() &&
                     lhs_dogs.size() == rhs_dogs.size() &&
                     lhs_loots.size() == rhs_loots.size();

        if(!check) {
            return false;
        }

        for (auto& [id, dog]: lhs_dogs) {
            if(!(*dog == *rhs_dogs.at(id))){
                return false;
            }
        }

        for (auto& [id, loot]: lhs_loots) {
            if(!(loot == rhs_loots.at(id))){
                return false;
            }
        }
        return check;
    }

    bool operator == (const Game& lhs, const Game& rhs){
        const auto& lhs_session = lhs.GetSessions();
        const auto& rhs_session = rhs.GetSessions();

        bool check = lhs.GetLootProbability() == rhs.GetLootProbability() &&
                lhs.GetLootPeriod() == rhs.GetLootPeriod() &&
                lhs_session.size() == rhs_session.size();

        if(!check){
            return false;
        }

        return std::equal(lhs_session.begin(), lhs_session.end(), rhs_session.begin(), rhs_session.end(), [](auto& lhs, auto& rhs){
            return ((lhs == nullptr) && (rhs == nullptr)) || ( ((lhs != nullptr) && (rhs != nullptr)) && (*lhs == *rhs) );
        });
    }

    bool operator == (const app::Player& lhs, const app::Player& rhs) {
        using namespace app;

        return lhs.GetId().ToString() == rhs.GetId().ToString() &&
                (*lhs.GetDog()->GetId() == *rhs.GetDog()->GetId()) &&
                (*lhs.GetSession()->GetId() == *rhs.GetSession()->GetId());
    }


    bool operator == (const app::Players& lhs, const app::Players& rhs) {
        using namespace app;

        const auto& lhs_tokens = lhs.GetPlayerTokens().GetTokenToPlayer(),
                    rhs_tokens = rhs.GetPlayerTokens().GetTokenToPlayer();
        const auto& lhs_player_list = lhs.GetList();
        const auto& rhs_player_list = rhs.GetList();

        if( (lhs_player_list.size() != rhs_player_list.size()) ||
            (lhs_tokens.size() != rhs_tokens.size()) ||
            (lhs_player_list.size() != lhs_tokens.size())){
            return false;
        }

        return std::ranges::all_of(lhs_tokens.cbegin(), lhs_tokens.cend(), [&rhs_tokens](const auto& el) {
            return rhs_tokens.contains(el.first) && (*rhs_tokens.at(el.first) == *el.second);
        });
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

    std::ostream& operator << (std::ostream& out, GameSession const& value) {
        out << "(" << "id " << *value.GetId() << ','
                   << " mapId " << *value.GetMapId() << ','
                   << " probability " << value.GetLootProbability() << ','
                   << " time_interval (ms) " << value.GetLootTimeInterval().count() << ','
                   << " limits " << value.GetLimitPlayers() << ','
                   << " dogs: ";

        const auto& dogs = value.GetDogs();
        out << "size " << dogs.size() << " {";
        for(const auto& [_, dog]: dogs){
            if(dog != nullptr){
                out << *dog;
            }else {
                out << "nullptr";
            }
            out << " ";
        }
        out << "}, ";

        out << "loots: ";
        const auto& loots = value.GetLoots();
        out << "size " << loots.size() << " {";
        for(const auto& [_, loot]: loots) {
            out << loot << " ";
        }
        out << "})\n";
        return out;
    }

    std::ostream& operator << (std::ostream& out, Game const& value) {
        out << "( probability " << value.GetLootProbability() << ','
            << " period (ms) " << value.GetLootPeriod() << ','
            << " sessions: ";

        auto sessions =  value.GetSessions();
        out << "size " << sessions.size() << "{";
        for (auto& session: sessions) {
            if(session != nullptr){
                out << *session;
            }else {
                out << "nullptr";
            }
            out << " ";
        }
        out << "})\n";
        return out;
    }

    std::ostream& operator << (std::ostream& out, app::Player const& value) {
        return out << "( id " << value.GetId().ToString() << ','
            << " dog_id " << *value.GetDog()->GetId() << ','
            << " session_id " << *value.GetSession()->GetId();
    }

    std::ostream& operator << (std::ostream& out, app::Players const& value) {
        using namespace app;

        const auto& tokens = value.GetPlayerTokens().GetTokenToPlayer();
        const auto& player_list = value.GetList();

        out << "(" << " size player list: " << player_list.size()
            << " size tokens: " << tokens.size()
            << " tokens list : ";

        out << "(";
        for (auto& [token, player]: tokens) {
            out << "{ token " << *token
                << " player " << player << " }";
        }
        out << ")";
        return out;
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

GameSession InitGameSession(const Game& game, const GameSession::Id& id,
                            const Map::Id& map_id, const std::vector<Dog>& dogs,
                            const std::vector<Loot>& loots) {
    auto map = game.FindMap(map_id);
    CHECK(map != nullptr);
    auto ms = std::chrono::milliseconds(static_cast<size_t>(game.GetLootPeriod()*1000));
    GameSession session(id, map, loot_gen::LootGenerator{ms, game.GetLootProbability()});

    for (auto& dog: dogs) {
        session.AddDog(dog);
    }

    for (auto& loot: loots) {
        session.AddLoot(loot);
    }
    return session;
}

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

SCENARIO_METHOD(InitGame, "GameSession Serialization") {
    GIVEN("a session") {
        const auto session = [&] {
            auto map = game.FindMap(Map::Id{"map1"});
            CHECK(map != nullptr);
            GameSession session = InitGameSession(game, GameSession::Id{1}, Map::Id{"map1"},
                                                  std::vector<Dog>{ExampleDogs::dog1, ExampleDogs::dog2, ExampleDogs::dog3},
                                                  std::vector<Loot>{ExampleLoot::loot4, ExampleLoot::loot5, ExampleLoot::loot6});
            return session;
        }();

        WHEN("session is serialized") {
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

                CHECK_THAT(restored, CommonMatcher(game_session));
            }
        }
    }
}

SCENARIO_METHOD(InitGame, "Game Serialization") {
    auto local_game = game;
    GIVEN("a game") {
        auto create_session_and_add_dog = [&](std::size_t dog_id, const std::string& name, const std::string& map_id){
            auto [current_index, session] = local_game.CreateFreeSession(Map::Id{map_id}); // создаёт пустую сессию
            session->AddDog({Dog::Id{dog_id}, name, static_cast<Point2d>(session->GenerateNewPosition(true))}); // добавляет в сессию собаку
            local_game.UpdateSessionFullness(current_index, *session); // обновляет порядок сессий относительно заполненности
        };

        WHEN("game is serialized") {
            create_session_and_add_dog(1, "Тузик1", "map1");
            create_session_and_add_dog(2, "Шарик1", "map1");
            create_session_and_add_dog(3, "Вася1", "map1");
            create_session_and_add_dog(4, "Тузик3", "map3");
            create_session_and_add_dog(5, "Шарик3", "map3");
            create_session_and_add_dog(6, "Вася3", "map3");
            {
                serialization::GameRepr repr(local_game);
                output_archive << repr;
            }

            THEN("it can be deserialized") {
                InputArchive input_archive{strm};
                serialization::GameRepr repr;
                input_archive >> repr;
                const auto restored = repr.Restore(test_config);

                CHECK_THAT(restored, CommonMatcher(local_game));
            }
        }
    }
}

SCENARIO_METHOD(InitGame, "Application Serialization") {
    using namespace app;
    GIVEN("a app") {
        const auto app = [&] {
            Application application(test_config);
            application.JoinGame(Map::Id{"map1"}, "Вася1"s);
            application.JoinGame(Map::Id{"map1"}, "Шарик1"s);
            application.JoinGame(Map::Id{"map1"}, "Тузик1"s);
            application.JoinGame(Map::Id{"map3"}, "Вася3"s);
            application.JoinGame(Map::Id{"map3"}, "Шарик3"s);
            application.JoinGame(Map::Id{"map3"}, "Тузик3"s);
            application.JoinGame(Map::Id{"town"}, "ВасяT"s);
            application.JoinGame(Map::Id{"town"}, "ШарикT"s);
            application.JoinGame(Map::Id{"town"}, "ТузикT"s);
            return application;
        }();

        WHEN("app is serialized") {
            const Application& application = app;
            {
                serialization::ApplicationRepr repr{application};
                output_archive << repr;
            }

            THEN("it can be deserialized") {
                InputArchive input_archive{strm};
                serialization::ApplicationRepr repr;
                input_archive >> repr;
                const auto restored = repr.Restore(test_config);

                CHECK_THAT(restored.GetGameModel(), CommonMatcher(application.GetGameModel()));
                CHECK_THAT(restored.GetPlayers(), CommonMatcher(application.GetPlayers()));
            }
        }
    }
}

SCENARIO_METHOD(ForSerializingListener, "Serializing Listener") {
    using namespace app;
    app::Application application(test_config);
    infrastructure::SerializingListener listener( application, state_save_test, save_period);
    application.AddApplicationListener(std::make_shared<infrastructure::SerializingListener>(listener));
    application.JoinGame(Map::Id{"map1"}, "Вася1"s);
    application.JoinGame(Map::Id{"map1"}, "Шарик1"s);
    application.JoinGame(Map::Id{"map1"}, "Тузик1"s);
    application.JoinGame(Map::Id{"map3"}, "Вася3"s);
    application.JoinGame(Map::Id{"map3"}, "Шарик3"s);
    application.JoinGame(Map::Id{"map3"}, "Тузик3"s);
    application.JoinGame(Map::Id{"town"}, "ВасяT"s);
    application.JoinGame(Map::Id{"town"}, "ШарикT"s);
    application.JoinGame(Map::Id{"town"}, "ТузикT"s);

    GIVEN("a copy app") {
        WHEN("app is saved") {
            Application app_copy = application;

            // Происходит сохранение
            application.Tick(tick_period);

            // После сохранения
            CHECK_THAT(app_copy.GetGameModel(), CommonMatcher(application.GetGameModel()));
            CHECK_THAT(app_copy.GetPlayers(), CommonMatcher(application.GetPlayers()));
            THEN("it can be load") {
                listener.Load();
                // После загрузки
                CHECK_THAT(app_copy.GetGameModel(), CommonMatcher(application.GetGameModel()));
                CHECK_THAT(app_copy.GetPlayers(), CommonMatcher(application.GetPlayers()));
            }
        }
    }
}