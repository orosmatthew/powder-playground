#include "simulation.hpp"

#include <cassert>

#include "elements.hpp"

namespace rl = raylib;

namespace pop {

void draw_particle(rl::Image& render_image, const Simulation& simulation, Vector2i pos)
{
    render_image.DrawPixel(pos.x, pos.y, simulation.element_at(pos).color);
}

void draw_column(
    rl::Image& render_image, rl::Image& gas_image, const Simulation& simulation, int start_col, int col_width)
{
    for (int x = start_col * col_width; x < (start_col + 1) * col_width; x++) {
        for (int y = 0; y < simulation.height(); y++) {
            if (simulation.type_at({ x, y }) == ElementType::e_gas) {
                draw_particle(gas_image, simulation, { x, y });
            }
            else {
                draw_particle(render_image, simulation, { x, y });
            }
        }
    }
}

void draw_sim(rl::Image& render_image, rl::Image& gas_image, const Simulation& simulation, BS::thread_pool& pool)
{
    for (int col = 0; col < 8; col++) {
        pool.push_task([col, &simulation, &render_image, &gas_image] {
            draw_column(render_image, gas_image, simulation, col, 40);
        });
    }
    pool.wait_for_tasks();
}

int Simulation::index_at(Vector2i pos) const
{
    return m_width * pos.y + pos.x;
}
Vector2i Simulation::pos_at(int i) const
{
    return { i % m_width, i / m_width };
}
bool Simulation::in_bounds(Vector2i pos) const
{
    return pos.x >= 0 && pos.x < m_width && pos.y >= 0 && pos.y < m_height;
}
const Particle& Simulation::particle_at(Vector2i pos) const
{
    return m_space.at(index_at(pos));
}
Particle& Simulation::particle_at(Vector2i pos)
{
    return m_space.at(index_at(pos));
}
void Simulation::swap(Vector2i pos1, Vector2i pos2)
{
    std::swap(m_space.at(index_at(pos1)), m_space.at(index_at(pos2)));
}

Simulation::Simulation(int width, int height)
    : m_width(width)
    , m_height(height)
{
    for (int i = 0; i < m_width * m_height; i++) {
        m_space.push_back({});
    }
}

void Simulation::push_element(Element element)
{
    ElementId id = m_element_id_count;
    m_element_id_count++;

    m_elements.insert({ id, element });
    m_element_name_map.insert({ element.name, id });
}

ElementId Simulation::id_of(const std::string& element_name) const
{
    return m_element_name_map.at(element_name);
}

void Simulation::update()
{
    std::vector<int> rand_indices;
    rand_indices.reserve(m_width);
    for (int i = 0; i < m_width; i++) {
        rand_indices.push_back(i);
    }

    for (int y = m_height - 1; y >= 0; y--) {
        simple_shuffle(rand_indices);
        for (int x : rand_indices) {
            std::invoke(m_elements.at(particle_at({ x, y }).element_id).update_func, *this, Vector2i { x, y });
        }
    }
}
int Simulation::width() const
{
    return m_width;
}
int Simulation::height() const
{
    return m_height;
}

ElementType Simulation::type_of(ElementId element_id) const
{
    return m_elements.at(element_id).type;
}

ElementType Simulation::type_at(Vector2i pos) const
{
    return type_of(particle_at(pos).element_id);
}

const Element Simulation::element_at(Vector2i pos) const
{
    return m_elements.at(particle_at(pos).element_id);
}
const Element Simulation::element_of(ElementId element_id) const
{
    return m_elements.at(element_id);
}

void Simulation::change_element(Vector2i pos, ElementId element_id)
{
    m_space.at(index_at(pos)).element_id = element_id;
}

void Simulation::change_element(Vector2i pos, const std::string& element_name)
{
    m_space.at(index_at(pos)).element_id = id_of(element_name);
}

void Simulation::clear_to(const std::string& element_name)
{
    ElementId id = id_of(element_name);
    for (int i = 0; i < m_space.size(); i++) {
        m_space.at(i).element_id = id;
    }
}

}
