#pragma once
#include "raylib.h"
#include "World.h"
#include "RTSCamera.h"

// Структура для хранения информации о выделенном блоке
struct MouseBlockSelection {
    bool found = false;
    Vector3 blockPos = { 0, 0, 0 }; // Мировые координаты блока
    Vector3 faceNormal = { 0, 0, 0 }; // Нормаль грани, на которую наведен курсор (для постройки)
};

// Основной класс управления мышью для строительства/разрушения
class MouseControl {
public:
    MouseControl(World* world, RTS_Camera* camera);

    // Обновить состояние — вызвать каждый кадр
    void Update();

    // Нарисовать wireframe вокруг выбранного блока
    void DrawSelection() const;

private:
    World* world;
    RTS_Camera* camera;

    MouseBlockSelection currentSelection;

    // Raycast по миру — возвращает инфу о попадании в блок
    MouseBlockSelection RaycastBlock(float maxDistance = 6.0f) const;

    // Вспомогательные действия
    void TryDestroyBlock();
    void TryPlaceBlock();
};
#pragma once
