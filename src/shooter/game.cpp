#include "game.h"
#include "weapon.h"
#include <algorithm>
#include <cstdlib>
#include <cmath>

static const float ROCKET_SPLASH_RADIUS = 6.0f;
static const int   ROCKET_SPLASH_DAMAGE = 80;

void Game::Init() {
    level.Build();
    player.Init();
    level.SpawnPickups(wave);

    camera.fovy       = 75.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    camera.up         = {0,1,0};

    SpawnWave();
    audio.PlayWaveStart();
    state = GameState::PLAYING;
}

void Game::SpawnWave() {
    enemies.clear();
    int count = 2 + wave * 2;
    auto& pts = level.spawnPoints;
    for (int i = 0; i < count; i++) {
        auto& sp = pts[i % pts.size()];
        Enemy e;
        e.Init(sp.position, sp.patrolTarget);
        e.speed    = 3.0f + wave * 0.4f;
        e.health   = 50  + wave * 10;
        e.shootRate = std::max(0.6f, 1.8f - wave * 0.1f);
        enemies.push_back(e);
    }
}

void Game::UpdateCamera() {
    camera.position = player.EyePos();
    camera.target   = Vector3Add(player.EyePos(), player.direction);
}

void Game::CheckCollisions() {
    auto walls = level.GetWallBounds();

    // --- Bullets vs walls & enemies ---
    for (auto& b : player.bullets) {
        if (!b.active) continue;
        BoundingBox bb = {
            {b.position.x-b.radius, b.position.y-b.radius, b.position.z-b.radius},
            {b.position.x+b.radius, b.position.y+b.radius, b.position.z+b.radius}
        };

        bool hitWall = false;
        for (auto& w : walls) {
            if (CheckCollisionBoxes(bb, w)) { hitWall = true; break; }
        }

        bool hitEnemy = false;
        if (!hitWall) {
            for (auto& e : enemies) {
                if (e.IsDead()) continue;
                if (CheckCollisionBoxes(bb, e.Bounds())) {
                    hitEnemy = true;
                    bool wasDying = e.IsDead();
                    e.TakeDamage(b.damage);
                    if (!wasDying) {
                        if (e.IsDead()) { audio.PlayEnemyDie(); score += 100 * wave; }
                        else              audio.PlayEnemyHit();
                    }
                    break;
                }
            }
        }

        // Rocket splash: detonate on wall OR enemy hit
        bool isRocket = (b.damage >= 100);
        if (isRocket && (hitWall || hitEnemy)) {
            // Splash damage all enemies in radius
            for (auto& e : enemies) {
                if (e.IsDead()) continue;
                float dist = Vector3Distance(b.position, e.position);
                if (dist <= ROCKET_SPLASH_RADIUS) {
                    // Falloff: full damage at 0, 0 at edge
                    float falloff = 1.0f - (dist / ROCKET_SPLASH_RADIUS);
                    int splashDmg = (int)(ROCKET_SPLASH_DAMAGE * falloff);
                    if (splashDmg > 0) {
                        bool wasDying = e.IsDead();
                        e.TakeDamage(splashDmg);
                        if (!wasDying) {
                            if (e.IsDead()) { audio.PlayEnemyDie(); score += 100 * wave; }
                            else              audio.PlayEnemyHit();
                        }
                    }
                }
            }
            // Self-damage if rocket explodes near player
            float selfDist = Vector3Distance(b.position, player.position);
            if (selfDist <= ROCKET_SPLASH_RADIUS) {
                float falloff = 1.0f - (selfDist / ROCKET_SPLASH_RADIUS);
                int selfDmg = (int)(ROCKET_SPLASH_DAMAGE * falloff * 0.5f);
                if (selfDmg > 0) player.TakeDamage(selfDmg);
            }
        }

        if (hitWall || hitEnemy) b.active = false;
    }

    // --- Player vs weapon pickups ---
    BoundingBox playerBox = {
        {player.position.x-0.4f, player.position.y-1.8f, player.position.z-0.4f},
        {player.position.x+0.4f, player.position.y+0.2f, player.position.z+0.4f}
    };
    for (auto& p : level.pickups) {
        if (!p.active) continue;
        if (CheckCollisionBoxes(playerBox, p.Bounds())) {
            player.PickupWeapon(p.type);
            p.active = false;
            audio.PlayPickup();
        }
    }
}

void Game::Update(float dt) {
    // Ambient drone loop (replayed every ~4s)
    ambientTimer -= dt;
    if (ambientTimer <= 0.0f && audio.ready) {
        PlaySound(audio.shoot); // placeholder — will use waveStart as ambient
        ambientTimer = 999.0f; // don't re-trigger; ambient only on wave start
    }

    if (state == GameState::MENU) {
        if (IsKeyPressed(KEY_ENTER)) { score=0; wave=1; Init(); }
        return;
    }
    if (state == GameState::GAME_OVER) {
        if (IsKeyPressed(KEY_R))      { score=0; wave=1; Init(); }
        if (IsKeyPressed(KEY_ESCAPE)) state = GameState::MENU;
        return;
    }
    if (IsKeyPressed(KEY_ESCAPE))
        state = (state==GameState::PAUSED) ? GameState::PLAYING : GameState::PAUSED;
    if (state == GameState::PAUSED) return;

    auto walls = level.GetWallBounds();
    bool fired = player.Update(dt, walls);
    if (fired)                    audio.PlayShoot();
    if (player.dryfireThisFrame)  audio.PlayDryFire();
    if (player.hurtFlash > 0.15f) audio.PlayPlayerHurt(); // triggered edge

    UpdateCamera();

    for (auto& e : enemies)
        e.Update(dt, player.position, player.health, walls);

    CheckCollisions();

    // Remove enemies that have fully finished their death animation
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const Enemy& e){ return e.IsGone(); }), enemies.end());

    // Wave clear: all enemies gone (finished death animations too)
    if (enemies.empty()) {
        wave++;
        player.ammo = std::min(player.ammo + GetWeaponDef(player.currentWeapon).ammoCapacity, 99);
        level.SpawnPickups(wave);
        SpawnWave();
        audio.PlayWaveStart();
    }

    if (player.IsDead()) state = GameState::GAME_OVER;
}

void Game::DrawScene() {
    BeginMode3D(camera);
        level.Draw(GetFrameTime());

        // Enemies (uses Enemy::Draw for death animation + particles)
        for (const auto& e : enemies) {
            e.Draw(camera);
        }

        // Bullets
        for (const auto& b : player.bullets) {
            if (!b.active) continue;
            DrawSphere(b.position, b.radius, b.color);
        }
    EndMode3D();

    // Enemy health bars (screen-space, after EndMode3D, only for living enemies)
    for (const auto& e : enemies) {
        if (e.IsDead()) continue;
        Vector2 sp = GetWorldToScreen({e.position.x, e.position.y+1.4f, e.position.z}, camera);
        if (sp.x > 10 && sp.x < GetScreenWidth()-10 && sp.y > 10 && sp.y < GetScreenHeight()-10) {
            DrawRectangle((int)sp.x-25,(int)sp.y-6, 50, 8, DARKGRAY);
            DrawRectangle((int)sp.x-25,(int)sp.y-6,
                (int)(50.0f*e.health/e.maxHealth), 8, RED);
        }
    }
    for (const auto& p : level.pickups) {
        if (!p.active) continue;
        Vector2 sp = GetWorldToScreen({p.position.x, p.position.y+1.2f, p.position.z}, camera);
        if (sp.x > 10 && sp.x < GetScreenWidth()-10 && sp.y > 10) {
            const char* nm = GetWeaponDef(p.type).name;
            DrawText(nm, (int)sp.x - MeasureText(nm,16)/2, (int)sp.y, 16, YELLOW);
        }
    }
}

void Game::Draw() {
    switch (state) {
        case GameState::MENU:      DrawMenu();    break;
        case GameState::GAME_OVER: DrawGameOver();break;
        case GameState::PAUSED:
            DrawScene();
            hud.Draw(player.health, player.ammo, score, wave,
                     GetWeaponDef(player.currentWeapon).name,
                     GetScreenWidth(), GetScreenHeight(), player.hurtFlash);
            minimap.Draw(player.position, player.yaw, enemies, level.pickups,
                         level.GetWallBounds(), GetScreenWidth(), GetScreenHeight());
            DrawPause();
            break;
        case GameState::PLAYING:
            DrawScene();
            hud.Draw(player.health, player.ammo, score, wave,
                     GetWeaponDef(player.currentWeapon).name,
                     GetScreenWidth(), GetScreenHeight(), player.hurtFlash);
            minimap.Draw(player.position, player.yaw, enemies, level.pickups,
                         level.GetWallBounds(), GetScreenWidth(), GetScreenHeight());
            break;
    }
}

void Game::DrawMenu() {
    ClearBackground({10,10,20,255});
    int w=GetScreenWidth(), h=GetScreenHeight();
    DrawText("CCGS SHOOTER",     w/2-140, h/2-80, 48, YELLOW);
    DrawText("Wave-based FPS",   w/2-90,  h/2-20, 24, LIGHTGRAY);
    DrawText("[ENTER] Start",    w/2-80,  h/2+30, 26, WHITE);
    DrawText("WASD Move | Mouse Aim | LMB Shoot | Walk over weapons to pick up",
             w/2-280, h/2+80, 16, GRAY);
}

void Game::DrawPause() {
    int w=GetScreenWidth(), h=GetScreenHeight();
    DrawRectangle(0,0,w,h,{0,0,0,120});
    DrawText("PAUSED",       w/2-70, h/2-40, 48, WHITE);
    DrawText("[ESC] Resume", w/2-80, h/2+20, 24, LIGHTGRAY);
}

void Game::DrawGameOver() {
    ClearBackground({15,0,0,255});
    int w=GetScreenWidth(), h=GetScreenHeight();
    DrawText("GAME OVER",                              w/2-130, h/2-80, 52, RED);
    DrawText(TextFormat("Score: %d  Wave: %d",score,wave), w/2-120, h/2,   28, WHITE);
    DrawText("[R] Restart  [ESC] Menu",                w/2-140, h/2+60, 24, LIGHTGRAY);
}

void Game::Shutdown() {
    enemies.clear();
    player.bullets.clear();
    audio.Shutdown();
}
