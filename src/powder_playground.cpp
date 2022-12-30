#include "powder_playground.hpp"

#include <random>

#include <BS_thread_pool.hpp>
#include <raylib-cpp.hpp>

#include "util/fixed_loop.hpp"
#define LOGGER_RAYLIB
#include "common.hpp"
#include "simulation.hpp"
#include "util/logger.hpp"

namespace rl = raylib;

namespace pop {

struct GameState {
    int screen_width;
    int screen_height;

    SimState sim_state;

    util::FixedLoop fixed_loop;
    BS::thread_pool thread_pool;

    Element selected_type = Element::e_salt;

    rl::Image render_image;
    rl::Image gas_image;
    rl::Texture2D sim_texture;
    rl::Texture2D gas_texture;
    rl::Shader blur_shader;
    rl::RenderTexture2D render_texture;
};

void main_loop(GameState& game_state)
{
    SimState& sim_state = game_state.sim_state;

    if (IsKeyPressed(KEY_ONE)) {
        game_state.selected_type = Element::e_null;
    }
    else if (IsKeyPressed(KEY_TWO)) {
        game_state.selected_type = Element::e_wall;
    }
    else if (IsKeyPressed(KEY_THREE)) {
        game_state.selected_type = Element::e_salt;
    }
    else if (IsKeyPressed(KEY_FOUR)) {
        game_state.selected_type = Element::e_water;
    }
    else if (IsKeyPressed(KEY_FIVE)) {
        game_state.selected_type = Element::e_lava;
    }
    else if (IsKeyPressed(KEY_SIX)) {
        game_state.selected_type = Element::e_steam;
    }
    else if (IsKeyPressed(KEY_SEVEN)) {
        game_state.selected_type = Element::e_stone;
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        rl::Vector2 mouse_pos = GetMousePosition();
        Vector2i sim_pos { (int)mouse_pos.x, (int)mouse_pos.y };
        if (sim_state.in_bounds(sim_pos)) {
            sim_state.particle_at(sim_pos).element = game_state.selected_type;
            if (game_state.selected_type == Element::e_salt) {
                sim_state.particle_at(sim_pos).shade = (float)GetRandomValue(750, 1000) / 1000.0f;
            }
        }
    }
    else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        rl::Vector2 mouse_pos = GetMousePosition();
        Vector2i sim_pos { (int)mouse_pos.x, (int)mouse_pos.y };
        if (sim_state.in_bounds(sim_pos)) {
            sim_state.particle_at(sim_pos).element = Element::e_null;
        }
    }

    game_state.fixed_loop.update(20, [&]() {
        update_sim(sim_state);
    });

    draw_sim(game_state.render_image, game_state.gas_image, sim_state, game_state.thread_pool);

    game_state.sim_texture.Update(game_state.render_image.data);
    game_state.gas_texture.Update(game_state.gas_image.data);

    BeginDrawing();
    {
        ClearBackground(rl::Color(15, 15, 15));

        //        game_state.blur_shader.BeginMode();
        game_state.sim_texture.Draw(
            rl::Rectangle(0, 0, (float)sim_state.width, (float)sim_state.height),
            rl::Rectangle(0, 0, (float)game_state.screen_width, (float)game_state.screen_height));
        //        game_state.blur_shader.EndMode();

        game_state.render_texture.BeginMode();
        ClearBackground(rl::Color().Alpha(0));

        game_state.gas_texture.Draw(
            rl::Rectangle(0, 0, (float)sim_state.width, (float)sim_state.height),
            rl::Rectangle(0, 0, (float)game_state.screen_width, (float)game_state.screen_height));

        game_state.render_texture.EndMode();

        game_state.blur_shader.BeginMode();
        game_state.render_texture.GetTexture().Draw();
        game_state.blur_shader.EndMode();

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
    SetTraceLogCallback(util::logger_callback_raylib);

    rl::Window window(screen_width, screen_height, "Powder Playground");

    SimState sim_state { .width = 320, .height = 240 };
    sim_state.space = std::vector<Particle>();
    for (int i = 0; i < sim_state.width * sim_state.height; i++) {
        sim_state.space.push_back({});
    }

    LOG->set_level(spdlog::level::err);
    GameState game_state {
        .screen_width = screen_width,
        .screen_height = screen_height,
        .sim_state = sim_state,
        .fixed_loop = util::FixedLoop(240),
        .thread_pool {},
        .selected_type = Element::e_salt,
        .render_image { game_state.sim_state.width, game_state.sim_state.height },
        .gas_image { game_state.sim_state.width, game_state.sim_state.height },
        .sim_texture { game_state.render_image },
        .gas_texture { game_state.gas_image },
        .blur_shader { nullptr, "../res/blur.frag" },
        .render_texture { 1200, 900 },
    };
    LOG->set_level(spdlog::level::info);

    game_state.render_texture.GetTexture().SetWrap(TEXTURE_WRAP_CLAMP);

    const rl::Vector2 blur_shader_resolution { screen_width, screen_height };
    game_state.blur_shader.SetValue(
        game_state.blur_shader.GetLocation("resolution"), &blur_shader_resolution, SHADER_UNIFORM_VEC2);

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