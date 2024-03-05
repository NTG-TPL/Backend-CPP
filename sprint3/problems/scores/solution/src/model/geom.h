#pragma once
#include <random>

namespace model {

using DimensionInt = int;
using DimensionDouble = double;
using CoordInt = DimensionInt;
using CoordDouble = DimensionDouble;

template <typename TypeNumber>
struct Point {
    TypeNumber x, y;

    explicit operator Point<CoordDouble>() const {
        return {static_cast<CoordDouble>(x), static_cast<CoordDouble>(y)};
    }

    explicit operator Point<DimensionInt>() const {
        return {static_cast<DimensionInt>(x), static_cast<DimensionInt>(y)};
    }
};

template <typename TypeNumber>
struct Velocity {
    TypeNumber dx, dy;
};

using Point2i = Point<DimensionInt>;
using Point2d = Point<CoordDouble>;
using Velocity2d = Velocity<CoordDouble>;


struct Size {
    DimensionInt width, height;
};

struct Rectangle {
    Point2i position;
    Size size;
};

struct Offset {
    DimensionInt dx, dy;
};

struct ObjectWidth {
    ObjectWidth() = delete;
    static inline const DimensionDouble DOG_WIDTH = 0.6;
    static inline const DimensionDouble ITEM_WIDTH = 0.0;
    static inline const DimensionDouble OFFICE_WIDTH = 0.5;
};

class Object {
public:
    explicit Object(Point2d const& position = Point2d(), double const& width = 0.0):
            position_(position), width_(width){
    }

    void SetPosition(Point2d const& position) { position_ = position; }
    [[nodiscard]] Point2d GetPosition() const noexcept {return position_; }

    void SetWidth(double const& width) { width_ = width; }
    [[nodiscard]] DimensionDouble GetWidth() const noexcept { return width_; }

    [[nodiscard]] Object GetObjectCopy(){
        return Object(position_, width_);
    }

    virtual ~Object() = default;
protected:
    Point2d position_;
    DimensionDouble width_;
};

/**
 * Генерирует число из диапазона [min, max]
 * @param min наименьшее сгенерированное число
 * @param max наибольшее сгенерированное число
 * @return случайное число из диапазона [min, max]
 */
template<typename T>
static T GenerateInRange(T min, T max){
    static std::random_device random_device;
    static std::mt19937 generator{random_device()};
    std::uniform_int_distribution<T> distribution(min, max);
    return distribution(generator);
}

template<>
double GenerateInRange(double min, double max){
    static std::random_device random_device;
    static std::mt19937 generator{random_device()};
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
}

} // namespace model