#pragma once

#include <functional>

#include <raylib-cpp.hpp>

#include "common.hpp"

namespace rl = raylib;

namespace pop {

class Simulation;

enum class ElementType {
    e_null,
    e_powder,
    e_solid,
    e_liquid,
    e_gas,
};

//enum class Element {
//    e_air,
//    e_wall,
//    e_salt,
//    e_water,
//    e_lava,
//    e_steam,
//    e_stone,
//    e_toxic_gas,
//};

struct Element {
    std::string name;
    std::string friendly_name;
    ElementType type;
    std::function<void(Simulation&, Vector2i)> update_func;
    rl::Color color;
};

std::string to_string(Element type);

ElementType type_of(Element element);

void update_salt(Simulation& sim_state, Vector2i particle_pos);

void update_water(Simulation& sim_state, Vector2i particle_pos);

void update_lava(Simulation& sim_state, Vector2i particle_pos);

void update_steam(Simulation& sim_state, Vector2i particle_pos);

void update_stone(Simulation& sim_state, Vector2i particle_pos);

void update_toxic_gas(Simulation& sim_state, Vector2i particle_pos);

}