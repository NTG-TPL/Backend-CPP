#pragma once

#include <chrono>

namespace app {

using milliseconds = std::chrono::milliseconds;
class ApplicationListener {
public:
    virtual void OnTick(std::chrono::milliseconds tick) = 0;
};

} // namespace app