#pragma once
#include <cstdint>
#include <vector>
struct Size {
    uint32_t w{}, h{};
};

struct Size3D {
    uint32_t w{}, h{}, depth{};

    Size3D() = default;

    Size3D(uint32_t w, uint32_t h, uint32_t depth) : w(w), h(h), depth(depth) {}

    Size3D(const Size& s) {
        w = s.w;
        h = s.h;
        depth = 1;
    }

    Size3D& operator=(const Size& s) {
        w = s.w;
        h = s.h;
        depth = 1;
        return *this;
    }
};

struct Point {
    int32_t x{}, y{};
};

struct Region2D {
    uint32_t x{}, y{};
    uint32_t w{}, h{};
};

struct Rect {
    int32_t x{}, y{};
    uint32_t w{}, h{};
};

template <typename T>
using Ref = T*;

struct ImageData {
    Size3D size;
    std::vector<uint8_t> data;
};
