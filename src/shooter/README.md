# CCGS Shooter — Native 3D FPS

A native C++17 first-person shooter built with **Raylib 5.0**, targeting Linux/ARM (Termux/PRoot).

## Architecture

```
src/shooter/
├── main.cpp      — Entry point, window + game loop
├── game.h/.cpp   — Top-level orchestrator (state machine, wave spawning, collision)
├── player.h/.cpp — FPS movement, mouse-look, bullet firing (WASD + mouse)
├── enemy.h/.cpp  — AI state machine: PATROL → CHASE → ATTACK
├── level.h/.cpp  — Arena geometry, wall BoundingBoxes, spawn points
├── hud.h/.cpp    — Crosshair, health bar, ammo, score, wave counter
└── Makefile      — Build system
```

## Controls

| Key / Input     | Action          |
|-----------------|-----------------|
| `WASD`          | Move            |
| Mouse           | Look / Aim      |
| Left Mouse Btn  | Shoot           |
| `ESC`           | Pause / Unpause |
| `R` (game over) | Restart         |

## Build

```bash
# Raylib must be installed first (builds into /usr/local):
cd /tmp && git clone --depth 1 --branch 5.0 https://github.com/raysan5/raylib.git raylib-src
cd raylib-src/src && make PLATFORM=PLATFORM_DESKTOP
cp libraylib.a /usr/local/lib/ && cp raylib.h raymath.h rlgl.h /usr/local/include/

# Then build the game:
cd /root/Claude-Code-Game-Studios/src/shooter
make
./ccgs_shooter
```

## Gameplay

- Survive wave-based enemy assaults
- Each wave: more enemies, faster speed, higher HP, faster shooting
- Enemies patrol, then chase and attack once in range
- Pick up ammo by clearing waves (+30 per wave)
- Score: 100 × wave per kill

## Design Notes

- **Collision**: AABB wall-slide (player can hug walls without sticking)
- **Camera**: True first-person, 75° FOV, pitch clamped ±80°
- **Enemy AI**: Simple 3-state FSM — expandable to add flanking, cover-seeking
- **No external assets** — fully procedural geometry, colour-coded by entity type
