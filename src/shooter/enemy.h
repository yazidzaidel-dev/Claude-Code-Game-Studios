#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>

enum class EnemyState { PATROL, CHASE, ATTACK, DYING, DEAD };

// One particle in a death burst
struct DeathParticle {
    Vector3 position;
    Vector3 velocity;
    float   life;       // remaining seconds
    float   maxLife;
    Color   color;
    float   size;
};

class Enemy {
public:
    Vector3     position;
    Vector3     patrolTarget;
    int         health      = 50;
    int         maxHealth   = 50;
    float       speed       = 3.5f;
    float       shootTimer  = 0.0f;
    float       shootRate   = 1.8f;
    EnemyState  state       = EnemyState::PATROL;

    // Death animation state
    float   dyingTimer  = 0.0f;        // counts up from 0
    float   dyingTotal  = 0.65f;       // total collapse duration
    float   fallAngle   = 0.0f;        // ragdoll tilt angle (radians)
    float   sinkY       = 0.0f;        // body sinks into floor
    std::vector<DeathParticle> particles;

    BoundingBox Bounds() const {
        return {
            { position.x-0.4f, position.y-0.9f, position.z-0.4f },
            { position.x+0.4f, position.y+0.9f, position.z+0.4f }
        };
    }

    void Init(Vector3 spawnPos, Vector3 patrolPt);
    void Update(float dt, const Vector3& playerPos, int& playerHealth,
                const std::vector<BoundingBox>& walls);
    void Draw(const Camera3D& cam) const;
    void TakeDamage(int dmg);
    void StartDying();
    bool IsDead()    const { return state == EnemyState::DEAD;   }
    bool IsGone()    const { return state == EnemyState::DEAD && dyingTimer > dyingTotal + 0.1f; }

private:
    void SpawnParticles();
    void UpdateParticles(float dt);
    void DrawParticles() const;
    void UpdateDying(float dt);
};
