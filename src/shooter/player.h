#pragma once
#include "raylib.h"
#include "raymath.h"
#include "weapon.h"
#include <vector>

struct Bullet {
    Vector3    position;
    Vector3    velocity;
    float      lifetime  = 2.0f;
    bool       active    = true;
    int        damage    = 25;
    Color      color     = YELLOW;
    float      radius    = 0.06f;
};

class Player {
public:
    Vector3  position  = { 0.0f, 1.8f, 0.0f };
    Vector3  direction = { 0.0f, 0.0f,-1.0f };
    float    yaw       = 0.0f;
    float    pitch     = 0.0f;
    int      health    = 100;
    int      ammo      = 12;      // current weapon ammo
    float    speed     = 8.0f;
    float    shootCooldown  = 0.0f;
    float    hurtFlash      = 0.0f; // screen-flash timer
    bool     firedThisFrame   = false;
    bool     dryfireThisFrame = false;
    bool     switchWeaponReq  = false;

    WeaponType currentWeapon = WeaponType::PISTOL;

    std::vector<Bullet> bullets;

    void Init();
    bool Update(float dt, const std::vector<BoundingBox>& walls, Vector2 touchMoveAxis, bool touchShoot, bool touchSwitch);
    void Shoot();
    void TakeDamage(int dmg);
    void PickupWeapon(WeaponType t);
    bool IsDead() const { return health <= 0; }

    Vector3 EyePos() const { return position; }
    const WeaponDef& Weapon() const { return GetWeaponDef(currentWeapon); }
};
