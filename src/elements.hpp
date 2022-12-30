#pragma once

#include <raylib-cpp.hpp>

#include "common.hpp"

namespace pop {

class SimState;

enum class ElementType {
    e_null,
    e_powder,
    e_solid,
    e_liquid,
    e_gas,
};

enum class Element {
    e_air,
    e_wall,
    e_salt,
    e_water,
    e_lava,
    e_steam,
    e_stone,
};

std::string to_string(Element type);

ElementType type_of(Element element);

void update_salt(SimState& sim_state, Vector2i particle_pos);

void update_water(SimState& sim_state, Vector2i particle_pos);

void update_lava(SimState& sim_state, Vector2i particle_pos);

void update_steam(SimState& sim_state, Vector2i particle_pos);

void update_stone(SimState& sim_state, Vector2i particle_pos);

}