#pragma once
#include "raylib.h"

// Procedurally generated sounds — no asset files needed
class AudioManager {
public:
    Sound shoot;
    Sound enemyHit;
    Sound enemyDie;
    Sound playerHurt;
    Sound pickup;
    Sound waveStart;
    Sound dryFire;    // empty gun click

    bool ready = false;

    void Init();
    void Shutdown();

    void PlayShoot()      { if (ready) PlaySound(shoot); }
    void PlayEnemyHit()   { if (ready) PlaySound(enemyHit); }
    void PlayEnemyDie()   { if (ready) PlaySound(enemyDie); }
    void PlayPlayerHurt() { if (ready) PlaySound(playerHurt); }
    void PlayPickup()     { if (ready) PlaySound(pickup); }
    void PlayWaveStart()  { if (ready) PlaySound(waveStart); }
    void PlayDryFire()    { if (ready) PlaySound(dryFire); }
};
