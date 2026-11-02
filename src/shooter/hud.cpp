#include "hud.h"
#include <cmath>

void HUD::Draw(int health, int ammo, int score, int wave,
               const char* weaponName,
               int screenW, int screenH, float hurtFlash) const {
    // Hurt vignette
    if (hurtFlash > 0.0f) {
        unsigned char alpha = (unsigned char)(hurtFlash / 0.18f * 120.0f);
        DrawRectangle(0, 0, screenW, screenH, {200,0,0,alpha});
    }

    DrawCrosshair(screenW/2, screenH/2);
    DrawHealthBar(health, 20, screenH-50);

    // Weapon + ammo
    DrawText(TextFormat("%s  %d", weaponName, ammo),
             screenW-200, screenH-50, 22, WHITE);

    // Score / wave
    DrawText(TextFormat("SCORE: %d", score), 20, 20, 28, YELLOW);
    DrawText(TextFormat("WAVE %d",   wave),  20, 54, 22, {200,200,255,255});

    // Low-health pulse
    if (health < 30) {
        DrawText("LOW HEALTH!", screenW/2-90, screenH/2+60, 24, RED);
    }
    if (ammo == 0)
        DrawText("NO AMMO", screenW/2-60, screenH/2+90, 22, ORANGE);
}

void HUD::DrawCrosshair(int cx, int cy) const {
    int s=10, g=3;
    DrawRectangle(cx-s-g, cy-1, s, 3, WHITE);
    DrawRectangle(cx+g,   cy-1, s, 3, WHITE);
    DrawRectangle(cx-1, cy-s-g, 3, s, WHITE);
    DrawRectangle(cx-1, cy+g,   3, s, WHITE);
    DrawRectangle(cx-1, cy-1,   3, 3, {255,80,80,255});
}

void HUD::DrawHealthBar(int health, int x, int y) const {
    int bw=200, bh=22;
    DrawRectangle(x, y, bw, bh, {40,40,40,200});
    Color hc = (health>60) ? GREEN : (health>30) ? YELLOW : RED;
    DrawRectangle(x, y, bw*health/100, bh, hc);
    DrawRectangleLines(x, y, bw, bh, WHITE);
    DrawText(TextFormat("HP %d", health), x+6, y+3, 16, WHITE);
}
