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

    Simulation simulation;

    util::FixedLoop fixed_loop;
    BS::thread_pool thread_pool;

    ElementId selected_element = 0;

    rl::Image powder_image;
    rl::Image gas_image;
    rl::Texture2D powder_texture;
    rl::Texture2D gas_texture;
    rl::Shader blur_shader;
    rl::RenderTexture2D gas_render_texture;
};

void main_loop(GameState& game_state)
{
    Simulation& simulation = game_state.simulation;

    if (IsKeyPressed(KEY_ONE)) {
        game_state.selected_element = simulation.id_of("air");
    }
    else if (IsKeyPressed(KEY_TWO)) {
        game_state.selected_element = simulation.id_of("wall");
    }
    else if (IsKeyPressed(KEY_THREE)) {
        game_state.selected_element = simulation.id_of("salt");
    }
    else if (IsKeyPressed(KEY_FOUR)) {
        game_state.selected_element = simulation.id_of("water");
    }
    else if (IsKeyPressed(KEY_FIVE)) {
        game_state.selected_element = simulation.id_of("lava");
    }
    else if (IsKeyPressed(KEY_SIX)) {
        game_state.selected_element = simulation.id_of("steam");
    }
    else if (IsKeyPressed(KEY_SEVEN)) {
        game_state.selected_element = simulation.id_of("stone");
    }
    else if (IsKeyPressed(KEY_EIGHT)) {
        game_state.selected_element = simulation.id_of("toxic_gas");
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        rl::Vector2 mouse_pos = GetMousePosition();
        Vector2i sim_pos { (int)mouse_pos.x, (int)mouse_pos.y };
        if (simulation.in_bounds(sim_pos)) {
            simulation.particle_at(sim_pos).element_id = game_state.selected_element;
            if (game_state.selected_element == simulation.id_of("salt")) {
                simulation.particle_at(sim_pos).shade = (float)GetRandomValue(750, 1000) / 1000.0f;
            }
        }
    }
    else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        rl::Vector2 mouse_pos = GetMousePosition();
        Vector2i sim_pos { (int)mouse_pos.x, (int)mouse_pos.y };
        if (simulation.in_bounds(sim_pos)) {
            simulation.particle_at(sim_pos).element_id = simulation.id_of("air");
        }
    }

    game_state.fixed_loop.update(20, [&]() { simulation.update(); });

    game_state.powder_image.ClearBackground(rl::Color().Alpha(0));
    game_state.gas_image.ClearBackground(rl::Color().Alpha(0));

    draw_sim(game_state.powder_image, game_state.gas_image, simulation, game_state.thread_pool);

    game_state.powder_texture.Update(game_state.powder_image.data);
    game_state.gas_texture.Update(game_state.gas_image.data);

    BeginDrawing();
    {
        ClearBackground(rl::Color(15, 15, 15));

        game_state.powder_texture.Draw(
            rl::Rectangle(0, 0, (float)simulation.width(), (float)simulation.height()),
            rl::Rectangle(0, 0, (float)game_state.screen_width, (float)game_state.screen_height));

        game_state.gas_render_texture.BeginMode();
        ClearBackground(rl::Color().Alpha(0));
        game_state.gas_texture.Draw(
            rl::Rectangle(0, 0, (float)simulation.width(), (float)simulation.height()),
            rl::Rectangle(0, 0, (float)game_state.screen_width, (float)game_state.screen_height));
        game_state.gas_render_texture.EndMode();

        game_state.blur_shader.BeginMode();
        game_state.gas_render_texture.GetTexture().Draw();
        game_state.blur_shader.EndMode();

        DrawFPS(10, 10);

        rl::DrawText(simulation.element_of(game_state.selected_element).friendly_name, 10, 50, 20, rl::Color::Yellow());
    }
    EndDrawing();
}

void init_elements(Simulation& simulation)
{
    Element air {};
    air.name = "air";
    air.friendly_name = "Air";
    air.type = ElementType::e_null;
    air.update_func = [](Simulation&, Vector2i) {};
    air.color = rl::Color(15, 15, 15);
    simulation.push_element(air);

    Element wall {};
    wall.name = "wall";
    wall.friendly_name = "Wall";
    wall.type = ElementType::e_solid;
    wall.update_func = [](Simulation&, Vector2i) {};
    wall.color = rl::Color(120, 120, 120);
    simulation.push_element(wall);

    Element salt {};
    salt.name = "salt";
    salt.friendly_name = "Salt";
    salt.type = ElementType::e_powder;
    salt.update_func = update_salt;
    salt.color = rl::Color::FromHSV(0.0f, 0.0f, 1.0f);
    simulation.push_element(salt);

    Element water {};
    water.name = "water";
    water.friendly_name = "Water";
    water.type = ElementType::e_liquid;
    water.update_func = update_water;
    water.color = rl::Color::FromHSV(243.0f, 0.9f, 1.0f);
    simulation.push_element(water);

    Element lava {};
    lava.name = "lava";
    lava.friendly_name = "Lava";
    lava.type = ElementType::e_liquid;
    lava.update_func = update_lava;
    lava.color = rl::Color(255, 94, 0);
    simulation.push_element(lava);

    Element steam {};
    steam.name = "steam";
    steam.friendly_name = "Steam";
    steam.type = ElementType::e_gas;
    steam.update_func = update_steam;
    steam.color = rl::Color(106, 194, 255);
    simulation.push_element(steam);

    Element stone {};
    stone.name = "stone";
    stone.friendly_name = "Stone";
    stone.type = ElementType::e_powder;
    stone.update_func = update_stone;
    stone.color = rl::Color(140, 140, 140);
    simulation.push_element(stone);

    Element toxic_gas {};
    toxic_gas.name = "toxic_gas";
    toxic_gas.friendly_name = "Toxic Gas";
    toxic_gas.type = ElementType::e_gas;
    toxic_gas.update_func = update_toxic_gas;
    toxic_gas.color = rl::Color(165, 185, 0);
    simulation.push_element(toxic_gas);
}

void run()
{
    const int screen_width = 1200;
    const int screen_height = 900;

    SetConfigFlags(FLAG_VSYNC_HINT);
    SetTraceLogCallback(util::logger_callback_raylib);

    rl::Window window(screen_width, screen_height, "Powder Playground");

    Simulation simulation(320, 240);

    init_elements(simulation);
    simulation.clear_to("air");

    LOG->set_level(spdlog::level::err);
    GameState game_state {
        .screen_width = screen_width,
        .screen_height = screen_height,
        .simulation = std::move(simulation),
        .fixed_loop = util::FixedLoop(240),
        .thread_pool {},
        .selected_element = 1,
        .powder_image { 320, 240 },
        .gas_image { 320, 240 },
        .powder_texture { game_state.powder_image },
        .gas_texture { game_state.gas_image },
        .blur_shader { nullptr, "res/blur.frag" },
        .gas_render_texture { 1200, 900 },
    };
    LOG->set_level(spdlog::level::info);

    game_state.gas_render_texture.GetTexture().SetWrap(TEXTURE_WRAP_CLAMP);

    const rl::Vector2 blur_shader_resolution { screen_width, screen_height };
    game_state.blur_shader.SetValue(
        game_state.blur_shader.GetLocation("resolution"), &blur_shader_resolution, SHADER_UNIFORM_VEC2);

    SetMouseScale(320.0f / 1200.0f, 320.0f / 1200.0f);

    while (!window.ShouldClose()) {
        main_loop(game_state);
    }
}

}