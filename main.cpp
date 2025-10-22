#include <sol/sol.hpp>
#include <benchmark/benchmark.h>
#include <iostream>

// Original version using pairs and building keys table
static void BM_randomEntryIdx_original(benchmark::State& state) {
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::math);

    for (auto _ : state) {
        auto result = lua.script(R"(
            utils = {}
            function utils.randomEntryIdx(t)
                local keys = {}

                for key, _ in pairs(t) do
                    keys[#keys + 1] = key
                end

                local index = math.random(1, #keys)
                return index, t[index]
            end

            function utils.randomEntry(t)
                local _, item = utils.randomEntryIdx(t)
                return item
            end

            local t = {}
            for i = 1, 100 do
                t[i] = i
            end

            utils.randomEntry(t)
        )");
        benchmark::DoNotOptimize(result);
    }
}

// Optimized version for array-like tables
static void BM_randomEntryIdx_optimized(benchmark::State& state) {
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::math);

    for (auto _ : state) {
        auto result = lua.script(R"(
            utils = {}
            function utils.randomEntryIdx(t)
                local index = math.random(1, #t)
                return index, t[index]
            end

            function utils.randomEntry(t)
                local _, item = utils.randomEntryIdx(t)
                return item
            end

            local t = {}
            for i = 1, 100 do
                t[i] = i
            end

            utils.randomEntry(t)
        )");
        benchmark::DoNotOptimize(result);
    }
}

// Register both benchmarks
BENCHMARK(BM_randomEntryIdx_original);
BENCHMARK(BM_randomEntryIdx_optimized);

BENCHMARK_MAIN();