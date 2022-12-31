#pragma once

#include <vector>
#include <array>

#include <raylib-cpp.hpp>

struct Vector2i {
    int x;
    int y;
};

template <typename T>
T inline pick_rand(std::vector<T>& vals)
{
    return vals.at(GetRandomValue(0, vals.size() - 1));
}

template <typename Y, typename T>
T inline pick_rand(Y& vals)
{
    return vals.at(GetRandomValue(0, vals.size() - 1));
}

template <typename T>
inline void simple_shuffle(std::vector<T>& vals)
{
    for (int i = vals.size() - 1; i > 0; i--) {
        int rand_val = GetRandomValue(0, i);
        std::swap(vals[i], vals[rand_val]);
    }
}

template <typename Y, typename T>
inline void simple_shuffle(Y vals)
{
    for (int i = vals.size() - 1; i > 0; i--) {
        int rand_val = GetRandomValue(0, i);
        std::swap(vals[i], vals[rand_val]);
    }
}