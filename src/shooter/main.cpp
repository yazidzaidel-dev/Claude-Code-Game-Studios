// ============================================================
//  CCGS Shooter — Native 3D First-Person Shooter
//  Engine: Raylib 5.0 | Language: C++17 | Platform: Linux/ARM
// ============================================================
#include "raylib.h"
#include "raymath.h"
#include "game.h"
#include <cstdio>

int main() {
    const int SCREEN_W = 1280;
    const int SCREEN_H = 720;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(SCREEN_W, SCREEN_H, "CCGS Shooter");
    SetTargetFPS(60);
    DisableCursor();

    Game game;
    game.Init();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        game.Update(dt);

        BeginDrawing();
            ClearBackground(BLACK);
            game.Draw();
        EndDrawing();
    }

    game.Shutdown();
    CloseWindow();
    return 0;
}
