#pragma once
#include "raylib.h"
#include <string>

enum class WeaponType { PISTOL, SHOTGUN, RIFLE, ROCKET };

struct WeaponDef {
    WeaponType  type;
    const char* name;
    int         damage;
    int         ammoCapacity;
    float       fireRate;       // shots per second
    float       spread;         // bullet spread angle (radians), 0 = perfect
    int         bulletsPerShot; // shotgun = 6, others = 1
    float       bulletSpeed;
    Color       bulletColor;
};

// Weapon pickup dropped in the level
struct WeaponPickup {
    Vector3    position;
    WeaponType type;
    bool       active = true;
    float      bobTimer = 0.0f;  // for visual bobbing animation

    BoundingBox Bounds() const {
        return {
            { position.x - 0.4f, position.y - 0.4f, position.z - 0.4f },
            { position.x + 0.4f, position.y + 0.8f, position.z + 0.4f }
        };
    }
};

// Static weapon definitions
static const WeaponDef WEAPON_DEFS[] = {
    // type,           name,       dmg, ammo, rate,  spread,  bps, spd,  color
    { WeaponType::PISTOL,  "Pistol",   25,  12,  5.0f,  0.00f,   1,  40.0f, YELLOW  },
    { WeaponType::SHOTGUN, "Shotgun",  18,   4,  1.2f,  0.12f,   6,  35.0f, ORANGE  },
    { WeaponType::RIFLE,   "Rifle",    35,  30,  8.0f,  0.02f,   1,  60.0f, SKYBLUE },
    { WeaponType::ROCKET,  "Rocket",  120,   4,  0.8f,  0.00f,   1,  25.0f, RED     },
};

inline const WeaponDef& GetWeaponDef(WeaponType t) {
    return WEAPON_DEFS[(int)t];
}
