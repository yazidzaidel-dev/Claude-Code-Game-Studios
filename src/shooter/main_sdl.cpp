// ============================================================
//  CCGS Shooter — SDL2 + OpenGL Native Backend (Termux / Linux)
// ============================================================
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <cstdio>
#include "game.h"

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        std::fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    int screenW = 1280;
    int screenH = 720;

    SDL_Window* window = SDL_CreateWindow(
        "CCGS Shooter (SDL2 Native)",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenW, screenH,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::fprintf(stderr, "SDL_GL_CreateContext Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_GL_SetSwapInterval(1); // VSync
    SDL_SetRelativeMouseMode(SDL_TRUE);

    // Initialize Raylib audio/window bridge or run our game loop using SDL events
    // Since our game architecture uses Raylib for rendering/audio/math,
    // we can either map Raylib window calls to SDL2 or provide an SDL2 main loop wrapper.
    // Let's create an SDL2 main loop that calls game.Update / game.Draw via Raylib or OpenGL.
    // Wait, Raylib itself can run on SDL2 if configured, or we can use Raylib's window init.
    // But wait! Raylib's InitWindow uses GLFW. To run without X11 (e.g. via direct OpenGL ES / EGL / KMS or SDL),
    // let's check if Raylib supports SDL or if SDL2 can create the OpenGL context that Raylib uses.
    // Actually, Raylib 5.0 has a PLATFORM_SDL or we can drive the game logic directly.

    std::fprintf(stdout, "SDL2 Window & OpenGL Context initialized successfully.\n");

    bool running = true;
    SDL_Event e;
    Uint32 lastTick = SDL_GetTicks();

    while (running) {
        Uint32 currentTick = SDL_GetTicks();
        float dt = (currentTick - lastTick) / 1000.0f;
        if (dt > 0.1f) dt = 0.1f;
        lastTick = currentTick;

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
            }
        }

        // Clear screen
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SDL_GL_SwapWindow(window);
        SDL_Delay(16);
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
