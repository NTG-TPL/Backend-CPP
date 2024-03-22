#define _USE_MATH_DEFINES

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_contains.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/matchers/catch_matchers_predicate.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <random>
#include <sstream>
#include <utility>

#include "../src/model/collision_detector.h"

using namespace std::literals;

using namespace model;
using Catch::Matchers::Contains;
using Catch::Matchers::WithinAbs;
using Catch::Matchers::Predicate;

static constexpr double epsilon = 1e-10;

namespace Catch {
    template<>
    struct StringMaker<GatheringEvent> {
        static std::string convert(GatheringEvent const& value) {
            std::ostringstream tmp;
            tmp << "(" << value.gatherer_id << "," << value.item_id << "," << value.sq_distance << "," << value.time << ")";
            return tmp.str();
        }
    };
    template<>
    struct StringMaker<CollectionResult> {
        static std::string convert(CollectionResult const& value) {
            std::ostringstream tmp;
            tmp << "(sq_distance=" << value.sq_distance << ",proj_ratio=" << value.proj_ratio << ")";
            return tmp.str();
        }
    };
}  // namespace Catch

class ItemGatherer : public ItemGathererProvider {
public:
    using Items = std::vector<Item>;
    using Gatherers = std::vector <Gatherer>;

    [[nodiscard]] size_t ItemsCount() const override { return items.size(); }
    [[nodiscard]] Item GetItem(size_t idx) const override { return items[idx]; }
    [[nodiscard]] size_t GatherersCount() const override { return gatherers.size(); }
    [[nodiscard]] Gatherer GetGatherer(size_t idx) const override { return gatherers[idx]; }

    std::vector<Item> items;
    std::vector<Gatherer> gatherers;
};

bool operator==(const GatheringEvent &lh, const GatheringEvent &rh) {
    return lh.item_id == rh.item_id &&
           lh.gatherer_id == rh.gatherer_id &&
           lh.sq_distance == Catch::Approx(rh.sq_distance).epsilon(epsilon) &&
           lh.time == Catch::Approx(rh.time).epsilon(epsilon);
}

template <typename T>
class IsEqualMatcher : public Catch::Matchers::MatcherBase<T> {
public:
    explicit IsEqualMatcher(T item_gatherer) : item_gatherer_(std::move(item_gatherer)) {}

    bool match(T const& in) const override {
        using std::begin;
        using std::end;
        return std::equal(begin(in), end(in), begin(item_gatherer_), end(item_gatherer_),
                          [] (const GatheringEvent &a, const GatheringEvent &b) {
                              return a == b;
                          });
    }

    [[nodiscard]] std::string describe() const override {
        std::ostringstream ss;
        ss << "{";
        for (const auto &v : item_gatherer_)
            ss << "(" << v.gatherer_id << "," << v.item_id << "," << v.sq_distance << "," << v.time << ")";
        ss << "}";
        return ss.str();
    }
private:
    T item_gatherer_;
};

template <typename T>
IsEqualMatcher<T> IsEqual(T t) {
    return IsEqualMatcher{ t };
}

struct ExampleItems {
    ExampleItems() = delete;
    static inline const ItemGatherer::Items item1 = { Item({5.0, 2.0}, 1.0 ) };
    static inline const ItemGatherer::Items item2 = { Item({5.0, 3},  1.0 ) };
    static inline const ItemGatherer::Items item3 = { Item({5.0, 2},  0.99 ) };
    static inline const ItemGatherer::Items item4 = { Item({11.0, 0},  2 ) };
    static inline const ItemGatherer::Items item5 = { Item({10.0, 2},  1 ) };
    static inline const ItemGatherer::Items item6 = { Item({10.0, 0},  1 ) };
    static inline const ItemGatherer::Items item7 = { Item({5.0, 0},  0.5 ) };
    static inline const ItemGatherer::Items item8 = { Item({5.0, 0},  2.0 ) };
    static inline const ItemGatherer::Items item_inclined_track_1 = {Item({3.0, 6.0}, 1.79 ) };
    static inline const ItemGatherer::Items item_inclined_track_2 = {Item({3.0, 6.0}, 1.78 ) };
    static inline const ItemGatherer::Items subsequence_items = {
            Item({1.97, 2.76}, 0.94 ),
            Item({1.36, 2.44}, 0.46 ),
            Item({6.00, 5.00}, 0.45 )};

};

struct ExampleGatherers {
    static inline  const ItemGatherer::Gatherers gatherer1 =
            { Gatherer{.start_pos{0.0, 0.0}, .end_pos{10.0, 0.0}, .width = 1.0} };
    static inline  const ItemGatherer::Gatherers gatherer_inclined_track =
            { Gatherer{.start_pos{-0.5, 1.0}, .end_pos{9.5, 6.0}, .width = 1.12} };
};

auto gen_lamda(const auto &ge_exp) {
    return [&ge_exp](GatheringEvent in) {
        return in == ge_exp;
    };
}
void contains_gather_event(ItemGatherer &item_gatherer, const ItemGatherer::Items &item, const GatheringEvent &ge_exp) {
    item_gatherer.items = item;
    auto ge_res = FindGatherEvents(item_gatherer);
    CHECK_THAT(ge_res, Contains(Predicate<GatheringEvent>(gen_lamda(ge_exp))));
}
void not_contains_gather_event(ItemGatherer &item_gatherer, const ItemGatherer::Items &item, const GatheringEvent &ge_exp) {
    item_gatherer.items = item;
    auto ge_res = FindGatherEvents(item_gatherer);
    CHECK_THAT(ge_res, !Contains(Predicate<GatheringEvent>(gen_lamda(ge_exp))));
}

static double GenerateInRangeDouble(double min, double max){
    static std::random_device random_device;
    static std::mt19937 generator{random_device()};
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
}

Point2d randomPoint(const Point2d& min, const Point2d& max ) {
    const double x = GenerateInRangeDouble(min.x, max.x);
    const double y = GenerateInRangeDouble(min.y, max.y);
    return {x, y};
}

ItemGatherer Init(size_t num, Point2d const& min, Point2d const& max, double const& maxMove = 50.0, double const& width = 0.6) {
    ItemGatherer result;
    assert(num >= 2);
    for(size_t i = 0; i < num-1; ++i) {
        result.items.emplace_back(randomPoint(min , max), width);
        Point2d start = randomPoint(min, max), end;
        if(i%2) {
            end.x = start.x;
            end.y = GenerateInRangeDouble( start.y - maxMove, start.y + maxMove);
            result.gatherers.emplace_back(start, end, width);
        } else {
            end.y = start.y;
            end.x = GenerateInRangeDouble( start.x - maxMove, start.x + maxMove);
            result.gatherers.emplace_back(start, end, width);
        }
    }
    result.items.emplace_back(Point2d(0.0 , maxMove / 2 ), width);
    result.gatherers.emplace_back(Point2d(0.0, 0.0), Point2d(0.0, maxMove), width);
    return result;
}

auto compTime = [](const GatheringEvent& a, const GatheringEvent& b) {
    return a.time < b.time;
};

SCENARIO("Collision detector", "[Collision detector]") {
    GIVEN("Item gatherer container") {
        ItemGatherer item_gatherer;
        WHEN("Conteiner emtpy") {
            CHECK(item_gatherer.GatherersCount() == 0);
            CHECK(item_gatherer.ItemsCount() == 0);
        }
        THEN("Add item1") {
            item_gatherer.items = ExampleItems::item1;
            THEN("item1") {
                CHECK(item_gatherer.ItemsCount() == 1);
            }
            AND_THEN("check item1") {
                CHECK_THAT(item_gatherer.GetItem(0).GetPosition().x, WithinAbs(ExampleItems::item1[0].GetPosition().x, epsilon));
                CHECK_THAT(item_gatherer.GetItem(0).GetPosition().y, WithinAbs(ExampleItems::item1[0].GetPosition().y, epsilon));
                CHECK_THAT(item_gatherer.GetItem(0).GetWidth(), WithinAbs(ExampleItems::item1[0].GetWidth(), epsilon));
            }
        }
    }
    AND_GIVEN("Track and points") {
        WHEN("line horizontal") {
            auto coll = TryCollectPoint(Point2d{0, 0}, Point2d{10, 0}, Point2d{5, 2});
            WHEN("IsCollected") {
                CHECK(coll.IsCollected(2));
            }
            AND_WHEN("!IsCollected") {
                CHECK(!coll.IsCollected(1));
            }
        }
        AND_WHEN("line vertical") {
            auto coll = TryCollectPoint(Point2d{0, 0}, Point2d{0, 10}, Point2d{2, 5});
            WHEN("IsCollected") {
                CHECK(coll.IsCollected(2));
            }
            AND_WHEN("!IsCollected") {
                CHECK(!coll.IsCollected(1));
            }
        }
        AND_WHEN("Point in line vertical") {
            auto coll = TryCollectPoint(Point2d{0, 0}, Point2d{0, 10}, Point2d{0, 5});
            WHEN("IsCollected") {
                CHECK(coll.IsCollected(1));
            }
            AND_WHEN("IsCollected") {
                CHECK(coll.IsCollected(10));
            }
        }
        AND_WHEN("Point out abscissa line vertical") {
            auto coll = TryCollectPoint(Point2d{0, 0}, Point2d{0, 10}, Point2d{12, 2});
            WHEN("Point !IsCollected") {
                CHECK(!coll.IsCollected(3));
                CHECK(!coll.IsCollected(2));
            }
        }
        AND_WHEN("Point out abscissa oblique line") {
            auto coll = TryCollectPoint(Point2d{0, 0}, Point2d{8, -6}, Point2d{6, -2});
            WHEN("IsCollected") {
                CHECK(coll.IsCollected(2));
            }
            AND_WHEN("!IsCollected") {
                CHECK(!coll.IsCollected(1.9));
            }
        }
    }
    AND_GIVEN("Item gatherer container for check collision") {
        ItemGatherer item_gatherer;
        THEN("add item1") {
            item_gatherer.items = ExampleItems::item1;
            WHEN("gatherer is not empty") {
                CHECK(item_gatherer.ItemsCount() == 1);
            }
        }
        AND_THEN("add gatherer1") {
            item_gatherer.gatherers = ExampleGatherers::gatherer1;
            WHEN("item_gatherer is not empty") {
                CHECK(item_gatherer.GatherersCount() == 1);
            }
        }
        AND_THEN("gatherer1") {
            item_gatherer.gatherers = ExampleGatherers::gatherer1;
            WHEN("capture item") {
                contains_gather_event(item_gatherer, ExampleItems::item1,
                                      GatheringEvent{.item_id = 0, .gatherer_id = 0, .sq_distance = 4.0, .time = 0.5});
            }
            AND_WHEN("not capture item") {
                not_contains_gather_event(item_gatherer, ExampleItems::item2,
                                          GatheringEvent{.item_id = 0, .gatherer_id = 0, .sq_distance = 9.0, .time = 0.5});
            }
            AND_WHEN("not capture item") {
                not_contains_gather_event(item_gatherer, ExampleItems::item3,
                                          GatheringEvent{.item_id = 0, .gatherer_id = 0, .sq_distance = 4.0, .time = 0.5});
            }
            AND_WHEN("not capture item") {
                not_contains_gather_event(item_gatherer, ExampleItems::item4,
                                          GatheringEvent{.item_id = 0, .gatherer_id = 0, .sq_distance = 4.0, .time = 0.5});
            }
            AND_WHEN("capture item") {
                contains_gather_event(item_gatherer, ExampleItems::item5,
                                      GatheringEvent{.item_id = 0, .gatherer_id = 0, .sq_distance = 4.0, .time = 1.0});
            }
            AND_WHEN("item inside 1") {
                contains_gather_event(item_gatherer, ExampleItems::item6,
                                      GatheringEvent{.item_id = 0, .gatherer_id = 0, .sq_distance = 0.0, .time = 1.0});
            }
            AND_WHEN("item inside 2") {
                contains_gather_event(item_gatherer, ExampleItems::item7,
                                      GatheringEvent{.item_id = 0, .gatherer_id = 0, .sq_distance = 0.0, .time = 0.5});
            }
            AND_WHEN("item inside 3") {
                contains_gather_event(item_gatherer, ExampleItems::item8,
                                      GatheringEvent{.item_id = 0, .gatherer_id = 0, .sq_distance = 0.0, .time = 0.5});
            }
        }
        AND_THEN("inclined track") {
            item_gatherer.gatherers = ExampleGatherers::gatherer_inclined_track;
            WHEN("capture item") {
                contains_gather_event(item_gatherer, ExampleItems::item_inclined_track_1,
                                      GatheringEvent{.item_id = 0, .gatherer_id = 0, .sq_distance = 8.45, .time = 0.48});
            }
            AND_WHEN("not capture item")  {
                not_contains_gather_event(item_gatherer, ExampleItems::item_inclined_track_2,
                                          GatheringEvent{.item_id = 0, .gatherer_id = 0, .sq_distance = 8.45, .time = 0.48});
            }
            AND_WHEN("subsequence items")  {
                item_gatherer.items = ExampleItems::subsequence_items;
                std::vector<GatheringEvent> ge_exp{{.item_id = 1, .gatherer_id = 0, .sq_distance = 0.20808, .time = 0.2064},
                                                   {.item_id = 0, .gatherer_id = 0, .sq_distance = 0.2205,  .time = 0.268},
                                                   {.item_id = 2, .gatherer_id = 0, .sq_distance = 0.45,    .time = 0.68}};
                auto ge_res = FindGatherEvents(item_gatherer);
                CHECK_THAT(ge_res, IsEqual(ge_exp));
            }
        }
    }
}