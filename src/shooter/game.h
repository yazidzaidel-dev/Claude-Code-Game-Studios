#pragma once
#include "raylib.h"
#include "raymath.h"
#include "player.h"
#include "enemy.h"
#include "level.h"
#include "hud.h"
#include "audio.h"
#include "weapon.h"
#include "minimap.h"
#include <vector>

enum class GameState { MENU, PLAYING, PAUSED, GAME_OVER };

class Game {
public:
    void Init();
    void Update(float dt);
    void Draw();
    void Shutdown();

    int  score = 0;
    int  wave  = 1;

private:
    GameState        state = GameState::MENU;
    Player           player;
    Level            level;
    HUD              hud;
    AudioManager     audio;
    Minimap          minimap;
    std::vector<Enemy> enemies;
    Camera3D         camera = {};

    float            ambientTimer = 0.0f;  // for looping ambient drone

    void SpawnWave();
    void CheckCollisions();
    void UpdateCamera();
    void DrawScene();
    void DrawMenu();
    void DrawPause();
    void DrawGameOver();
};
