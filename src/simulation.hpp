#pragma once

#include <string>
#include <vector>

#include <BS_thread_pool.hpp>
#include <raylib-cpp.hpp>

#include "common.hpp"
#include "elements.hpp"

namespace pop {

void draw_sim(
    raylib::Image& render_image, raylib::Image& gas_image, const Simulation& simulation, BS::thread_pool& pool);

using ElementId = uint32_t;

struct Particle {
    ElementId element_id = 0;
    float shade = 1.0f;
};

class Simulation {
public:
    Simulation(int width, int height);

    void push_element(Element element);

    void update();

    void change_element(Vector2i pos, ElementId element_id);

    void change_element(Vector2i pos, const std::string& element_name);

    void clear_to(const std::string& element_name);

    [[nodiscard]] int width() const;

    [[nodiscard]] int height() const;

    [[nodiscard]] const Element element_at(Vector2i pos) const;

    [[nodiscard]] const Element element_of(ElementId element_id) const;

    [[nodiscard]] ElementId id_of(const std::string& element_name) const;

    [[nodiscard]] ElementId id_at(Vector2i pos) const;

    [[nodiscard]] ElementType type_of(ElementId element_id) const;

    [[nodiscard]] ElementType type_at(Vector2i pos) const;

    [[nodiscard]] int index_at(Vector2i pos) const;

    [[nodiscard]] Vector2i pos_at(int i) const;

    [[nodiscard]] bool in_bounds(Vector2i pos) const;

    [[nodiscard]] const Particle& particle_at(Vector2i pos) const;

    [[nodiscard]] Particle& particle_at(Vector2i pos);

    void swap(Vector2i pos1, Vector2i pos2);

private:
    const int m_width;
    const int m_height;
    ElementId m_element_id_count = 1;
    std::unordered_map<ElementId, Element> m_elements {};
    std::unordered_map<std::string, ElementId> m_element_name_map {};
    std::vector<Particle> m_space {};
};

}