#include "audio.h"
#include <cmath>
#include <cstring>

// Helper: generate a raw PCM wave and load it as a Sound
static Sound MakeSound(int sampleRate, float durationSec,
                        float (*generator)(float t, float dur, void* ctx), void* ctx) {
    int sampleCount = (int)(sampleRate * durationSec);
    short* samples = new short[sampleCount];
    for (int i = 0; i < sampleCount; i++) {
        float t = (float)i / sampleRate;
        float v = generator(t, durationSec, ctx);
        if (v >  1.0f) v =  1.0f;
        if (v < -1.0f) v = -1.0f;
        samples[i] = (short)(v * 28000.0f);
    }
    Wave w = {};
    w.frameCount = sampleCount;
    w.sampleRate = sampleRate;
    w.sampleSize = 16;
    w.channels   = 1;
    w.data       = samples;
    Sound s = LoadSoundFromWave(w);
    delete[] samples;
    return s;
}

// --- Generator functions ---

static float GenShoot(float t, float dur, void*) {
    // Sharp click → white-noise decay
    float env = expf(-t * 22.0f);
    float noise = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
    float tone  = sinf(t * 1200.0f * 6.28f) * expf(-t * 40.0f);
    return (noise * 0.6f + tone * 0.4f) * env;
}

static float GenEnemyHit(float t, float dur, void*) {
    float env = expf(-t * 18.0f);
    float noise = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
    float tone  = sinf(t * 400.0f * 6.28f) * expf(-t * 25.0f);
    return (noise * 0.4f + tone * 0.6f) * env;
}

static float GenEnemyDie(float t, float dur, void*) {
    float env = expf(-t * 8.0f);
    float freq = 300.0f - t * 600.0f;
    if (freq < 60.0f) freq = 60.0f;
    float tone  = sinf(t * freq * 6.28f);
    float noise = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
    return (tone * 0.5f + noise * 0.5f) * env;
}

static float GenPlayerHurt(float t, float dur, void*) {
    float env = expf(-t * 6.0f);
    float freq = 200.0f + sinf(t * 25.0f) * 80.0f;
    return sinf(t * freq * 6.28f) * env * 0.8f;
}

static float GenPickup(float t, float dur, void*) {
    // Rising arpeggio: three quick tones
    float freqs[3] = { 440.0f, 554.0f, 660.0f };
    int step = (int)(t / (dur / 3.0f));
    if (step > 2) step = 2;
    float env = expf(-(t - step * (dur/3.0f)) * 20.0f);
    return sinf(t * freqs[step] * 6.28f) * env * 0.7f;
}

static float GenWaveStart(float t, float dur, void*) {
    // Low rumble swell
    float env = (t < dur * 0.3f) ? t / (dur * 0.3f) : expf(-(t - dur * 0.3f) * 3.0f);
    float tone = sinf(t * 80.0f * 6.28f) + 0.5f * sinf(t * 160.0f * 6.28f);
    return tone * env * 0.4f;
}

static float GenDryFire(float t, float dur, void*) {
    float env = expf(-t * 60.0f);
    float noise = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
    return noise * env * 0.6f;
}


void AudioManager::Init() {
    InitAudioDevice();
    if (!IsAudioDeviceReady()) { ready = false; return; }

    const int SR = 44100;
    shoot      = MakeSound(SR, 0.18f, GenShoot,      nullptr);
    enemyHit   = MakeSound(SR, 0.15f, GenEnemyHit,   nullptr);
    enemyDie   = MakeSound(SR, 0.35f, GenEnemyDie,   nullptr);
    playerHurt = MakeSound(SR, 0.30f, GenPlayerHurt,  nullptr);
    pickup     = MakeSound(SR, 0.24f, GenPickup,      nullptr);
    waveStart  = MakeSound(SR, 0.80f, GenWaveStart,   nullptr);
    dryFire    = MakeSound(SR, 0.05f, GenDryFire,     nullptr);

    // Ambient handled by re-triggering waveStart sound from Game::Update
    // (Music streaming requires a file on disk; we skip it here)

    ready = true;
}

void AudioManager::Shutdown() {
    if (!ready) return;
    UnloadSound(shoot);
    UnloadSound(enemyHit);
    UnloadSound(enemyDie);
    UnloadSound(playerHurt);
    UnloadSound(pickup);
    UnloadSound(waveStart);
    UnloadSound(dryFire);
    CloseAudioDevice();
    ready = false;
}
