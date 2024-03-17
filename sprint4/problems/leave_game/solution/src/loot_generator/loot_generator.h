#pragma once
#include <chrono>
#include <functional>

namespace loot_gen {

/**
 *  Генератор трофеев
 */
class LootGenerator {
public:
    using RandomGenerator = std::function<double()>;
    using TimeInterval = std::chrono::milliseconds;

    /**
     * @param base_interval базовый отрезок времени > 0
     * @param probability вероятность появления трофея в течение базового интервала времени
     * @param random_gen генератор псевдослучайных чисел в диапазоне от [0 до 1]
     */
    LootGenerator(TimeInterval base_interval, double probability,
                  RandomGenerator random_gen = DefaultGenerator)
        : base_interval_{base_interval}
        , probability_{probability}
        , random_generator_{std::move(random_gen)} {
    }

    /**
     * Возвращает количество трофеев, которые должны появиться на карте спустя
     * заданный промежуток времени.
     * Количество трофеев, появляющихся на карте не превышает количество мародёров.
     * @param time_delta отрезок времени, прошедший с момента предыдущего вызова Generate
     * @param loot_count количество трофеев на карте до вызова Generate
     * @param looter_count количество мародёров на карте
     * @return количество трофеев
     */
    unsigned Generate(TimeInterval time_delta, unsigned loot_count, unsigned looter_count);

    /**
    * Возвращает интервал времени выпадения трофеев
    * @return Интервал времени
    */
    [[nodiscard]] loot_gen::LootGenerator::TimeInterval GetTimeInterval() const noexcept;

    /**
    * Возвращает вероятность выпадения трофеев
    * @return вероятность выпадения трофеев
    */
    [[nodiscard]] double GetProbability() const noexcept;

private:
    static double DefaultGenerator() noexcept {
        return 1.0;
    };
    TimeInterval base_interval_;
    double probability_;
    TimeInterval time_without_loot_{};
    RandomGenerator random_generator_;
};

}  // namespace loot_gen
