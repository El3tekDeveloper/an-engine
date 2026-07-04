#pragma once
#include <cstddef>

constexpr size_t KB(size_t n){ return n * 1024ull; }
constexpr size_t MB(size_t n){ return KB(n) * 1024ull; }
constexpr size_t GB(size_t n){ return MB(n) * 1024ull; }
