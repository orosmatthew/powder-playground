#include "elements.hpp"

#include "simulation.hpp"

namespace pop {

void update_salt(SimState& sim_state, Vector2i particle_pos)
{

    Vector2i bottom_pos { particle_pos.x, particle_pos.y + 1 };
    if (sim_state.in_bounds(bottom_pos)) {
        int rand_val;
        if (sim_state.particle_at(bottom_pos).element == Element::e_null) {
            rand_val = GetRandomValue(0, 5);
            if (rand_val <= 4) {
                sim_state.swap(particle_pos, bottom_pos);
            }
            return;
        }
        if (type_of(sim_state.particle_at(bottom_pos).element) == ElementType::e_liquid) {
            rand_val = GetRandomValue(0, 20);
            if (rand_val < 5) {
                sim_state.swap(particle_pos, bottom_pos);
            }
            return;
        }
    }

    int rand_side = GetRandomValue(0, 1);
    if (rand_side == 0) {
        rand_side = -1;
    }
    Vector2i side_pos { particle_pos.x + rand_side, particle_pos.y + 1 };
    if (sim_state.in_bounds(side_pos)
        && (sim_state.particle_at(side_pos).element == Element::e_null
            || type_of(sim_state.particle_at(side_pos).element) == ElementType::e_liquid)) {
        sim_state.swap(particle_pos, side_pos);
        return;
    }
}

void update_water(SimState& sim_state, Vector2i particle_pos)
{
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            if (x == 0 && y == 0) {
                continue;
            }
            if (!sim_state.in_bounds({ particle_pos.x + x, particle_pos.y + y })) {
                continue;
            }
            if (sim_state.particle_at({ particle_pos.x + x, particle_pos.y + y }).element == Element::e_lava) {
                sim_state.space[sim_state.index_at(particle_pos)].element = Element::e_steam;
                return;
            }
        }
    }

    Vector2i bottom_pos { particle_pos.x, particle_pos.y + 1 };
    if (sim_state.in_bounds(bottom_pos) && sim_state.particle_at(bottom_pos).element == Element::e_null) {
        if (GetRandomValue(0, 5) < 5) {
            sim_state.swap(particle_pos, bottom_pos);
        }
        return;
    }

    std::vector<int> sides = { -1, 1 };
    simple_shuffle<int>(sides);

    for (int side : sides) {
        Vector2i side_below_pos { particle_pos.x + side, particle_pos.y + 1 };
        if (sim_state.in_bounds(side_below_pos) && sim_state.particle_at(side_below_pos).element == Element::e_null) {
            sim_state.swap(particle_pos, side_below_pos);
            return;
        }
    }

    int rand_side = sides.at(0);

    Vector2i side_pos { particle_pos.x + rand_side, particle_pos.y };
    if (sim_state.in_bounds(side_pos) && sim_state.particle_at(side_pos).element == Element::e_null) {
        sim_state.swap(particle_pos, side_pos);
        return;
    }
}

void update_lava(SimState& sim_state, Vector2i particle_pos)
{
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            if (x == 0 && y == 0) {
                continue;
            }
            if (!sim_state.in_bounds({ particle_pos.x + x, particle_pos.y + y })) {
                continue;
            }
            if (sim_state.particle_at({ particle_pos.x + x, particle_pos.y + y }).element == Element::e_water) {
                sim_state.space[sim_state.index_at(particle_pos)].element = Element::e_stone;
                return;
            }
        }
    }

    Vector2i bottom_pos { particle_pos.x, particle_pos.y + 1 };
    if (sim_state.in_bounds(bottom_pos) && sim_state.particle_at(bottom_pos).element == Element::e_null) {
        if (GetRandomValue(0, 5) < 5) {
            sim_state.swap(particle_pos, bottom_pos);
        }
        return;
    }

    std::vector<int> sides = { -1, 1 };
    simple_shuffle<int>(sides);

    for (int side : sides) {
        Vector2i side_below_pos { particle_pos.x + side, particle_pos.y + 1 };
        if (sim_state.in_bounds(side_below_pos) && sim_state.particle_at(side_below_pos).element == Element::e_null) {
            sim_state.swap(particle_pos, side_below_pos);
            return;
        }
    }

    int rand_side = sides.at(0);

    Vector2i side_pos { particle_pos.x + rand_side, particle_pos.y };
    if (sim_state.in_bounds(side_pos) && sim_state.particle_at(side_pos).element == Element::e_null) {
        sim_state.swap(particle_pos, side_pos);
        return;
    }
}

void update_steam(SimState& sim_state, Vector2i particle_pos)
{
    std::vector<int> rand_sides = { -1, 0, 1 };
    std::vector<int> rand_vert = { -1, -1, -1, 0, 1 };
    Vector2i rand_rel { pick_rand<int>(rand_sides), pick_rand<int>(rand_vert) };
    if (rand_rel.x == 0 && rand_rel.y == 0) {
        return;
    }
    Vector2i rand_pos { particle_pos.x + rand_rel.x, particle_pos.y + rand_rel.y };

    if (!sim_state.in_bounds(rand_pos)) {
        return;
    }

    Particle& p = sim_state.particle_at(rand_pos);

    if (type_of(p.element) == ElementType::e_liquid) {
        if (rand_rel.y != 0 && rand_rel.y != 1) {
            sim_state.swap(particle_pos, rand_pos);
            return;
        }
    }

    if (p.element == Element::e_null) {
        if (GetRandomValue(0, 4) < 1) {
            sim_state.swap(particle_pos, rand_pos);
        }
        return;
    }
}

std::string to_string(Element type)
{
    switch (type) {
    case Element::e_null:
        return "Air";
    case Element::e_wall:
        return "Wall";
    case Element::e_salt:
        return "Salt";
    case Element::e_water:
        return "Water";
    case Element::e_lava:
        return "Lava";
    case Element::e_steam:
        return "Steam";
    case Element::e_stone:
        return "Stone";
    default:
        return "";
    }
}

void update_stone(SimState& sim_state, Vector2i particle_pos)
{
    Vector2i bottom_pos { particle_pos.x, particle_pos.y + 1 };
    if (sim_state.in_bounds(bottom_pos)) {
        int rand_val;
        if (sim_state.particle_at(bottom_pos).element == Element::e_null) {
            rand_val = GetRandomValue(0, 5);
            if (rand_val <= 4) {
                sim_state.swap(particle_pos, bottom_pos);
            }
            return;
        }
        if (sim_state.particle_at(bottom_pos).element == Element::e_water
            || sim_state.particle_at(bottom_pos).element == Element::e_lava) {
            rand_val = GetRandomValue(0, 20);
            if (rand_val < 5) {
                sim_state.swap(particle_pos, bottom_pos);
            }
            return;
        }
    }

    int rand_side = GetRandomValue(0, 1);
    if (rand_side == 0) {
        rand_side = -1;
    }
    Vector2i side_pos { particle_pos.x + rand_side, particle_pos.y + 1 };
    if (sim_state.in_bounds(side_pos)
        && (sim_state.particle_at(side_pos).element == Element::e_null
            || sim_state.particle_at(side_pos).element == Element::e_water)) {
        sim_state.swap(particle_pos, side_pos);
        return;
    }
}

ElementType type_of(Element element)
{
    switch (element) {
    case Element::e_null:
        return ElementType::e_null;
    case Element::e_wall:
        return ElementType::e_solid;
    case Element::e_salt:
        return ElementType::e_powder;
    case Element::e_water:
        return ElementType::e_liquid;
    case Element::e_lava:
        return ElementType::e_liquid;
    case Element::e_steam:
        return ElementType::e_gas;
    case Element::e_stone:
        return ElementType::e_powder;
    default:
        return ElementType::e_null;
    }
}

}