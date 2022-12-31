#include "elements.hpp"

#include "simulation.hpp"

namespace pop {

void update_salt(Simulation& simulation, Vector2i particle_pos)
{
    Vector2i bottom_pos { particle_pos.x, particle_pos.y + 1 };
    if (simulation.in_bounds(bottom_pos)) {
        int rand_val;
        if (simulation.type_at(bottom_pos) == ElementType::e_null) {
            rand_val = GetRandomValue(0, 5);
            if (rand_val <= 4) {
                simulation.swap(particle_pos, bottom_pos);
            }
            return;
        }
        if (simulation.type_at(bottom_pos) == ElementType::e_liquid) {
            rand_val = GetRandomValue(0, 20);
            if (rand_val < 5) {
                simulation.swap(particle_pos, bottom_pos);
            }
            return;
        }
    }

    int rand_side = GetRandomValue(0, 1);
    if (rand_side == 0) {
        rand_side = -1;
    }
    Vector2i side_pos { particle_pos.x + rand_side, particle_pos.y + 1 };
    if (simulation.in_bounds(side_pos)
        && (simulation.type_at(side_pos) == ElementType::e_null
            || simulation.type_at(side_pos) == ElementType::e_liquid)) {
        simulation.swap(particle_pos, side_pos);
        return;
    }
}

void update_water(Simulation& simulation, Vector2i particle_pos)
{
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            if (x == 0 && y == 0) {
                continue;
            }
            if (!simulation.in_bounds({ particle_pos.x + x, particle_pos.y + y })) {
                continue;
            }
            if (simulation.element_at({ particle_pos.x + x, particle_pos.y + y }).name == "lava") {
                simulation.change_element(particle_pos, "steam");
                return;
            }
        }
    }

    Vector2i bottom_pos { particle_pos.x, particle_pos.y + 1 };
    if (simulation.in_bounds(bottom_pos) && simulation.type_at(bottom_pos) == ElementType::e_null) {
        if (GetRandomValue(0, 5) < 5) {
            simulation.swap(particle_pos, bottom_pos);
        }
        return;
    }

    std::vector<int> sides = { -1, 1 };
    simple_shuffle<int>(sides);

    for (int side : sides) {
        Vector2i side_below_pos { particle_pos.x + side, particle_pos.y + 1 };
        if (simulation.in_bounds(side_below_pos) && simulation.type_at(side_below_pos) == ElementType::e_null) {
            simulation.swap(particle_pos, side_below_pos);
            return;
        }
    }

    int rand_side = sides.at(0);

    Vector2i side_pos { particle_pos.x + rand_side, particle_pos.y };
    if (simulation.in_bounds(side_pos) && simulation.type_at(side_pos) == ElementType::e_null) {
        simulation.swap(particle_pos, side_pos);
        return;
    }
}

void update_lava(Simulation& simulation, Vector2i particle_pos)
{
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            if (x == 0 && y == 0) {
                continue;
            }
            if (!simulation.in_bounds({ particle_pos.x + x, particle_pos.y + y })) {
                continue;
            }
            if (simulation.element_at({ particle_pos.x + x, particle_pos.y + y }).name == "water") {
                simulation.change_element(particle_pos, "stone");
                return;
            }
        }
    }

    Vector2i bottom_pos { particle_pos.x, particle_pos.y + 1 };
    if (simulation.in_bounds(bottom_pos) && simulation.type_at(bottom_pos) == ElementType::e_null) {
        if (GetRandomValue(0, 5) < 5) {
            simulation.swap(particle_pos, bottom_pos);
        }
        return;
    }

    std::vector<int> sides = { -1, 1 };
    simple_shuffle<int>(sides);

    for (int side : sides) {
        Vector2i side_below_pos { particle_pos.x + side, particle_pos.y + 1 };
        if (simulation.in_bounds(side_below_pos) && simulation.type_at(side_below_pos) == ElementType::e_null) {
            simulation.swap(particle_pos, side_below_pos);
            return;
        }
    }

    int rand_side = sides.at(0);

    Vector2i side_pos { particle_pos.x + rand_side, particle_pos.y };
    if (simulation.in_bounds(side_pos) && simulation.type_at(side_pos) == ElementType::e_null) {
        simulation.swap(particle_pos, side_pos);
        return;
    }
}

void update_steam(Simulation& simulation, Vector2i particle_pos)
{
    std::vector<int> rand_sides = { -1, 0, 1 };
    std::vector<int> rand_vert = { -1, -1, -1, 0, 1 };
    Vector2i rand_rel { pick_rand<int>(rand_sides), pick_rand<int>(rand_vert) };
    if (rand_rel.x == 0 && rand_rel.y == 0) {
        return;
    }
    Vector2i rand_pos { particle_pos.x + rand_rel.x, particle_pos.y + rand_rel.y };

    if (!simulation.in_bounds(rand_pos)) {
        return;
    }

    Particle& p = simulation.particle_at(rand_pos);

    if (simulation.type_of(p.element_id) == ElementType::e_liquid) {
        if (rand_rel.y != 0 && rand_rel.y != 1) {
            simulation.swap(particle_pos, rand_pos);
            return;
        }
    }

    if (simulation.element_of(p.element_id).type == ElementType::e_null
        || simulation.type_of(p.element_id) == ElementType::e_gas) {
        if (GetRandomValue(0, 4) < 1) {
            simulation.swap(particle_pos, rand_pos);
        }
        return;
    }
}

void update_stone(Simulation& simulation, Vector2i particle_pos)
{
    Vector2i bottom_pos { particle_pos.x, particle_pos.y + 1 };
    if (simulation.in_bounds(bottom_pos)) {
        int rand_val;
        if (simulation.type_at(bottom_pos) == ElementType::e_null) {
            rand_val = GetRandomValue(0, 5);
            if (rand_val <= 4) {
                simulation.swap(particle_pos, bottom_pos);
            }
            return;
        }
        if (simulation.type_at(bottom_pos) == ElementType::e_liquid) {
            rand_val = GetRandomValue(0, 20);
            if (rand_val < 5) {
                simulation.swap(particle_pos, bottom_pos);
            }
            return;
        }
    }

    int rand_side = GetRandomValue(0, 1);
    if (rand_side == 0) {
        rand_side = -1;
    }
    Vector2i side_pos { particle_pos.x + rand_side, particle_pos.y + 1 };
    if (simulation.in_bounds(side_pos)
        && (simulation.type_at(side_pos) == ElementType::e_null
            || simulation.type_at(side_pos) == ElementType::e_liquid)) {
        simulation.swap(particle_pos, side_pos);
        return;
    }
}

void update_toxic_gas(Simulation& sim_state, Vector2i particle_pos)
{
    update_steam(sim_state, particle_pos);
}

}