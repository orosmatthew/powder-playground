#include "powder_playground.hpp"

#include <iostream>
#include <random>

#include <BS_thread_pool.hpp>
#include <raylib-cpp.hpp>

#include "fixed_loop.hpp"

namespace rl = raylib;

namespace pop {

struct Particle {
    bool exists = false;
    bool modified = false;
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

void update_sim(SimState& sim_state, std::mt19937& rand_engine)
{
    std::vector<int> rand_indices;
    rand_indices.reserve(sim_state.space.size());
    for (int i = 0; i < sim_state.space.size(); i++) {
        rand_indices.push_back(i);
    }

    std::shuffle(rand_indices.begin(), rand_indices.end(), rand_engine);

    for (Particle& p : sim_state.space) {
        p.modified = false;
    }

    for (int i : rand_indices) {
        if (sim_state.space.at(i).modified || !sim_state.space.at(i).exists) {
            continue;
        }

        Vector2i pos = sim_state.get_pos(i);

        if (!sim_state.in_space({ pos.x, pos.y + 1 })) {
            continue;
        }

        Vector2i bottom_pos { pos.x, pos.y + 1 };
        if (!sim_state.particle_at(bottom_pos).exists && !sim_state.particle_at(bottom_pos).modified) {
            sim_state.swap(pos, bottom_pos);
            sim_state.space.at(i).modified = true;
            sim_state.particle_at(bottom_pos).modified = true;
        }
        else {
            int val = GetRandomValue(0, 1);
            if (val == 0) {
                val = -1;
            }
            Vector2i diag_pos { pos.x + val, pos.y + 1 };
            if (sim_state.in_space(diag_pos) && !sim_state.particle_at(diag_pos).exists
                && !sim_state.particle_at(diag_pos).modified) {
                sim_state.swap(pos, diag_pos);
                sim_state.space.at(i).modified = true;
                sim_state.particle_at(diag_pos).modified = true;
            }
        }
    }

    //    for (int x = 0; x < sim_state.width; x++) {
    //        for (int y = 0; y < sim_state.height; y++) {
    //            size_t index = get_index(x, y, sim_state.width);
    //            if (prev_space.at(index).exists) {
    //                if (y + 1 < sim_state.height) {
    //                    if (!prev_space.at(get_index(x, y + 1, sim_state.width)).exists) {
    //                        std::swap(sim_state.space[index], sim_state.space[get_index(x, y + 1, sim_state.width)]);
    //                    }
    //                    else {
    //                        int val = GetRandomValue(0, 1);
    //                        if (val == 0) {
    //                            val = -1;
    //                        }
    //                        rl::Vector2 pos((float)(x + val), (float)(y + 1));
    //                        if (pos.CheckCollision(rl::Rectangle(0, 0, (float)sim_state.width,
    //                        (float)sim_state.height))) {
    //                            if (!prev_space.at(get_index((int)pos.x, (int)pos.y, sim_state.width)).exists) {
    //                                std::swap(
    //                                    sim_state.space[index],
    //                                    sim_state.space[get_index((int)pos.x, (int)pos.y, sim_state.width)]);
    //                            }
    //                        }
    //                    }
    //                }
    //            }
    //        }
    //    }
}

void draw_sim(rl::Image& render_image, const SimState& sim_state, BS::thread_pool& pool)
{
    for (int t = 0; t < 8; t++) {
        pool.push_task([t, &sim_state, &render_image] {
            for (int x = t * 40; x < (t + 1) * 40; x++) {
                for (int y = 0; y < sim_state.height; y++) {
                    int index = sim_state.get_index({ x, y });
                    if (sim_state.space.at(index).exists) {
                        render_image.DrawPixel(x, y, rl::Color::FromHSV(0.0f, 0.0f, sim_state.space.at(index).shade));
                    }
                    else {
                        render_image.DrawPixel(x, y, rl::Color::Black());
                    }
                }
            }
        });
    }
    pool.wait_for_tasks();
    //    for (int x = 0; x < sim_state.width; x++) {
    //        for (int y = 0; y < sim_state.height; y++) {
    //            size_t index = get_index(x, y, sim_state.width);
    //            if (sim_state.space.at(index).exists) {
    //                render_image.DrawPixel(x, y, rl::Color::FromHSV(0.0f, 0.0f, sim_state.space.at(index).shade));
    //            }
    //            else {
    //                render_image.DrawPixel(x, y, rl::Color::Black());
    //            }
    //        }
    //    }
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
        sim_state.space.push_back(Particle {});
    }
    rl::Image render_image = rl::Image(sim_state.width, sim_state.height);

    for (int y = 0; y < sim_state.height; y++) {
        for (int x = 0; x < sim_state.width; x++) {
            render_image.DrawPixel(
                rl::Vector2((float)x, (float)y),
                rl::Color::FromHSV(0.0f, 0.0f, ((float)GetRandomValue(0, 1000)) / 1000.0f));
        }
    }

    SetMouseScale(320.0f / 1200.0f, 320.0f / 1200.0f);

    rl::Texture2D texture(render_image);

    util::FixedLoop fixed_loop(120);

    BS::thread_pool pool;

    std::mt19937 rand_engine;

    while (!window.ShouldClose()) {

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            rl::Vector2 mouse_pos = GetMousePosition();
            if (sim_state.in_space({ (int)mouse_pos.x, (int)mouse_pos.y })) {
                sim_state.space.at(sim_state.get_index({ (int)mouse_pos.x, (int)mouse_pos.y })).exists = true;
                sim_state.space.at(sim_state.get_index({ (int)mouse_pos.x, (int)mouse_pos.y })).shade
                    = (float)GetRandomValue(750, 1000) / 1000.0f;
            }
        }
        else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            rl::Vector2 mouse_pos = GetMousePosition();
            if (sim_state.in_space({ (int)mouse_pos.x, (int)mouse_pos.y })) {
                sim_state.space.at(sim_state.get_index({ (int)mouse_pos.x, (int)mouse_pos.y })).exists = false;
            }
        }

        fixed_loop.update(20, [&]() {
            update_sim(sim_state, rand_engine);
        });

        draw_sim(render_image, sim_state, pool);

        texture.Update(render_image.data);

        BeginDrawing();
        {
            ClearBackground(rl::Color::Black());

            texture.Draw(
                rl::Rectangle(0, 0, (float)sim_state.width, (float)sim_state.height),
                rl::Rectangle(0, 0, screen_width, screen_height));

            DrawFPS(10, 10);
        }
        EndDrawing();
    }
}

}