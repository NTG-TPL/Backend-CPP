#pragma once
#include "geom.h"
#include "../util/tagged.h"

namespace model {

class Office: public Object {
public:
    using Id = util::Tagged<std::string, Office>;

    Office(Id id, Point2i position, const Offset& offset) noexcept:
        id_{std::move(id)}, Object(static_cast<Point2d>(position), ObjectWidth::OFFICE_WIDTH),
        offset_{offset} {
    }

    [[nodiscard]] const Id& GetId() const noexcept;
    [[nodiscard]] Offset GetOffset() const noexcept;

private:
    Id id_;
    Offset offset_;
};

} // namespace model