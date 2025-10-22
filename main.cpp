#include <sol/sol.hpp>
#include <benchmark/benchmark.h>
#include <iostream>

bool IsTableIpairsCompatible(const sol::table& maybeSequentialTable)
{
    // get "array length" of the table same as "#table" in lua
    size_t tableLuaLength = maybeSequentialTable.size();

    if (tableLuaLength == 0)
    {
        // don't consider an empty table valid
        return false;
    }

    // Loop over all key-values and detect any non-numeric keys or holes
    size_t indicesMatched = 0;
    for (auto& kvp : maybeSequentialTable)
    {
        const auto& key = kvp.first;

        if (!key.is<size_t>())
        {
            // Found non-integer key
            return false;
        }

        size_t k = key.as<size_t>();
        if (k < 1 || k > tableLuaLength)
        {
            // key is out of range of detected table size
            return false;
        }

        // Ensure detected table length has no holes/nil values in the table
        if (!kvp.second.is<sol::nil_t>())
        {
            ++indicesMatched;
        }
    }

    // Table has all integer keys, that range from 1 to index, with no missing (or nil-valued) keys
    return indicesMatched == tableLuaLength;
}

struct LuaContext {
    sol::state lua;
    sol::table tInts;
    sol::table tStrings;
    sol::table tMixed;
    sol::function randomEntry;

    LuaContext() {
        // Open base + math for math.random
        lua.open_libraries(sol::lib::base, sol::lib::math);

        // Create tables once
        int tableSize = 999;
        tInts = lua.create_table();
        for (int i = 1; i <= tableSize; ++i) {
            tInts[i] = i * 2;
        }

        tStrings = lua.create_table();
        for (int i = 1; i <= tableSize; ++i) {
            std::string s = std::to_string(i);
            tStrings[s] = i * 2;
        }

        tMixed = lua.create_table();
        for (int i = 1; i <= tableSize; ++i) {
            if (i > tableSize / 2)
            {
                tMixed[i] = i * 2;
            }
            else
            {
                std::string s = std::to_string(i);
                tMixed[s] = i * 2;
            }
        }

        //lua.set_function("IsTableIpairsCompatible", &IsTableIpairsCompatible);

        // Original function: uses pairs + keys
        lua.script(R"(
function IsTableIpairsCompatible(tbl)
    local tableSize = #tbl
    -- most basic requirement to be ipairs-compatible: 1st and Nth entry is not nil
    if
        tableSize == 0 or
        tbl[1] == nil or
        tbl[tableSize] == nil
    then
        return false
    end

    -- assume a table with 1000+ entries with sequential keys starting from 1 has no other relevant entries
    -- technically incorrect but very highly unlikely (and improves performance for very large tables)
    if tableSize >= 1000 then
        return true
    end

    local nonNilEntries = 0
    -- Loop over all key-values and detect any non-numeric keys or holes
    for k, v in pairs(tbl) do
        -- Found non-integer key
        if type(k) ~= 'number' or k ~= math.floor(k) then
            return false
        end

        -- key is out of range of detected table size
        if
            k < 1 or
            k > tableSize
        then
            return false
        end

        -- Ensure detected table length has no holes/nil values in the table
        if v ~= nil then
            nonNilEntries = nonNilEntries + 1
        end
    end

    -- Table has all integer keys, that range from 1 to index, with no missing (or nil-valued) keys
    return nonNilEntries == tableSize
end

function randomEntryIdx(t)
    if IsTableIpairsCompatible(t) then
        local index = math.random(1, #t)
        return index, t[index]
    end

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

static void BM_randomEntryIdx_ints(benchmark::State& state) {
    static LuaContext ctx;

    for (auto _ : state) {
        benchmark::DoNotOptimize(ctx.randomEntry(ctx.tInts));
    }
}

static void BM_randomEntryIdx_strings(benchmark::State& state) {
    static LuaContext ctx;

    for (auto _ : state) {
        benchmark::DoNotOptimize(ctx.randomEntry(ctx.tStrings));
    }
}

static void BM_randomEntryIdx_mixed(benchmark::State& state) {
    static LuaContext ctx;

    for (auto _ : state) {
        benchmark::DoNotOptimize(ctx.randomEntry(ctx.tMixed));
    }
}

// Register both benchmarks
BENCHMARK(BM_randomEntryIdx_ints)->Iterations(1000);
BENCHMARK(BM_randomEntryIdx_ints)->Iterations(1000);
BENCHMARK(BM_randomEntryIdx_ints)->Iterations(1000);
BENCHMARK(BM_randomEntryIdx_strings)->Iterations(1000);
BENCHMARK(BM_randomEntryIdx_strings)->Iterations(1000);
BENCHMARK(BM_randomEntryIdx_strings)->Iterations(1000);
BENCHMARK(BM_randomEntryIdx_mixed)->Iterations(1000);
BENCHMARK(BM_randomEntryIdx_mixed)->Iterations(1000);
BENCHMARK(BM_randomEntryIdx_mixed)->Iterations(1000);
BENCHMARK(BM_randomEntryIdx_ints)->Iterations(1000);
BENCHMARK(BM_randomEntryIdx_mixed)->Iterations(1000);
BENCHMARK(BM_randomEntryIdx_strings)->Iterations(1000);

BENCHMARK_MAIN();