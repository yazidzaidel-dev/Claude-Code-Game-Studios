#pragma once
#include "raylib.h"

struct TouchControls {
    // Virtual joystick (left side of screen)
    Vector2 joystickCenter = { 120.0f, 600.0f };
    float   joystickRadius = 70.0f;
    Vector2 joystickTouch  = { 120.0f, 600.0f };
    bool    joystickActive = false;
    Vector2 moveAxis       = { 0.0f, 0.0f }; // x = strafe, y = forward

    // Action buttons (right side of screen)
    // Shoot button
    Vector2 shootBtnCenter = { 1140.0f, 580.0f };
    float   shootBtnRadius = 55.0f;
    bool    isShooting     = false;

    // Weapon switch button
    Vector2 weaponBtnCenter = { 1140.0f, 440.0f };
    float   weaponBtnRadius = 40.0f;

    // Touch look tracking (right side drag to aim)
    int     lookTouchId    = -1;
    Vector2 lastLookPos    = { 0, 0 };
    Vector2 lookDelta      = { 0, 0 };

    void Update(int screenW, int screenH, float& yaw, float& pitch, bool& outShoot, bool& outSwitch);
    void Draw(int screenW, int screenH) const;
};
