#pragma once

#include <string>
#include <vector>

#include <BS_thread_pool.hpp>
#include <raylib-cpp.hpp>

#include "common.hpp"
#include "elements.hpp"

namespace pop {

struct Particle {
    Element element = Element::e_air;
    float shade = 1.0f;
};

struct SimState {
    const int width;
    const int height;
    std::vector<Particle> space;

    [[nodiscard]] inline int index_at(Vector2i pos) const
    {
        return width * pos.y + pos.x;
    }

    [[nodiscard]] inline Vector2i pos_at(int i) const
    {
        return { i % width, i / width };
    }

    [[nodiscard]] inline bool in_bounds(Vector2i pos) const
    {
        return pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height;
    }

    [[nodiscard]] inline const Particle& particle_at(Vector2i pos) const
    {
        return space.at(index_at(pos));
    }

    [[nodiscard]] inline Particle& particle_at(Vector2i pos)
    {
        return space.at(index_at(pos));
    }

    inline void swap(Vector2i pos1, Vector2i pos2)
    {
        std::swap(space.at(index_at(pos1)), space.at(index_at(pos2)));
    }
};

void update_sim(SimState& sim_state);

void draw_sim(raylib::Image& render_image, raylib::Image& gas_image, const SimState& sim_state, BS::thread_pool& pool);

}