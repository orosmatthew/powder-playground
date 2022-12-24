#include "powder_playground.hpp"

#include <cassert>
#include <iostream>
#include <random>

#include <BS_thread_pool.hpp>
#include <raylib-cpp.hpp>

#include "fixed_loop.hpp"

namespace rl = raylib;

namespace pop {

enum class ParticleType {
    e_null,
    e_wall,
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

struct GameState {
    int screen_width;
    int screen_height;

    SimState sim_state;

    std::mt19937 rand_engine;
    util::FixedLoop fixed_loop;
    BS::thread_pool thread_pool;

    ParticleType selected_type = ParticleType::e_salt;

    rl::Image render_image;
    rl::Texture2D sim_texture;
};

std::string to_string(ParticleType type)
{
    switch (type) {
    case ParticleType::e_null:
        return "Air";
    case ParticleType::e_wall:
        return "Wall";
    case ParticleType::e_salt:
        return "Salt";
    case ParticleType::e_water:
        return "Water";
    default:
        return "";
    }
}

void update_salt(SimState& sim_state, Vector2i particle_pos)
{
    Vector2i bottom_pos { particle_pos.x, particle_pos.y + 1 };
    if (sim_state.in_bounds(bottom_pos)
        && (sim_state.particle_at(bottom_pos).type == ParticleType::e_null
            || sim_state.particle_at(bottom_pos).type == ParticleType::e_water)) {
        if (GetRandomValue(0, 5) < 5) {
            sim_state.swap(particle_pos, bottom_pos);
        }
        return;
    }

    int rand_side = GetRandomValue(0, 1);
    if (rand_side == 0) {
        rand_side = -1;
    }
    Vector2i side_pos { particle_pos.x + rand_side, particle_pos.y + 1 };
    if (sim_state.in_bounds(side_pos)
        && (sim_state.particle_at(side_pos).type == ParticleType::e_null
            || sim_state.particle_at(side_pos).type == ParticleType::e_water)) {
        sim_state.swap(particle_pos, side_pos);
        return;
    }
}

void update_water(SimState& sim_state, Vector2i particle_pos)
{
    Vector2i bottom_pos { particle_pos.x, particle_pos.y + 1 };
    if (sim_state.in_bounds(bottom_pos) && sim_state.particle_at(bottom_pos).type == ParticleType::e_null) {
        if (GetRandomValue(0, 5) < 5) {
            sim_state.swap(particle_pos, bottom_pos);
        }
        return;
    }

    int sides[2];
    int rand_side = GetRandomValue(0, 1);
    if (rand_side == 0) {
        sides[0] = -1;
        sides[1] = 1;
    }
    else {
        sides[0] = 1;
        sides[1] = -1;
    }
    for (int side : sides) {
        Vector2i side_below_pos { particle_pos.x + side, particle_pos.y };
        if (sim_state.in_bounds(side_below_pos) && sim_state.particle_at(side_below_pos).type == ParticleType::e_null) {
            sim_state.swap(particle_pos, side_below_pos);
            return;
        }
    }

    Vector2i side_pos { particle_pos.x + rand_side, particle_pos.y };
    if (sim_state.in_bounds(side_pos) && sim_state.particle_at(side_pos).type == ParticleType::e_null) {
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
    case ParticleType::e_wall:
        break;
    }
}

void update_sim(SimState& sim_state, std::mt19937& rand_engine)
{
    std::vector<int> rand_indices;
    rand_indices.reserve(sim_state.width);
    for (int i = 0; i < sim_state.width; i++) {
        rand_indices.push_back(i);
    }

    for (int y = sim_state.height - 1; y >= 0; y--) {
        std::shuffle(rand_indices.begin(), rand_indices.end(), rand_engine);
        for (int x : rand_indices) {
            update_particle(sim_state, { x, y });
        }
    }
}

void draw_particle(rl::Image& render_image, const SimState& sim_state, Vector2i pos)
{
    switch (sim_state.particle_at(pos).type) {
    case ParticleType::e_null:
        render_image.DrawPixel(pos.x, pos.y, rl::Color(15, 15, 15));
        break;
    case ParticleType::e_salt:
        render_image.DrawPixel(pos.x, pos.y, rl::Color::FromHSV(0.0f, 0.0f, sim_state.particle_at(pos).shade));
        break;
    case ParticleType::e_water:
        render_image.DrawPixel(pos.x, pos.y, rl::Color::FromHSV(243.0f, 0.9f, 1.0f));
        break;
    case ParticleType::e_wall:
        render_image.DrawPixel(pos.x, pos.y, rl::Color(120, 120, 120));
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
    assert(render_image.width == sim_state.width && render_image.height == sim_state.height);

    for (int col = 0; col < 8; col++) {
        pool.push_task([col, &sim_state, &render_image] {
            draw_column(render_image, sim_state, col, 40);
        });
    }
    pool.wait_for_tasks();
}

void main_loop(GameState& game_state)
{
    SimState& sim_state = game_state.sim_state;

    if (IsKeyPressed(KEY_ONE)) {
        game_state.selected_type = ParticleType::e_null;
    }
    else if (IsKeyPressed(KEY_TWO)) {
        game_state.selected_type = ParticleType::e_wall;
    }
    else if (IsKeyPressed(KEY_THREE)) {
        game_state.selected_type = ParticleType::e_salt;
    }
    else if (IsKeyPressed(KEY_FOUR)) {
        game_state.selected_type = ParticleType::e_water;
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        rl::Vector2 mouse_pos = GetMousePosition();
        Vector2i sim_pos { (int)mouse_pos.x, (int)mouse_pos.y };
        if (sim_state.in_bounds(sim_pos)) {
            sim_state.particle_at(sim_pos).type = game_state.selected_type;
            if (game_state.selected_type == ParticleType::e_salt) {
                sim_state.particle_at(sim_pos).shade = (float)GetRandomValue(750, 1000) / 1000.0f;
            }
        }
    }
    else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        rl::Vector2 mouse_pos = GetMousePosition();
        Vector2i sim_pos { (int)mouse_pos.x, (int)mouse_pos.y };
        if (sim_state.in_bounds(sim_pos)) {
            sim_state.particle_at(sim_pos).type = ParticleType::e_null;
        }
    }

    game_state.fixed_loop.update(20, [&]() {
        update_sim(sim_state, game_state.rand_engine);
    });

    draw_sim(game_state.render_image, sim_state, game_state.thread_pool);

    game_state.sim_texture.Update(game_state.render_image.data);

    BeginDrawing();
    {
        ClearBackground(rl::Color(15, 15, 15));

        game_state.sim_texture.Draw(
            rl::Rectangle(0, 0, (float)sim_state.width, (float)sim_state.height),
            rl::Rectangle(0, 0, (float)game_state.screen_width, (float)game_state.screen_height));

        DrawFPS(10, 10);

        rl::DrawText(to_string(game_state.selected_type), 10, 50, 20, rl::Color::Yellow());
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

    GameState game_state {
        .screen_width = screen_width,
        .screen_height = screen_height,
        .sim_state = std::move(sim_state),
        .rand_engine {},
        .fixed_loop { 240 },
        .thread_pool {},
        .selected_type = ParticleType::e_salt,
        .render_image { game_state.sim_state.width, game_state.sim_state.height },
        .sim_texture { game_state.render_image }
    };

    for (int i = 0; i < game_state.sim_state.space.size(); i++) {
        Vector2i pos = game_state.sim_state.pos_at(i);
        game_state.render_image.DrawPixel(pos.x, pos.y, rl::Color(15, 15, 15));
    }

    SetMouseScale(320.0f / 1200.0f, 320.0f / 1200.0f);

    while (!window.ShouldClose()) {
        main_loop(game_state);
    }
}

}