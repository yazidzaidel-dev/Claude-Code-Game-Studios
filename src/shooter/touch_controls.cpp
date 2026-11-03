#include "touch_controls.h"
#include "raymath.h"
#include <cmath>
#include <algorithm>

void TouchControls::Update(int screenW, int screenH, float& yaw, float& pitch, bool& outShoot, bool& outSwitch) {
    (void)screenW; (void)screenH;
    outShoot = false;
    outSwitch = false;
    moveAxis = { 0, 0 };
    lookDelta = { 0, 0 };
    isShooting = false;

    int touchCount = GetTouchPointCount();
    bool leftTouchFound = false;
    bool rightTouchFound = false;

    for (int i = 0; i < touchCount; i++) {
        Vector2 pos = GetTouchPosition(i);
        int id = GetTouchPointId(i);

        // Left half: Virtual Joystick
        if (pos.x < (float)screenW * 0.45f) {
            leftTouchFound = true;
            if (!joystickActive) {
                joystickActive = true;
                joystickCenter = pos;
            }
            joystickTouch = pos;

            Vector2 diff = Vector2Subtract(joystickTouch, joystickCenter);
            float dist = Vector2Length(diff);
            if (dist > joystickRadius) {
                diff = Vector2Scale(Vector2Normalize(diff), joystickRadius);
                joystickTouch = Vector2Add(joystickCenter, diff);
            }
            moveAxis.x = diff.x / joystickRadius;
            moveAxis.y = -diff.y / joystickRadius;
        }
        else {
            // Right half: Shoot, Weapon Swap, or Look Drag
            if (Vector2Distance(pos, shootBtnCenter) <= shootBtnRadius + 20.0f) {
                isShooting = true;
                outShoot = true;
            }
            else if (Vector2Distance(pos, weaponBtnCenter) <= weaponBtnRadius + 20.0f) {
                outSwitch = true;
            }
            else {
                if (lookTouchId == -1) {
                    lookTouchId = id;
                    lastLookPos = pos;
                } else if (lookTouchId == id) {
                    Vector2 delta = Vector2Subtract(pos, lastLookPos);
                    lookDelta = delta;
                    lastLookPos = pos;
                    yaw += delta.x * 0.003f;
                    pitch -= delta.y * 0.003f;
                    pitch = std::clamp(pitch, -1.4f, 1.4f);
                }
                rightTouchFound = true;
            }
        }
    }

    if (!leftTouchFound) {
        joystickActive = false;
        joystickCenter = { 120.0f, (float)screenH - 120.0f };
        joystickTouch = joystickCenter;
        moveAxis = { 0, 0 };
    }

    if (!rightTouchFound) {
        lookTouchId = -1;
    }
}

void TouchControls::Draw(int screenW, int screenH) const {
    (void)screenH;
    // Joystick
    DrawCircleV(joystickCenter, joystickRadius, Color{60, 60, 80, 100});
    DrawCircleLines((int)joystickCenter.x, (int)joystickCenter.y, joystickRadius, Color{150, 150, 200, 160});
    DrawCircleV(joystickTouch, 28.0f, Color{200, 200, 240, 180});

    // Shoot Button
    Color shootCol = isShooting ? RED : Color{220, 50, 50, 160};
    DrawCircleV(shootBtnCenter, shootBtnRadius, shootCol);
    DrawCircleLines((int)shootBtnCenter.x, (int)shootBtnCenter.y, shootBtnRadius, WHITE);
    DrawText("FIRE", (int)shootBtnCenter.x - 22, (int)shootBtnCenter.y - 12, 18, WHITE);

    // Weapon Switch Button
    DrawCircleV(weaponBtnCenter, weaponBtnRadius, Color{50, 120, 220, 160});
    DrawCircleLines((int)weaponBtnCenter.x, (int)weaponBtnCenter.y, weaponBtnRadius, WHITE);
    DrawText("SWAP", (int)weaponBtnCenter.x - 24, (int)weaponBtnCenter.y - 10, 16, WHITE);
}
