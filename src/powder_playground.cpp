#include "powder_playground.hpp"

#include <raylib-cpp.hpp>

namespace rl = raylib;

namespace pop {

void run()
{
    const int screen_width = 1200;
    const int screen_height = 900;

    rl::Window window(screen_width, screen_height, "Powder Playground");

    const int sim_width = 320;
    const int sim_height = 240;

    rl::Image image(sim_width, sim_height, rl::Color::Black());

    rl::Texture2D texture(image);

    while (!window.ShouldClose()) {

        for (int x = 0; x < sim_width; x++) {
            for (int y = 0; y < sim_height; y++) {
                image.DrawPixel(
                    rl::Vector2((float)x, (float)y),
                    rl::Color::FromHSV(0.0f, 0.0f, ((float)GetRandomValue(0, 1000)) / 1000.0f));
            }
        }

        texture.Update(image.data);

        BeginDrawing();
        {
            ClearBackground(rl::Color::Black());

            texture.Draw(rl::Rectangle(0, 0, sim_width, sim_height), rl::Rectangle(0, 0, screen_width, screen_height));

            DrawFPS(10, 10);
        }
        EndDrawing();
    }
}

}