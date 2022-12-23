#include "powder_playground.hpp"

#include <iostream>
#include <random>

#include <BS_thread_pool.hpp>
#include <raylib-cpp.hpp>

#include "fixed_loop.hpp"

namespace rl = raylib;

namespace pop {

enum class ParticleType {
    e_null,
    e_salt,
    e_water,
};

struct Particle {
    ParticleType type = ParticleType::e_null;
    float shade = 1.0f;
};

struct Vector2i {
    int x;
    int y;
};

struct SimState {
    const int width;
    const int height;
    std::vector<Particle> space;

    [[nodiscard]] inline int get_index(Vector2i pos) const
    {
        return width * pos.y + pos.x;
    }

    [[nodiscard]] inline Vector2i get_pos(int i) const
    {
        return { i % width, i / width };
    }

    [[nodiscard]] inline bool in_space(Vector2i pos) const
    {
        return pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height;
    }

    [[nodiscard]] inline const Particle& particle_at(Vector2i pos) const
    {
        return space.at(get_index(pos));
    }

    [[nodiscard]] inline Particle& particle_at(Vector2i pos)
    {
        return space.at(get_index(pos));
    }

    inline void swap(Vector2i pos1, Vector2i pos2)
    {
        std::swap(space.at(get_index(pos1)), space.at(get_index(pos2)));
    }
};

void update_salt(SimState& sim_state, Vector2i particle_pos)
{
    Vector2i bottom_pos { particle_pos.x, particle_pos.y + 1 };
    if (sim_state.in_space(bottom_pos)
        && (sim_state.particle_at(bottom_pos).type == ParticleType::e_null
            || sim_state.particle_at(bottom_pos).type == ParticleType::e_water)) {
        sim_state.swap(particle_pos, bottom_pos);
        return;
    }

    int rand_side = GetRandomValue(0, 1);
    if (rand_side == 0) {
        rand_side = -1;
    }
    Vector2i side_pos { particle_pos.x + rand_side, particle_pos.y + 1 };
    if (sim_state.in_space(side_pos)
        && (sim_state.particle_at(side_pos).type == ParticleType::e_null
            || sim_state.particle_at(side_pos).type == ParticleType::e_water)) {
        sim_state.swap(particle_pos, side_pos);
        return;
    }
}

void update_water(SimState& sim_state, Vector2i particle_pos)
{
    Vector2i bottom_pos { particle_pos.x, particle_pos.y + 1 };
    if (sim_state.in_space(bottom_pos) && sim_state.particle_at(bottom_pos).type == ParticleType::e_null) {
        sim_state.swap(particle_pos, bottom_pos);
        return;
    }

    int rand_side = GetRandomValue(0, 1);
    if (rand_side == 0) {
        rand_side = -1;
    }
    Vector2i side_pos { particle_pos.x + rand_side, particle_pos.y };
    if (sim_state.in_space(side_pos) && sim_state.particle_at(side_pos).type == ParticleType::e_null) {
        sim_state.swap(particle_pos, side_pos);
        return;
    }
}

void update_particle(SimState& sim_state, Vector2i particle_pos)
{
    switch (sim_state.particle_at(particle_pos).type) {
    case ParticleType::e_salt:
        update_salt(sim_state, particle_pos);
        break;
    case ParticleType::e_water:
        update_water(sim_state, particle_pos);
        break;
    case ParticleType::e_null:
        break;
    }
}

void update_sim(SimState& sim_state, std::mt19937& rand_engine)
{
    std::vector<int> rand_indices;
    rand_indices.reserve(sim_state.space.size());
    for (int i = 0; i < sim_state.space.size(); i++) {
        rand_indices.push_back(i);
    }

    std::shuffle(rand_indices.begin(), rand_indices.end(), rand_engine);

    for (int i : rand_indices) {
        update_particle(sim_state, sim_state.get_pos(i));
    }
}

void draw_particle(rl::Image& render_image, const SimState& sim_state, Vector2i pos)
{
    switch (sim_state.particle_at(pos).type) {
    case ParticleType::e_null:
        render_image.DrawPixel(pos.x, pos.y, rl::Color::Black());
        break;
    case ParticleType::e_salt:
        render_image.DrawPixel(pos.x, pos.y, rl::Color::FromHSV(0.0f, 0.0f, sim_state.particle_at(pos).shade));
        break;
    case ParticleType::e_water:
        render_image.DrawPixel(pos.x, pos.y, rl::Color::FromHSV(243.0f, 0.9f, sim_state.particle_at(pos).shade));
        break;
    }
}

void draw_column(rl::Image& render_image, const SimState& sim_state, int start_col, int col_width)
{
    for (int x = start_col * col_width; x < (start_col + 1) * col_width; x++) {
        for (int y = 0; y < sim_state.height; y++) {
            draw_particle(render_image, sim_state, { x, y });
        }
    }
}

void draw_sim(rl::Image& render_image, const SimState& sim_state, BS::thread_pool& pool)
{
    for (int col = 0; col < 8; col++) {
        pool.push_task([col, &sim_state, &render_image] {
            draw_column(render_image, sim_state, col, 40);
        });
    }
    pool.wait_for_tasks();
}

void main_loop(
    const int screen_width,
    const int screen_height,
    SimState& sim_state,
    util::FixedLoop& fixed_loop,
    rl::Image& render_image,
    rl::Texture& sim_texture,
    std::mt19937& rand_engine,
    BS::thread_pool& thread_pool)
{
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        rl::Vector2 mouse_pos = GetMousePosition();
        Vector2i sim_pos { (int)mouse_pos.x, (int)mouse_pos.y };
        if (sim_state.in_space(sim_pos)) {
            sim_state.particle_at(sim_pos).type = ParticleType::e_salt;
            sim_state.particle_at(sim_pos).shade = (float)GetRandomValue(750, 1000) / 1000.0f;
        }
    }
    else if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
        rl::Vector2 mouse_pos = GetMousePosition();
        Vector2i sim_pos { (int)mouse_pos.x, (int)mouse_pos.y };
        if (sim_state.in_space({ (int)mouse_pos.x, (int)mouse_pos.y })) {
            sim_state.particle_at(sim_pos).type = ParticleType::e_water;
            sim_state.particle_at(sim_pos).shade = 1.0f;
        }
    }
    else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        rl::Vector2 mouse_pos = GetMousePosition();
        Vector2i sim_pos { (int)mouse_pos.x, (int)mouse_pos.y };
        if (sim_state.in_space(sim_pos)) {
            sim_state.particle_at(sim_pos).type = ParticleType::e_null;
        }
    }

    fixed_loop.update(20, [&]() {
        update_sim(sim_state, rand_engine);
    });

    draw_sim(render_image, sim_state, thread_pool);

    sim_texture.Update(render_image.data);

    BeginDrawing();
    {
        ClearBackground(rl::Color::Black());

        sim_texture.Draw(
            rl::Rectangle(0, 0, (float)sim_state.width, (float)sim_state.height),
            rl::Rectangle(0, 0, (float)screen_width, (float)screen_height));

        DrawFPS(10, 10);
    }
    EndDrawing();
}

void run()
{
    const int screen_width = 1200;
    const int screen_height = 900;

    //    SetConfigFlags(FLAG_VSYNC_HINT);

    rl::Window window(screen_width, screen_height, "Powder Playground");

    SimState sim_state { .width = 320, .height = 240 };
    sim_state.space = std::vector<Particle>();
    for (int i = 0; i < sim_state.width * sim_state.height; i++) {
        sim_state.space.push_back({});
    }
    rl::Image render_image = rl::Image(sim_state.width, sim_state.height);

    for (int i = 0; i < sim_state.space.size(); i++) {
        Vector2i pos = sim_state.get_pos(i);
        render_image.DrawPixel(pos.x, pos.y, rl::Color::Black());
    }

    SetMouseScale(320.0f / 1200.0f, 320.0f / 1200.0f);

    rl::Texture2D texture(render_image);

    util::FixedLoop fixed_loop(240);

    BS::thread_pool pool;

    std::mt19937 rand_engine;

    while (!window.ShouldClose()) {
        main_loop(screen_width, screen_height, sim_state, fixed_loop, render_image, texture, rand_engine, pool);
    }
}

}