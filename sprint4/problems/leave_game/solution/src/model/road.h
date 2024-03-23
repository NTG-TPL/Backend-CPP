#pragma once
#include "geom.h"

namespace model {
class Road {
    struct HorizontalTag {
        HorizontalTag() = default;
    };

    struct VerticalTag {
        VerticalTag() = default;
    };

public:
    class RoadRectangle {
    public:
        struct Borders {
            CoordDouble min_x, max_x,
                    min_y, max_y;

            [[nodiscard]] inline bool Contains(Point2d point) const{
                return  point.x >= min_x && point.x <= max_x &&
                        point.y >= min_y && point.y <= max_y;
            }
        };

        RoadRectangle (Point2d start, Point2d end, DimensionDouble width) :
                borders_{std::min(start.x, end.x) - width / 2,
                         std::max(start.x, end.x) + width / 2,
                         std::min(start.y, end.y) - width / 2,
                         std::max(start.y, end.y) + width / 2},
                width_(width) {}

        [[nodiscard]] Borders GetBorders() const {
            return borders_;
        }

        [[nodiscard]] DimensionDouble GetWidth() const{
            return width_;
        }

    private:
        Borders borders_;
        DimensionDouble width_;
    };

    constexpr static HorizontalTag HORIZONTAL{};
    constexpr static VerticalTag VERTICAL{};

    Road(HorizontalTag, Point2i start, CoordInt end_x, DimensionDouble width = 0.8) noexcept:
            start_{start}, end_{end_x, start.y},
            road_rectangle_(static_cast<Point2d>(start_), static_cast<Point2d>(end_), width) {}
    Road(VerticalTag, Point2i start, CoordInt end_y, DimensionDouble width = 0.8) noexcept:
            start_{start}, end_{start.x, end_y},
            road_rectangle_(static_cast<Point2d>(start_), static_cast<Point2d>(end_), width) {}

    [[nodiscard]] bool IsHorizontal() const noexcept;
    [[nodiscard]] bool IsVertical() const noexcept;
    [[nodiscard]] Point2i GetStart() const noexcept;
    [[nodiscard]] Point2i GetEnd() const noexcept;
    [[nodiscard]] Road::RoadRectangle::Borders GetBorders() const noexcept;
    [[nodiscard]] DimensionDouble GetWidth() const noexcept;
    [[nodiscard]] bool Contains(Point2d point) const noexcept;

private:
    Point2i start_;
    Point2i end_;
    RoadRectangle road_rectangle_;
};
} // namespace model