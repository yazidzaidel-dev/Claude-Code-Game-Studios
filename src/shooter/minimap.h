#pragma once
#include "raylib.h"
#include "raymath.h"
#include "enemy.h"
#include "weapon.h"
#include <vector>

class Minimap {
public:
    // Config
    float mapSize    = 160.0f;   // pixel size of the map square
    float worldSize  = 60.0f;    // matches Level::floorSize
    float margin     = 14.0f;    // px from screen edge
    float dotR       = 3.5f;     // enemy dot radius
    float playerR    = 5.0f;     // player triangle half-size

    void Draw(
        const Vector3&              playerPos,
        float                       playerYaw,
        const std::vector<Enemy>&   enemies,
        const std::vector<WeaponPickup>& pickups,
        const std::vector<BoundingBox>& walls,
        int screenW, int screenH) const;

private:
    // Convert world XZ → minimap pixel, origin = top-left of map rect
    Vector2 WorldToMap(float wx, float wz, Vector2 origin) const;
    void DrawPlayerArrow(Vector2 center, float yaw) const;
};
