#pragma once

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_contains.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/matchers/catch_matchers_predicate.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <string>


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