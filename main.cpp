#include <sol/sol.hpp>
#include <benchmark/benchmark.h>
#include <iostream>

struct LuaContext {
    sol::state lua;
    sol::table tInts;
    sol::table tStrings;
    sol::function randomEntry;

    LuaContext() {
        // Open base + math for math.random
        lua.open_libraries(sol::lib::base, sol::lib::math);

        // Create table once
        tInts = lua.create_table();
        for (int i = 1; i <= 1000; ++i) {
            tInts[i] = i * 2;
        }

        tStrings = lua.create_table();
        for (int i = 1; i <= 1000; ++i) {
            tStrings[std::to_string(i)] = i * 2;
        }

        // Original function: uses pairs + keys
        lua.script(R"(
            function randomEntryIdx(t)
                local keys = {}

                for key, _ in pairs(t) do
                    keys[#keys + 1] = key
                end

                local index = math.random(1, #keys)
                return index, t[index]
            end

            function randomEntry(t)
                local _, item = randomEntryIdx(t)
                return item
            end
        )");

        // Store function handles
        randomEntry = lua["randomEntry"];
    }
};

// Original version using pairs and building keys table
static void BM_randomEntryIdx_ints(benchmark::State& state) {
    static LuaContext ctx;

    for (auto _ : state) {
        benchmark::DoNotOptimize(ctx.randomEntry(ctx.tInts));
    }
}

// Optimized version for array-like tables
static void BM_randomEntryIdx_strings(benchmark::State& state) {
    static LuaContext ctx;

    for (auto _ : state) {
        benchmark::DoNotOptimize(ctx.randomEntry(ctx.tStrings));
    }
}

// Register both benchmarks
BENCHMARK(BM_randomEntryIdx_ints);
BENCHMARK(BM_randomEntryIdx_strings);

BENCHMARK_MAIN();