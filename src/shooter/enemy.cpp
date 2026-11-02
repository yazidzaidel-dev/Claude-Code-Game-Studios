#include "enemy.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

static const float CHASE_RANGE  = 18.0f;
static const float ATTACK_RANGE = 10.0f;
static const int   ENEMY_DAMAGE = 8;
static const int   PARTICLE_COUNT = 18;

void Enemy::Init(Vector3 spawnPos, Vector3 patrolPt) {
    position     = spawnPos;
    patrolTarget = patrolPt;
    health       = maxHealth;
    state        = EnemyState::PATROL;
    shootTimer   = shootRate * 0.5f;
    dyingTimer   = 0.0f;
    fallAngle    = 0.0f;
    sinkY        = 0.0f;
    particles.clear();
}

void Enemy::StartDying() {
    state      = EnemyState::DYING;
    dyingTimer = 0.0f;
    SpawnParticles();
}

void Enemy::SpawnParticles() {
    particles.clear();
    particles.reserve(PARTICLE_COUNT);
    Color colors[] = { RED, ORANGE, YELLOW, {200,80,20,255}, {255,160,60,255} };
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        DeathParticle p;
        p.position = { position.x, position.y + 0.5f, position.z };
        // Random burst direction
        float az  = ((float)rand()/RAND_MAX) * 6.2832f;
        float el  = ((float)rand()/RAND_MAX) * 3.1416f - 0.4f;
        float spd = 3.0f + ((float)rand()/RAND_MAX) * 6.0f;
        p.velocity = {
            std::cos(az) * std::cos(el) * spd,
            std::sin(el) * spd + 2.0f,
            std::sin(az) * std::cos(el) * spd
        };
        p.maxLife = p.life = 0.35f + ((float)rand()/RAND_MAX) * 0.35f;
        p.color   = colors[rand() % 5];
        p.size    = 0.06f + ((float)rand()/RAND_MAX) * 0.12f;
        particles.push_back(p);
    }
}

void Enemy::UpdateParticles(float dt) {
    for (auto& p : particles) {
        p.life -= dt;
        p.velocity.y -= 12.0f * dt;  // gravity
        p.position = Vector3Add(p.position, Vector3Scale(p.velocity, dt));
        if (p.position.y < 0.05f) { p.position.y = 0.05f; p.velocity.y *= -0.3f; }
    }
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const DeathParticle& p){ return p.life <= 0.0f; }), particles.end());
}

void Enemy::UpdateDying(float dt) {
    dyingTimer += dt;
    float t = std::min(dyingTimer / dyingTotal, 1.0f);

    // Ease-in collapse: tilt forward and sink
    fallAngle = t * t * 1.5708f;   // 0 → π/2 (fall flat)
    sinkY     = t * t * 0.9f;      // sink into floor

    UpdateParticles(dt);

    if (dyingTimer >= dyingTotal)
        state = EnemyState::DEAD;
}

void Enemy::DrawParticles() const {
    for (const auto& p : particles) {
        float alpha = p.life / p.maxLife;
        Color c = { p.color.r, p.color.g, p.color.b, (unsigned char)(alpha * 255) };
        DrawSphere(p.position, p.size * alpha, c);
    }
}

void Enemy::Draw(const Camera3D& cam) const {
    if (state == EnemyState::DEAD && particles.empty()) return;

    if (state == EnemyState::DYING || state == EnemyState::DEAD) {
        DrawParticles();

        // Ragdoll: rotate the body cube around its base
        if (state == EnemyState::DYING) {
            // Draw a tilting/shrinking body
            float scale = 1.0f - (dyingTimer / dyingTotal) * 0.4f;
            Vector3 drawPos = {
                position.x,
                position.y - sinkY,
                position.z
            };
            // Tilt by drawing elongated cube with decreasing height
            float h = 1.8f * scale;
            Color dc = { 180, 40, 40, (unsigned char)(255 * (1.0f - dyingTimer/dyingTotal)) };
            DrawCube(drawPos, 0.8f, h, 0.8f, dc);
            DrawCubeWires(drawPos, 0.8f, h, 0.8f, {0,0,0,80});
        }
        return;
    }

    // Living enemy
    Color ec = (state == EnemyState::ATTACK) ? RED :
               (state == EnemyState::CHASE)  ? ORANGE : GRAY;

    // Flash red when health is low
    if (health < maxHealth / 3) {
        float flash = sinf((float)GetTime() * 12.0f);
        if (flash > 0) ec = RED;
    }

    DrawCube(position, 0.8f, 1.8f, 0.8f, ec);
    DrawCubeWires(position, 0.8f, 1.8f, 0.8f, BLACK);

    // Eyes (white dots facing player direction approximated)
    Vector3 eyeBase = { position.x, position.y + 0.5f, position.z };
    DrawSphere({eyeBase.x-0.15f, eyeBase.y, eyeBase.z-0.41f}, 0.07f, WHITE);
    DrawSphere({eyeBase.x+0.15f, eyeBase.y, eyeBase.z-0.41f}, 0.07f, WHITE);
    DrawSphere({eyeBase.x-0.15f, eyeBase.y, eyeBase.z-0.46f}, 0.04f, BLACK);
    DrawSphere({eyeBase.x+0.15f, eyeBase.y, eyeBase.z-0.46f}, 0.04f, BLACK);

    // Health bar (world space, handled in game.cpp screen-space)
    (void)cam;
}

void Enemy::Update(float dt, const Vector3& playerPos, int& playerHealth,
                   const std::vector<BoundingBox>& walls) {
    if (state == EnemyState::DYING || state == EnemyState::DEAD) {
        UpdateDying(dt);
        return;
    }

    float dist = Vector3Distance(position, playerPos);

    switch (state) {
        case EnemyState::PATROL: {
            Vector3 toTarget = Vector3Subtract(patrolTarget, position);
            if (Vector3Length(toTarget) < 0.5f)
                patrolTarget = Vector3Scale(patrolTarget, -1.0f);
            else
                position = Vector3Add(position,
                    Vector3Scale(Vector3Normalize(toTarget), speed * dt));
            if (dist < CHASE_RANGE) state = EnemyState::CHASE;
            break;
        }
        case EnemyState::CHASE: {
            Vector3 toPlayer = Vector3Subtract(playerPos, position);
            position = Vector3Add(position,
                Vector3Scale(Vector3Normalize(toPlayer), speed * dt));
            if (dist < ATTACK_RANGE)        state = EnemyState::ATTACK;
            if (dist > CHASE_RANGE * 1.5f)  state = EnemyState::PATROL;
            break;
        }
        case EnemyState::ATTACK: {
            shootTimer -= dt;
            if (shootTimer <= 0.0f) {
                playerHealth -= ENEMY_DAMAGE;
                if (playerHealth < 0) playerHealth = 0;
                shootTimer = shootRate;
            }
            if (dist > ATTACK_RANGE) state = EnemyState::CHASE;
            break;
        }
        default: break;
    }
}

void Enemy::TakeDamage(int dmg) {
    if (state == EnemyState::DYING || state == EnemyState::DEAD) return;
    health -= dmg;
    if (health < 0) health = 0;
    if (health == 0) StartDying();
}
