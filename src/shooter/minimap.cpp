#include "minimap.h"
#include <cmath>

Vector2 Minimap::WorldToMap(float wx, float wz, Vector2 origin) const {
    float half = worldSize * 0.5f;
    float nx   = (wx + half) / worldSize;   // 0..1
    float nz   = (wz + half) / worldSize;
    return { origin.x + nx * mapSize, origin.y + nz * mapSize };
}

void Minimap::DrawPlayerArrow(Vector2 c, float yaw) const {
    // Equilateral triangle pointing in yaw direction
    // yaw=0 → facing -Z (north on map = up)
    float angle = yaw - 1.5708f; // rotate so 0 = up
    float r = playerR;

    auto rot = [&](float dx, float dy) -> Vector2 {
        float ca = cosf(angle), sa = sinf(angle);
        return { c.x + ca*dx - sa*dy, c.y + sa*dx + ca*dy };
    };

    Vector2 tip  = rot(0,        -r * 1.6f);
    Vector2 left = rot(-r,        r);
    Vector2 rght = rot( r,        r);

    DrawTriangle(tip, left, rght, { 80, 220, 80, 255 });
    DrawTriangleLines(tip, left, rght, WHITE);
}

void Minimap::Draw(
        const Vector3&              playerPos,
        float                       playerYaw,
        const std::vector<Enemy>&   enemies,
        const std::vector<WeaponPickup>& pickups,
        const std::vector<BoundingBox>& walls,
        int screenW, int screenH) const {

    // Position: bottom-right corner
    Vector2 origin = {
        screenW - mapSize - margin,
        screenH - mapSize - margin
    };

    // --- Background + border ---
    DrawRectangle((int)origin.x - 2, (int)origin.y - 2,
                  (int)mapSize + 4, (int)mapSize + 4, { 0,0,0,200 });
    DrawRectangle((int)origin.x, (int)origin.y,
                  (int)mapSize, (int)mapSize, { 20, 30, 20, 210 });
    DrawRectangleLines((int)origin.x,(int)origin.y,
                       (int)mapSize,(int)mapSize, { 80,120,80,220 });

    // --- Walls (draw as dark rectangles) ---
    float scale = mapSize / worldSize;
    for (const auto& wb : walls) {
        float cx = (wb.min.x + wb.max.x) * 0.5f;
        float cz = (wb.min.z + wb.max.z) * 0.5f;
        float sx = (wb.max.x - wb.min.x) * scale;
        float sz = (wb.max.z - wb.min.z) * scale;
        Vector2 mc = WorldToMap(cx, cz, origin);
        DrawRectangle(
            (int)(mc.x - sx * 0.5f),
            (int)(mc.y - sz * 0.5f),
            (int)sx, (int)sz,
            { 80, 90, 100, 200 }
        );
    }

    // --- Weapon pickups ---
    for (const auto& p : pickups) {
        if (!p.active) continue;
        Vector2 mp = WorldToMap(p.position.x, p.position.z, origin);
        Color pc   = GetWeaponDef(p.type).bulletColor;
        DrawCircle((int)mp.x, (int)mp.y, dotR + 1.0f, { pc.r, pc.g, pc.b, 200 });
        // Small cross to distinguish from enemies
        DrawLine((int)mp.x-4,(int)mp.y,(int)mp.x+4,(int)mp.y, WHITE);
        DrawLine((int)mp.x,(int)mp.y-4,(int)mp.x,(int)mp.y+4, WHITE);
    }

    // --- Enemies ---
    for (const auto& e : enemies) {
        if (e.IsGone()) continue;
        Vector2 me = WorldToMap(e.position.x, e.position.z, origin);
        Color ec;
        if (e.state == EnemyState::DYING)   ec = { 200, 100, 20, 180 };
        else if (e.state==EnemyState::ATTACK) ec = { 255,  40, 40, 230 };
        else if (e.state==EnemyState::CHASE)  ec = { 255, 140, 20, 230 };
        else                                  ec = { 160, 160, 160, 200 };
        DrawCircle((int)me.x, (int)me.y, (int)dotR, ec);
        DrawCircleLines((int)me.x,(int)me.y,(int)dotR,{0,0,0,180});
    }

    // --- Player ---
    Vector2 mp = WorldToMap(playerPos.x, playerPos.z, origin);
    DrawPlayerArrow(mp, playerYaw);

    // --- Label ---
    DrawText("MAP", (int)origin.x + 4, (int)origin.y + 4, 12, { 100,160,100,200 });
}
