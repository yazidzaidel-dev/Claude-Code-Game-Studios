#pragma once
#include "raylib.h"

class HUD {
public:
    void Draw(int health, int ammo, int score, int wave,
              const char* weaponName,
              int screenW, int screenH, float hurtFlash) const;
private:
    void DrawCrosshair(int cx, int cy) const;
    void DrawHealthBar(int health, int x, int y) const;
};
