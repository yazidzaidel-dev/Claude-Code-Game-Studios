#pragma once
#include "raylib.h"
#include "weapon.h"
#include <vector>

struct Wall {
    Vector3     center;
    Vector3     size;
    Color       color;
    BoundingBox bounds;
};

struct SpawnPoint {
    Vector3 position;
    Vector3 patrolTarget;
};

class Level {
public:
    std::vector<Wall>         walls;
    std::vector<SpawnPoint>   spawnPoints;
    std::vector<WeaponPickup> pickups;
    float                     floorSize = 60.0f;

    void Build();
    void Draw(float dt);
    std::vector<BoundingBox> GetWallBounds() const;
    void SpawnPickups(int wave);

private:
    void AddWall(Vector3 center, Vector3 size, Color color);
};
