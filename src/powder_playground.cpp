#include "powder_playground.hpp"

#include <iostream>

#include <raylib-cpp.hpp>

#include "fixed_loop.hpp"

namespace rl = raylib;

namespace pop {

struct Particle {
    bool exists = false;
    float shade = 1.0f;
};

struct SimState {
    const int width;
    const int height;
    std::vector<Particle> space;
};

size_t get_index(size_t row, size_t col, size_t width)
{
    return width * col + row;
}

void update_sim(SimState& sim_state)
{
    std::vector<Particle> prev_space = sim_state.space;
    for (int x = 0; x < sim_state.width; x++) {
        for (int y = 0; y < sim_state.height; y++) {
            size_t index = get_index(x, y, sim_state.width);
            if (prev_space.at(index).exists) {
                if (y + 1 < sim_state.height) {
                    if (!prev_space.at(get_index(x, y + 1, sim_state.width)).exists) {
                        std::swap(sim_state.space[index], sim_state.space[get_index(x, y + 1, sim_state.width)]);
                    }
                    else {
                        int val = GetRandomValue(0, 1);
                        if (val == 0) {
                            val = -1;
                        }
                        rl::Vector2 pos((float)(x + val), (float)(y + 1));
                        if (pos.CheckCollision(rl::Rectangle(0, 0, (float)sim_state.width, (float)sim_state.height))) {
                            if (!prev_space.at(get_index((int)pos.x, (int)pos.y, sim_state.width)).exists) {
                                std::swap(
                                    sim_state.space[index],
                                    sim_state.space[get_index((int)pos.x, (int)pos.y, sim_state.width)]);
                            }
                        }
                    }
                }
            }
        }
    }
}

void draw_sim(rl::Image& render_image, const SimState& sim_state)
{
    for (int x = 0; x < sim_state.width; x++) {
        for (int y = 0; y < sim_state.height; y++) {
            size_t index = get_index(x, y, sim_state.width);
            if (sim_state.space.at(index).exists) {
                render_image.DrawPixel(x, y, rl::Color::FromHSV(0.0f, 0.0f, sim_state.space.at(index).shade));
            }
            else {
                render_image.DrawPixel(x, y, rl::Color::Black());
            }
        }
    }
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

    util::FixedLoop fixed_loop(2000);

    while (!window.ShouldClose()) {

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            rl::Vector2 mouse_pos = GetMousePosition();
            sim_state.space.at(get_index((size_t)mouse_pos.x, (size_t)mouse_pos.y, sim_state.width)).exists = true;
            sim_state.space.at(get_index((size_t)mouse_pos.x, (size_t)mouse_pos.y, sim_state.width)).shade
                = (float)GetRandomValue(750, 1000) / 1000.0f;
        }
        else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            rl::Vector2 mouse_pos = GetMousePosition();
            sim_state.space.at(get_index((size_t)mouse_pos.x, (size_t)mouse_pos.y, sim_state.width)).exists = false;
        }

        fixed_loop.update(20, [&]() { update_sim(sim_state); });

        draw_sim(render_image, sim_state);

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