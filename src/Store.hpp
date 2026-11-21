#pragma once
#include <Geode/Geode.hpp>
#include <cstdint>
#include <string>

namespace gcu {
    // Flat key -> integer store, serialized to one string in Geode saved values.
    // Corrupt or missing data is ignored key by key, never fatal.
    struct Store {
        static int64_t get(std::string const& key, int64_t def = 0);
        static void set(std::string const& key, int64_t v);
        static void add(std::string const& key, int64_t d);
        static bool has(std::string const& key);
        static void reset();
        static int64_t now();      // unix seconds
    };
}
