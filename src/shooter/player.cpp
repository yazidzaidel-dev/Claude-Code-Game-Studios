#include "player.h"
#include <cmath>
#include <algorithm>

static const float MOUSE_SENS = 0.002f;

void Player::Init() {
    position       = { 0.0f, 1.8f, 0.0f };
    yaw            = 0.0f;
    pitch          = 0.0f;
    health         = 100;
    currentWeapon  = WeaponType::PISTOL;
    ammo           = GetWeaponDef(currentWeapon).ammoCapacity;
    hurtFlash      = 0.0f;
    bullets.clear();
}

void Player::PickupWeapon(WeaponType t) {
    currentWeapon = t;
    ammo = GetWeaponDef(t).ammoCapacity;
}

void Player::Shoot() {
    const WeaponDef& wd = Weapon();
    if (ammo <= 0) return;
    for (int i = 0; i < wd.bulletsPerShot; i++) {
        Bullet b;
        b.position = EyePos();
        b.damage   = wd.damage;
        b.color    = wd.bulletColor;
        b.radius   = (wd.type == WeaponType::ROCKET) ? 0.18f : 0.06f;

        // Apply spread
        float sx = wd.spread * (((float)rand()/RAND_MAX)*2.0f - 1.0f);
        float sy = wd.spread * (((float)rand()/RAND_MAX)*2.0f - 1.0f);
        Vector3 spreadDir = Vector3Normalize({
            direction.x + sx,
            direction.y + sy,
            direction.z
        });
        b.velocity = Vector3Scale(spreadDir, wd.bulletSpeed);
        b.lifetime = 2.0f;
        b.active   = true;
        bullets.push_back(b);
    }
    ammo--;
}

void Player::TakeDamage(int dmg) {
    health -= dmg;
    if (health < 0) health = 0;
    hurtFlash = 0.18f;
}

bool Player::Update(float dt, const std::vector<BoundingBox>& walls) {
    firedThisFrame    = false;
    dryfireThisFrame  = false;

    // --- Mouse look ---
    Vector2 md = GetMouseDelta();
    yaw   += md.x * MOUSE_SENS;
    pitch -= md.y * MOUSE_SENS;
    pitch  = std::clamp(pitch, -1.4f, 1.4f);

    direction = Vector3Normalize({
        std::sin(yaw) * std::cos(pitch),
        std::sin(pitch),
        std::cos(yaw) * std::cos(pitch)
    });
    Vector3 right = Vector3Normalize(Vector3CrossProduct(direction, {0,1,0}));

    // --- Movement ---
    Vector3 move   = {0,0,0};
    Vector3 flatFwd = Vector3Normalize({direction.x, 0, direction.z});
    if (IsKeyDown(KEY_W)) move = Vector3Add(move, flatFwd);
    if (IsKeyDown(KEY_S)) move = Vector3Subtract(move, flatFwd);
    if (IsKeyDown(KEY_A)) move = Vector3Subtract(move, right);
    if (IsKeyDown(KEY_D)) move = Vector3Add(move, right);

    if (Vector3Length(move) > 0.001f) {
        move = Vector3Scale(Vector3Normalize(move), speed * dt);
        Vector3 np = Vector3Add(position, move);
        BoundingBox pbX = {{ np.x-0.4f, np.y-1.8f, position.z-0.4f },
                           { np.x+0.4f, np.y+0.2f, position.z+0.4f }};
        BoundingBox pbZ = {{ position.x-0.4f, np.y-1.8f, np.z-0.4f },
                           { position.x+0.4f, np.y+0.2f, np.z+0.4f }};
        bool hitX = false, hitZ = false;
        for (auto& w : walls) {
            if (CheckCollisionBoxes(pbX, w)) hitX = true;
            if (CheckCollisionBoxes(pbZ, w)) hitZ = true;
        }
        if (!hitX) position.x = np.x;
        if (!hitZ) position.z = np.z;
    }

    // --- Shoot ---
    const WeaponDef& wd = Weapon();
    shootCooldown -= dt;
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && shootCooldown <= 0.0f) {
        if (ammo > 0) {
            Shoot();
            shootCooldown  = 1.0f / wd.fireRate;
            firedThisFrame = true;
        } else {
            dryfireThisFrame = true;
            shootCooldown = 0.25f;
        }
    }

    // --- Update bullets ---
    for (auto& b : bullets) {
        b.position = Vector3Add(b.position, Vector3Scale(b.velocity, dt));
        b.lifetime -= dt;
        if (b.lifetime <= 0.0f) b.active = false;
    }
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b){ return !b.active; }), bullets.end());

    // Hurt flash decay
    if (hurtFlash > 0.0f) hurtFlash -= dt;

    return firedThisFrame;
}
