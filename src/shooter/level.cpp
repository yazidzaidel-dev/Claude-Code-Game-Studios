#include "level.h"
#include <cmath>

void Level::AddWall(Vector3 center, Vector3 size, Color color) {
    Wall w;
    w.center = center;
    w.size   = size;
    w.color  = color;
    w.bounds = {
        { center.x-size.x*0.5f, center.y-size.y*0.5f, center.z-size.z*0.5f },
        { center.x+size.x*0.5f, center.y+size.y*0.5f, center.z+size.z*0.5f }
    };
    walls.push_back(w);
}

void Level::Build() {
    walls.clear();
    spawnPoints.clear();
    pickups.clear();

    float h = 4.0f, hs = floorSize * 0.5f;
    Color wc = {80,90,100,255};
    AddWall({ 0, h*0.5f,-hs }, {floorSize, h, 1.0f}, wc);
    AddWall({ 0, h*0.5f, hs }, {floorSize, h, 1.0f}, wc);
    AddWall({-hs, h*0.5f, 0}, {1.0f, h, floorSize}, wc);
    AddWall({ hs, h*0.5f, 0}, {1.0f, h, floorSize}, wc);

    Color ic = {110,80,60,255};
    AddWall({ 8, h*0.5f,  5}, {5.0f,h,1.0f}, ic);
    AddWall({-8, h*0.5f, -5}, {5.0f,h,1.0f}, ic);
    AddWall({ 0, h*0.5f, 12}, {1.0f,h,6.0f}, ic);
    AddWall({ 0, h*0.5f,-12}, {1.0f,h,6.0f}, ic);
    AddWall({ 15,h*0.5f,  0}, {4.0f,h,4.0f},{70,100,70,255});
    AddWall({-15,h*0.5f,  0}, {4.0f,h,4.0f},{70,100,70,255});

    spawnPoints.push_back({{ 20,1.0f, 20},{15,1.0f,15}});
    spawnPoints.push_back({{-20,1.0f,-20},{-15,1.0f,-15}});
    spawnPoints.push_back({{ 20,1.0f,-20},{14,1.0f,-14}});
    spawnPoints.push_back({{-20,1.0f, 20},{-14,1.0f,14}});
    spawnPoints.push_back({{ 0,1.0f, 22},{ 0,1.0f,15}});
    spawnPoints.push_back({{ 0,1.0f,-22},{ 0,1.0f,-15}});
}

void Level::SpawnPickups(int wave) {
    pickups.clear();
    // Shotgun always at wave 2+
    if (wave >= 2)
        pickups.push_back({{ 10, 0.5f,  0}, WeaponType::SHOTGUN, true});
    // Rifle at wave 3+
    if (wave >= 3)
        pickups.push_back({{-10, 0.5f,  0}, WeaponType::RIFLE,   true});
    // Rocket at wave 5+
    if (wave >= 5)
        pickups.push_back({{  0, 0.5f,-10}, WeaponType::ROCKET,  true});
    // Extra pistol ammo crate always
    pickups.push_back({{ 5, 0.5f, 10}, WeaponType::PISTOL, true});
}

void Level::Draw(float dt) {
    DrawPlane({0,0,0}, {floorSize,floorSize}, {40,50,40,255});

    for (const auto& w : walls) {
        DrawCubeV(w.center, w.size, w.color);
        DrawCubeWiresV(w.center, w.size, {0,0,0,60});
    }

    // Draw weapon pickups with bobbing animation
    for (auto& p : pickups) {
        if (!p.active) continue;
        p.bobTimer += dt;
        float bob = sinf(p.bobTimer * 2.5f) * 0.15f;
        Vector3 drawPos = { p.position.x, p.position.y + bob + 0.4f, p.position.z };

        const WeaponDef& wd = GetWeaponDef(p.type);
        // Box representing the weapon
        DrawCube(drawPos, 0.5f, 0.18f, 0.18f, wd.bulletColor);
        DrawCubeWires(drawPos, 0.5f, 0.18f, 0.18f, WHITE);
        // Glow ring
        DrawCircle3D(drawPos, 0.42f, {1,0,0}, 0.0f, ColorAlpha(wd.bulletColor, 0.35f));

        // Label above pickup
        Vector2 screenPt = GetWorldToScreen(
            {drawPos.x, drawPos.y + 0.5f, drawPos.z},
            // camera passed via global — we'll draw label in game.cpp instead
            (Camera3D){}
        );
        (void)screenPt; // label drawn in game DrawScene using camera
    }
}

std::vector<BoundingBox> Level::GetWallBounds() const {
    std::vector<BoundingBox> out;
    out.reserve(walls.size());
    for (const auto& w : walls) out.push_back(w.bounds);
    return out;
}
