#include "road.h"

namespace model {

/**
* Проверка на горизонтальный маршрут
* @return если маршрут горизонтальный, то возвращает true, иначе - false.
*/
bool Road::IsHorizontal() const noexcept {
    return start_.y == end_.y;
}

/**
* Проверка на вертикальный маршрут
* @return если маршрут вертикальный, то возвращает true, иначе - false.
*/
bool Road::IsVertical() const noexcept {
    return start_.x == end_.x;
}

/**
* Получить начальную точку маршрута
* @return Point2i start;
*/
Point2i Road::GetStart() const noexcept {
    return start_;
}

/**
* Получить конечную точку маршрута
* @return Point2i end;
*/
Point2i Road::GetEnd() const noexcept {
    return end_;
}

/**
* Получить границы дороги
* @return Borders - границы дороги от минимального до максимального значений по оси x и по y
*/
Road::RoadRectangle::Borders Road::GetBorders() const noexcept {
    return road_rectangle_.GetBorders();
}

/**
* Получить ширину дороги
* @return Ширина дороги
*/
[[nodiscard]] DimensionDouble Road::GetWidth() const noexcept {
    return road_rectangle_.GetWidth();
}

/**
* Проверяет, содержится ли точка в маршруте
* @param point точка
* @return true - если содержится, иначе - false
*/
bool Road::Contains(Point2d point) const noexcept{
    return road_rectangle_.GetBorders().Contains(point);
}
} // namespace model