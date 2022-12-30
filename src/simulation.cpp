#include "simulation.hpp"

#include <cassert>

#include "elements.hpp"

namespace rl = raylib;

namespace pop {

void update_particle(pop::SimState& sim_state, Vector2i particle_pos)
{
    switch (sim_state.particle_at(particle_pos).element) {
    case Element::e_salt:
        update_salt(sim_state, particle_pos);
        break;
    case Element::e_water:
        update_water(sim_state, particle_pos);
        break;
    case Element::e_stone:
        update_stone(sim_state, particle_pos);
        break;
    case Element::e_null:
    case Element::e_wall:
        break;
    case Element::e_lava:
        update_lava(sim_state, particle_pos);
        break;
    case Element::e_steam:
        update_steam(sim_state, particle_pos);
        break;
    }
}

void update_sim(SimState& sim_state)
{
    std::vector<int> rand_indices;
    rand_indices.reserve(sim_state.width);
    for (int i = 0; i < sim_state.width; i++) {
        rand_indices.push_back(i);
    }

    for (int y = sim_state.height - 1; y >= 0; y--) {
        simple_shuffle(rand_indices);
        for (int x : rand_indices) {
            update_particle(sim_state, { x, y });
        }
    }
}

void draw_particle(rl::Image& render_image, const SimState& sim_state, Vector2i pos)
{
    switch (sim_state.particle_at(pos).element) {
    case Element::e_null:
        render_image.DrawPixel(pos.x, pos.y, rl::Color(15, 15, 15));
        break;
    case Element::e_salt:
        render_image.DrawPixel(pos.x, pos.y, rl::Color::FromHSV(0.0f, 0.0f, sim_state.particle_at(pos).shade));
        break;
    case Element::e_water:
        render_image.DrawPixel(pos.x, pos.y, rl::Color::FromHSV(243.0f, 0.9f, 1.0f));
        break;
    case Element::e_wall:
        render_image.DrawPixel(pos.x, pos.y, rl::Color(120, 120, 120));
        break;
    case Element::e_lava:
        render_image.DrawPixel(pos.x, pos.y, rl::Color(255, 94, 0));
        break;
    case Element::e_steam:
        render_image.DrawPixel(pos.x, pos.y, rl::Color(106, 194, 255));
        break;
    case Element::e_stone:
        render_image.DrawPixel(pos.x, pos.y, rl::Color(140, 140, 140));
        break;
    }
}

void draw_column(rl::Image& render_image, rl::Image& gas_image, const SimState& sim_state, int start_col, int col_width)
{
    for (int x = start_col * col_width; x < (start_col + 1) * col_width; x++) {
        for (int y = 0; y < sim_state.height; y++) {
            if (sim_state.particle_at({ x, y }).element == Element::e_steam) {
                draw_particle(gas_image, sim_state, { x, y });
            }
            else {
                draw_particle(render_image, sim_state, { x, y });
            }
        }
    }
}

void draw_sim(rl::Image& render_image, rl::Image& gas_image, const SimState& sim_state, BS::thread_pool& pool)
{
    assert(render_image.width == sim_state.width && render_image.height == sim_state.height);

    for (int col = 0; col < 8; col++) {
        pool.push_task([col, &sim_state, &render_image, &gas_image] {
            draw_column(render_image, gas_image, sim_state, col, 40);
        });
    }
    pool.wait_for_tasks();
}

}
