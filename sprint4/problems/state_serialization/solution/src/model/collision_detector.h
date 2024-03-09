#pragma once

#include "geom.h"

#include <algorithm>
#include <vector>

namespace model {

using Item = Object;

struct CollectionResult {
    [[nodiscard]] bool IsCollected(double collect_radius) const {
        return proj_ratio >= 0 && proj_ratio <= 1 && sq_distance <= collect_radius * collect_radius;
    }

    double sq_distance; // Квадрат расстояния до точки
    double proj_ratio;  // Доля пройденного отрезка
};

// Движемся из точки a в точку b и пытаемся подобрать точку c
CollectionResult TryCollectPoint(Point2d a, Point2d b, Point2d c);

struct Gatherer {
    Point2d start_pos;
    Point2d end_pos;
    double width;
};

class ItemGathererProvider {
protected:
    ~ItemGathererProvider() = default;

public:
    [[nodiscard]] virtual size_t ItemsCount() const = 0;
    [[nodiscard]] virtual Item GetItem(size_t idx) const = 0;
    [[nodiscard]] virtual size_t GatherersCount() const = 0;
    [[nodiscard]] virtual Gatherer GetGatherer(size_t idx) const = 0;
};

struct GatheringEvent {
    size_t item_id;
    size_t gatherer_id;
    double sq_distance;
    double time;
};

std::vector<GatheringEvent> FindGatherEvents(const ItemGathererProvider& provider);

}  // namespace collision_detector
