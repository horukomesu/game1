#include "RTSCamera.h"
#include <cmath>

RTS_Camera::RTS_Camera(int w, int h)
    : screenWidth(w), screenHeight(h),
    cameraDistance(50.0f), cameraRotation(45.0f), cameraTilt(60.0f),
    cameraTarget{ 0.0f, 0.0f, 0.0f }, cameraZoom(0.5f),
    minZoom(0.5f), maxZoom(5.0f),
    moveSpeed(0.5f), edgeSize(24.0f), rotSensitivity(0.3f),
    rotating(false), prevMouse{ 0 }
{
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = (float)h;
    camera.projection = CAMERA_PERSPECTIVE; // CAMERA_ORTHOGRAPHIC;
    bounds.enabled = false;
}

// Ограничения камеры по карте
void RTS_Camera::SetMapBounds(Vector2 minXZ, Vector2 maxXZ) {
    bounds.minXZ = minXZ;
    bounds.maxXZ = maxXZ;
    bounds.enabled = true;
}
void RTS_Camera::DisableMapBounds() {
    bounds.enabled = false;
}

// Главный апдейт — вызывается каждый кадр
void RTS_Camera::Update() {
    Vector2 mouse = GetMousePosition();
    Vector2 moveDir = { 0, 0 };

    // Edge scrolling
    if (mouse.x < edgeSize) moveDir.y = 1;
    else if (mouse.x > screenWidth - edgeSize) moveDir.y = -1;
    if (mouse.y < edgeSize) moveDir.x = -1;
    else if (mouse.y > screenHeight - edgeSize) moveDir.x = 1;

    // Клавиши
    if (IsKeyDown(KEY_RIGHT)) moveDir.y -= 1;
    if (IsKeyDown(KEY_LEFT))  moveDir.y += 1;
    if (IsKeyDown(KEY_UP))    moveDir.x -= 1;
    if (IsKeyDown(KEY_DOWN))  moveDir.x += 1;

    // Поворот по зажатой средней кнопке
    if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) {
        rotating = true;
        prevMouse = mouse;
    }
    if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON)) rotating = false;
    if (rotating) {
        float dx = mouse.x - prevMouse.x;
        cameraRotation += dx * rotSensitivity;
        prevMouse = mouse;
    }

    // Зум
    float wheel = GetMouseWheelMove()*-1;
    if (wheel != 0.0f) {
        cameraZoom *= 1.0f + (wheel * 0.2f);
        if (cameraZoom < minZoom) cameraZoom = minZoom;
        if (cameraZoom > maxZoom) cameraZoom = maxZoom;
    }

    // Движение камеры относительно поворота
    if (moveDir.x != 0 || moveDir.y != 0) {
        float angleRad = DEG2RAD * cameraRotation;
        float cosA = cosf(angleRad);
        float sinA = sinf(angleRad);

        float moveX = (moveDir.x * cosA - moveDir.y * sinA) * moveSpeed * cameraZoom;
        float moveY = (moveDir.x * sinA + moveDir.y * cosA) * moveSpeed * cameraZoom;

        cameraTarget.x += moveX;
        cameraTarget.z += moveY;
        ClampToBounds();
    }

    // Вычисление позиции камеры
    float rotRad = DEG2RAD * cameraRotation;
    float tiltRad = DEG2RAD * cameraTilt;

    Vector3 offset = {
        cameraDistance * cameraZoom * cosf(tiltRad) * cosf(rotRad),
        cameraDistance * cameraZoom * sinf(tiltRad),
        cameraDistance * cameraZoom * cosf(tiltRad) * sinf(rotRad)
    };
    camera.position.x = cameraTarget.x + offset.x;
    camera.position.y = cameraTarget.y + offset.y;
    camera.position.z = cameraTarget.z + offset.z;
    camera.target = cameraTarget;
    camera.fovy = ((float)screenWidth/50) * cameraZoom;

}

// Возвращает камеру для raylib
Camera3D RTS_Camera::GetCamera3D() const {
    return camera;
}

// Ограничения камеры (если включены)
void RTS_Camera::ClampToBounds() {
    if (bounds.enabled) {
        if (cameraTarget.x < bounds.minXZ.x) cameraTarget.x = bounds.minXZ.x;
        if (cameraTarget.x > bounds.maxXZ.x) cameraTarget.x = bounds.maxXZ.x;
        if (cameraTarget.z < bounds.minXZ.y) cameraTarget.z = bounds.minXZ.y;
        if (cameraTarget.z > bounds.maxXZ.y) cameraTarget.z = bounds.maxXZ.y;
    }
}
